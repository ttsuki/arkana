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
#include "../ark/xmm.h"

namespace arkana::camellia
{
    namespace avx2
    {
        namespace impl
        {
            using namespace arkana::xmm;

            ARKXMM_API lookup_sbox32(const functions::sbox::sbox_t<uint32_t>& sbox, vu32x8 index, int shift) -> vu32x8
            {
                return xmm::gather<vu32x8>(sbox.data(), index >> shift * 8 & broadcast<vu32x8>(0x000000FF));
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

            ARKXMM_API transpose_32x4x4(vu32x8& x0, vu32x8& x1, vu32x8& x2, vu32x8& x3)
            {
                auto i0 = reinterpret<vu32x8>(x0);                // i0 = | 00010203|04050607|08090A0B|0C0D0E0F |
                auto i1 = reinterpret<vu32x8>(x1);                // i1 = | 10111213|14151617|18191A1B|1C1D1E1F |
                auto i2 = reinterpret<vu32x8>(x2);                // i2 = | 20212223|24252627|28292A2B|2C2D2E2F |
                auto i3 = reinterpret<vu32x8>(x3);                // i3 = | 30313233|34353637|38393A3B|3C3D3E3F |
                auto t0 = reinterpret<vu64x4>(unpack_lo(i0, i1)); // t0 = | 00010203 10111213|04050607 14151617 |
                auto t1 = reinterpret<vu64x4>(unpack_hi(i0, i1)); // t1 = | 08090A0B 18191A1B|0C0D0E0F 1C1D1E1F |
                auto t2 = reinterpret<vu64x4>(unpack_lo(i2, i3)); // t2 = | 20210223 30313233|24252627 34353637 |
                auto t3 = reinterpret<vu64x4>(unpack_hi(i2, i3)); // t3 = | 28290A2B 38393A3B|2C2D2E2F 3C3D3E3F |
                x0 = reinterpret<vu32x8>(unpack_lo(t0, t2));      // x0 = | 00010203 10111213 20212223 30313233 |
                x1 = reinterpret<vu32x8>(unpack_hi(t0, t2));      // x1 = | 04050607 14151617 24252627 34353637 |
                x2 = reinterpret<vu32x8>(unpack_lo(t1, t3));      // x2 = | 08090A0B 18191A1B 28292A2B 38393A3B |
                x3 = reinterpret<vu32x8>(unpack_hi(t1, t3));      // x3 = | 0C0D0E0F 1C1D1E1F 2C2D2E2F 3C3D3E3F |
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

            using key64 = uint64_t;

            ARKXMM_API operator ^=(v64& lhs, v64 rhs) noexcept -> v64&
            {
                lhs.l ^= rhs.l;
                lhs.r ^= rhs.r;
                return lhs;
            }

            ARKXMM_API camellia_prewhite(v128& block, const key64& kl, const key64& kr) -> v128&
            {
                const vu32x8 kx = reinterpret<vu32x8>(u64x4(kl, kr));
                block.l.l ^= kx;
                block.l.r ^= kx;
                block.r.l ^= kx;
                block.r.r ^= kx;
                transpose_32x4x4(block.l.l, block.l.r, block.r.l, block.r.r);
                return block;
            }

            ARKXMM_API camellia_postwhite(v128& block, const uint64_t& kl, const key64& kr) -> v128&
            {
                const vu32x8 kx = reinterpret<vu32x8>(u64x4(kl, kr));
                transpose_32x4x4(block.r.l, block.r.r, block.l.l, block.l.r);
                block.l.l ^= kx;
                block.l.r ^= kx;
                block.r.l ^= kx;
                block.r.r ^= kx;
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

            ARKXMM_API swap_xor128(v128& v, const v128& k) -> v128&
            {
                v.r.l ^= k.l.l;
                v.r.r ^= k.l.r;
                v.l.l ^= k.r.l;
                v.l.r ^= k.r.r;
                return v;
            }

            ARKXMM_API swap_store_v128(v128* dst, const v128& reg)
            {
                xmm::store_u(&dst->l.l, reg.r.l);
                xmm::store_u(&dst->l.r, reg.r.r);
                xmm::store_u(&dst->r.l, reg.l.l);
                xmm::store_u(&dst->r.r, reg.l.r);
            }
        }

        namespace impl
        {
            using ref::impl::key_vector_small_t;
            using ref::impl::key_vector_large_t;
            using ref::impl::generate_key_vector;

            template <class key_vector_t>
            static inline auto process_blocks_ecb(void* dst, const void* src, size_t length, const key_vector_t& kv)
            -> std::enable_if_t<is_any_of<key_vector_t, key_vector_small_t, key_vector_large_t>::value, void>
            {
                // rfc3713 ecb-mode
                using namespace functions;
                ecb_mode::process_blocks_ecb<
                    v128,
                    load_v128,
                    camellia_prewhite,
                    camellia_f_table_lookup_32<v64&, lookup_sbox32>,
                    camellia_fl<v64&, rotl_be1>,
                    camellia_fl_inv<v64&, rotl_be1>,
                    camellia_postwhite,
                    swap_store_v128>(dst, src, length, kv);
            }

            using ref::impl::ctr_iv_t;
            using ref::impl::ctr_nonce_t;

            template <class key_vector_t>
            static inline auto process_bytes_ctr(void* dst, const void* src, size_t position, size_t length, const key_vector_t& kv, const ctr_iv_t& ctr_iv, const ctr_nonce_t& ctr_nonce)
            -> std::enable_if_t<is_any_of<key_vector_t, key_vector_small_t, key_vector_large_t>::value, void>
            {
                // rfc5528 ctr-mode
                struct ctr_t
                {
                    uint32_t n, ivl, ivr, ctr;
                };

                ctr_t ctr0 = arkana::load_u<ctr_t>(&kv);
                ctr0.n ^= arkana::load_u<uint32_t>(ctr_nonce.data() + 0);
                ctr0.ivl ^= arkana::load_u<uint32_t>(ctr_iv.data() + 0);
                ctr0.ivr ^= arkana::load_u<uint32_t>(ctr_iv.data() + 4);

                using namespace functions;
                ctr_mode::process_bytes_ctr<
                    v128,
                    camellia_thruwhite<v128&, key64>,
                    camellia_f_table_lookup_32<v64&, lookup_sbox32>,
                    camellia_fl<v64&, rotl_be1>,
                    camellia_fl_inv<v64&, rotl_be1>,
                    camellia_postwhite,
                    load_v128,
                    swap_xor128,
                    store_v128>(dst, src, position, length, kv, [ctr0](size_t index) -> v128
                {
                    // rfc5528 ctr generator
                    v32 ctr = byteswap(u32x8(static_cast<uint32_t>(index * 8)) + u32x8(1, 3, 5, 7, 2, 4, 6, 8));
                    v128 v;
                    v.l.l = u32x8(ctr0.n);
                    v.l.r = u32x8(ctr0.ivl);
                    v.r.l = u32x8(ctr0.ivr);
                    v.r.r = u32x8(ctr0.ctr) ^ ctr;
                    return v;
                });
            }


            template <class key_vector_t, class ctr_generator_t>
            static inline auto process_bytes_ctr(void* dst, const void* src, size_t position, size_t length, const key_vector_t& kv, ctr_generator_t&& ctr)
            -> std::enable_if_t<is_any_of<key_vector_t, key_vector_small_t, key_vector_large_t>::value, void>
            {
                // custom ctr-mode
                using ctr_t = std::invoke_result_t<ctr_generator_t, size_t>;
                static_assert(sizeof(ctr_t) == 16);
                static_assert(std::is_trivially_copyable_v<ctr_t>);

                using namespace functions;
                ctr_mode::process_bytes_ctr<
                    v128,
                    camellia_prewhite,
                    camellia_f_table_lookup_32<v64&, lookup_sbox32>,
                    camellia_fl<v64&, rotl_be1>,
                    camellia_fl_inv<v64&, rotl_be1>,
                    camellia_postwhite,
                    load_v128,
                    swap_xor128,
                    store_v128>(dst, src, position, length, kv, [ctr = std::forward<decltype(ctr)>(ctr)](size_t index) -> v128
                {
                    array<ctr_t, 8> v = {
                        ctr(index * 8 + 0),
                        ctr(index * 8 + 1),
                        ctr(index * 8 + 2),
                        ctr(index * 8 + 3),
                        ctr(index * 8 + 4),
                        ctr(index * 8 + 5),
                        ctr(index * 8 + 6),
                        ctr(index * 8 + 7),
                    };

                    return load_v128(reinterpret_cast<v128*>(v.data()));
                });
            }
        }

        using impl::key_vector_small_t;
        using impl::key_vector_large_t;

        static inline key_vector_small_t generate_key_vector_encrypt(const key_128bit_t& key) { return impl::generate_key_vector(key, true_t{}); }
        static inline key_vector_large_t generate_key_vector_encrypt(const key_192bit_t& key) { return impl::generate_key_vector(key, true_t{}); }
        static inline key_vector_large_t generate_key_vector_encrypt(const key_256bit_t& key) { return impl::generate_key_vector(key, true_t{}); }
        static inline key_vector_small_t generate_key_vector_decrypt(const key_128bit_t& key) { return impl::generate_key_vector(key, false_t{}); }
        static inline key_vector_large_t generate_key_vector_decrypt(const key_192bit_t& key) { return impl::generate_key_vector(key, false_t{}); }
        static inline key_vector_large_t generate_key_vector_decrypt(const key_256bit_t& key) { return impl::generate_key_vector(key, false_t{}); }
        static inline void process_blocks_ecb(void* dst, const void* src, size_t length, const key_vector_small_t& kv) { return impl::process_blocks_ecb(dst, src, length, kv); }
        static inline void process_blocks_ecb(void* dst, const void* src, size_t length, const key_vector_large_t& kv) { return impl::process_blocks_ecb(dst, src, length, kv); }

        using impl::ctr_iv_t;
        using impl::ctr_nonce_t;
        static inline void process_bytes_ctr(void* dst, const void* src, size_t position, size_t length, const key_vector_small_t& kv, const ctr_iv_t& ctr_iv, const ctr_nonce_t& ctr_nonce) { return impl::process_bytes_ctr(dst, src, position, length, kv, ctr_iv, ctr_nonce); }
        static inline void process_bytes_ctr(void* dst, const void* src, size_t position, size_t length, const key_vector_large_t& kv, const ctr_iv_t& ctr_iv, const ctr_nonce_t& ctr_nonce) { return impl::process_bytes_ctr(dst, src, position, length, kv, ctr_iv, ctr_nonce); }
    }
}
