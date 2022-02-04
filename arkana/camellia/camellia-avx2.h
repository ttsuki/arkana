/// @file
/// @brief	arkana::camellia
///			- An implementation of Camellia cipher
/// @author Copyright(c) 2021 ttsuki
/// 
/// This software is released under the MIT License.
/// https://opensource.org/licenses/MIT
///
/// - camellia https://info.isl.ntt.co.jp/crypt/camellia/ 

#pragma once

#include "camellia.h"

#include "../bits/xmm.h"

namespace arkana::camellia
{
    namespace avx2
    {
        namespace impl
        {
            using namespace arkana::xmm;

            template <template <class T> class YMM, class T>
            ARKXMM_API transpose_32x4x4(YMM<T>& x0, YMM<T>& x1, YMM<T>& x2, YMM<T>& x3)
            {
                auto i0 = reinterpret<YMM<uint32_t>>(x0);                // i0 = | 00010203|04050607|08090A0B|0C0D0E0F |
                auto i1 = reinterpret<YMM<uint32_t>>(x1);                // i1 = | 10111213|14151617|18191A1B|1C1D1E1F |
                auto i2 = reinterpret<YMM<uint32_t>>(x2);                // i2 = | 20212223|24252627|28292A2B|2C2D2E2F |
                auto i3 = reinterpret<YMM<uint32_t>>(x3);                // i3 = | 30313233|34353637|38393A3B|3C3D3E3F |
                auto t0 = reinterpret<YMM<uint64_t>>(unpack_lo(i0, i1)); // t0 = | 00010203 10111213|04050607 14151617 |
                auto t1 = reinterpret<YMM<uint64_t>>(unpack_hi(i0, i1)); // t1 = | 08090A0B 18191A1B|0C0D0E0F 1C1D1E1F |
                auto t2 = reinterpret<YMM<uint64_t>>(unpack_lo(i2, i3)); // t2 = | 20210223 30313233|24252627 34353637 |
                auto t3 = reinterpret<YMM<uint64_t>>(unpack_hi(i2, i3)); // t3 = | 28290A2B 38393A3B|2C2D2E2F 3C3D3E3F |
                x0 = reinterpret<YMM<T>>(unpack_lo(t0, t2));             // x0 = | 00010203 10111213 20212223 30313233 |
                x1 = reinterpret<YMM<T>>(unpack_hi(t0, t2));             // x1 = | 04050607 14151617 24252627 34353637 |
                x2 = reinterpret<YMM<T>>(unpack_lo(t1, t3));             // x2 = | 08090A0B 18191A1B 28292A2B 38393A3B |
                x3 = reinterpret<YMM<T>>(unpack_hi(t1, t3));             // x3 = | 0C0D0E0F 1C1D1E1F 2C2D2E2F 3C3D3E3F |
            }

            ARKXMM_API rotl_be1(vu32x8 v) -> vu32x8
            {
                /* Big endian rotl(v, 1) */                                                    //  BE: 01234567 89abcdef ghijklmn opqrstuv
                /* on little endian      */                                                    //  ->: 12345678 9abcdefg hijklmno pqrstuv0
                /* input on LE */                                                              //  LE: opqrstuv ghijklmn 89abcdef 01234567   
                /* target on LE */                                                             //  ->: pqrstuv0 hijklmno 9abcdefg 12345678   
                auto x = reinterpret<vu32x8>(abs(reinterpret<vi8x32>(v) < zero<vi8x32>()));    //x   = _______o _______g _______8 _______0
                auto a = rotl(x, 24);                                                          //a   = _______0 _______o _______g _______8
                auto b = reinterpret<vu32x8>(reinterpret<vu8x32>(v) + reinterpret<vu8x32>(v)); //  b = pqrstuv_ hijklmn_ 9abcdef_ 1234567_
                return a | b;                                                                  //a|b = pqrstuv0 hijklmno 9abcdefg 12345678
            }

            using v32 = vu32x8;

            struct v64
            {
                v32 l, r;
            };

            struct v128
            {
                v64 l, r;
            };

            ARKXMM_API decompose(v64& x) noexcept -> v64& { return x; }
            ARKXMM_API recompose(v64& t, v32 l, v32 r) noexcept -> v64& { return t.l = l, t.r = r, t; }
            ARKXMM_API operator ^=(v64& lhs, v64 rhs) noexcept -> v64& { return lhs.l ^= rhs.l, lhs.r ^= rhs.r, lhs; }

            using camellia::impl::sbox::sbox_t;

            ARKXMM_API lookup_sbox32(const sbox_t<uint32_t>& sbox, vu32x8 index, int shift) -> vu32x8
            {
                return xmm::gather<vu32x8>(sbox.data(), extract_byte<0>(index >> shift * 8));
            }

            using camellia::impl::camellia_f_table_lookup_32;
            using camellia::impl::camellia_fl;
            using camellia::impl::camellia_fl_inv;

            using camellia::impl::key_t;
            using camellia::impl::key_vector_t;
            using camellia::impl::generate_key_vector_inlined;
            using camellia::impl::process_block_inlined;


            ARKXMM_API camellia_prewhite(v128& block, uint64_t kl, uint64_t kr) -> v128&
            {
                const vu32x8 kx = reinterpret<vu32x8>(u64x4(kl, kr));
                block.l.l ^= kx;
                block.l.r ^= kx;
                block.r.l ^= kx;
                block.r.r ^= kx;
                transpose_32x4x4(block.l.l, block.l.r, block.r.l, block.r.r);
                return block;
            }

            ARKXMM_API camellia_postwhite(v128& block, v64& l, v64& r, uint64_t kl, uint64_t kr) -> v128&
            {
                const vu32x8 kx = reinterpret<vu32x8>(u64x4(kl, kr));

                transpose_32x4x4(r.l, r.r, l.l, l.r);

                l.l ^= kx;
                l.r ^= kx;
                r.l ^= kx;
                r.r ^= kx;

                l.l ^= r.l;
                l.r ^= r.r;
                r.l ^= l.l;
                r.r ^= l.r;
                l.l ^= r.l;
                l.r ^= r.r;

                block.l = l;
                block.r = r;
                return block;
            }

            ARKXMM_API load_v128(const v128* src) -> v128
            {
                return v128{
                    {xmm::load_u(&src->l.l), xmm::load_u(&src->l.r)},
                    {xmm::load_u(&src->r.l), xmm::load_u(&src->r.r)},
                };
            }

            ARKXMM_API store_v128(v128* dst, const v128& reg)
            {
                xmm::store_u(&dst->l.l, reg.l.l);
                xmm::store_u(&dst->l.r, reg.l.r);
                xmm::store_u(&dst->r.l, reg.r.l);
                xmm::store_u(&dst->r.r, reg.r.r);
            }
        }

        using unit_t = impl::v128; // 128 bytes per process
        template <size_t key_bits> using key_t = impl::key_t<key_bits>;
        template <size_t key_bits> using key_vector_t = impl::key_vector_t<uint64_t, key_bits>;

        template <size_t key_bits, bool for_encryption>
        static key_vector_t<key_bits> generate_key_vector(const key_t<key_bits>& key)
        {
            return impl::generate_key_vector_inlined<key_bits, for_encryption>(key);
        }

        template <size_t key_bits>
        static void process_blocks_ecb(unit_t* dst, const unit_t* src, size_t unit_count, const key_vector_t<key_bits>& kv)
        {
            for (size_t i = 0; i < unit_count; i++)
            {
                impl::v128 b = impl::load_v128(src + i);
                b = impl::process_block_inlined<
                    impl::v128,
                    uint64_t,
                    impl::camellia_prewhite,
                    impl::camellia_f_table_lookup_32<impl::v64, std::uint64_t, impl::lookup_sbox32>,
                    impl::camellia_fl<impl::v64, std::uint64_t, impl::rotl_be1>,
                    impl::camellia_fl_inv<impl::v64, std::uint64_t, impl::rotl_be1>,
                    impl::camellia_postwhite
                >(b, kv);
                impl::store_v128(dst + i, b);
            }
        }
    }
}
