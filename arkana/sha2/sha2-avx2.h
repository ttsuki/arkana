/// @file
/// @brief	arkana::sha2
///			- An implementation of SHA-1, SHA-2
/// @author Copyright(c) 2022 ttsuki
/// 
/// This software is released under the MIT License.
/// https://opensource.org/licenses/MIT
///
/// This SHA-2 implementation based on
///   "Fast SHA-256 Implementations on Intel® Architecture Processors"
///   -- J. Guilford, K. Yap, V. Gopal, 2012,
/// https://www.intel.com/content/dam/www/public/us/en/documents/white-papers/sha-256-implementations-paper.pdf

#pragma once

#include "../bits/xmm.h"

#include "sha2.h"

namespace arkana::sha2
{
    namespace avx2
    {
        namespace impl
        {
            using functions::vector_t;
            using functions::chunk_t;
            using functions::compress_sha1;
            using functions::compress_sha2;

            ARKXMM_API rotate_sha1_x4(const xmm::vu32x4 x0, const xmm::vu32x4 x1, const xmm::vu32x4 x2, const xmm::vu32x4 x3) noexcept -> xmm::vu32x4
            {
                //  in: x0 = w[-16] w[-15] w[-14] w[-13]
                //  in: x1 = w[-12] w[-11] w[-10] w[ -9]
                //  in: x2 = w[ -8] w[ -7] w[ -6] w[ -5]
                //  in: x3 = w[ -4] w[ -3] w[ -2] w[ -1]
                // out: x4 = w[  0] w[  1] w[  2] w[  3]

                using namespace xmm;

                vu32x4 w16 = x0;
                vu32x4 w14 = byte_align_r_128<8>(x0, x1);
                vu32x4 w8 = x2;
                vu32x4 w3 = byte_shift_r_128<4>(x3);
                vu32x4 r = rotl(w16 ^ w14 ^ w8 ^ w3, 1);
                return r ^ byte_shift_l_128<12>(rotl(r, 1));
            }

            ARKXMM_API rotate_sha1_x4(const xmm::vu32x4 x0, const xmm::vu32x4 x1, const xmm::vu32x4, const xmm::vu32x4, const xmm::vu32x4 x4, const xmm::vu32x4, const xmm::vu32x4 x6, const xmm::vu32x4 x7) noexcept -> xmm::vu32x4
            {
                //  in: x0 = w[-32] w[-31] w[-30] w[-29]
                //  in: x1 = w[-28] w[-27] w[-26] w[-25]
                //  in: x2 = w[-24] w[-23] w[-22] w[-21]
                //  in: x3 = w[-20] w[-19] w[-18] w[-17]
                //  in: x4 = w[-16] w[-15] w[-14] w[-13]
                //  in: x5 = w[-12] w[-11] w[-10] w[ -9]
                //  in: x6 = w[ -8] w[ -7] w[ -6] w[ -5]
                //  in: x7 = w[ -4] w[ -3] w[ -2] w[ -1]
                // out: x4 = w[  0] w[  1] w[  2] w[  3]

                using namespace xmm;

                vu32x4 w32 = x0;
                vu32x4 w28 = x1;
                vu32x4 w16 = x4;
                vu32x4 w6 = byte_align_r_128<8>(x6, x7);
                return rotl(w32 ^ w28 ^ w16 ^ w6, 2);
            }

            static void process_chunk_sha1(vector_t<uint32_t>& vec, const chunk_t<uint32_t>& input) noexcept
            {
                using namespace xmm;
                using bits::byteswap;

                vector_t<uint32_t> x = vec;
                constexpr uint32_t k[4] = {0x5A827999, 0x6ED9EBA1, 0x8F1BBCDC, 0xCA62C1D6};

                vu32x4 x0 = byteswap(xmm::load_u(reinterpret_cast<const vu32x4*>(input.data() + 0)));
                vu32x4 x1 = byteswap(xmm::load_u(reinterpret_cast<const vu32x4*>(input.data() + 4)));
                vu32x4 x2 = byteswap(xmm::load_u(reinterpret_cast<const vu32x4*>(input.data() + 8)));
                vu32x4 x3 = byteswap(xmm::load_u(reinterpret_cast<const vu32x4*>(input.data() + 12)));
                vu32x4 x4 = rotate_sha1_x4(x0, x1, x2, x3);
                {
                    auto k0 = u32x4(k[0]);
                    auto ck0 = to_array(x0 + k0);
                    auto ck1 = to_array(x1 + k0);
                    auto ck2 = to_array(x2 + k0);
                    auto ck3 = to_array(x3 + k0);
                    auto ck4 = to_array(x4 + k0);
                    compress_sha1<00 + 0>(x, ck0[0]);
                    compress_sha1<00 + 1>(x, ck0[1]);
                    compress_sha1<00 + 2>(x, ck0[2]);
                    compress_sha1<00 + 3>(x, ck0[3]);
                    compress_sha1<00 + 4>(x, ck1[0]);
                    compress_sha1<00 + 0>(x, ck1[1]);
                    compress_sha1<00 + 1>(x, ck1[2]);
                    compress_sha1<00 + 2>(x, ck1[3]);
                    compress_sha1<00 + 3>(x, ck2[0]);
                    compress_sha1<00 + 4>(x, ck2[1]);
                    compress_sha1<00 + 0>(x, ck2[2]);
                    compress_sha1<00 + 1>(x, ck2[3]);
                    compress_sha1<00 + 2>(x, ck3[0]);
                    compress_sha1<00 + 3>(x, ck3[1]);
                    compress_sha1<00 + 4>(x, ck3[2]);
                    compress_sha1<00 + 0>(x, ck3[3]);
                    compress_sha1<00 + 1>(x, ck4[0]);
                    compress_sha1<00 + 2>(x, ck4[1]);
                    compress_sha1<00 + 3>(x, ck4[2]);
                    compress_sha1<00 + 4>(x, ck4[3]);
                }

                vu32x4 x5 = rotate_sha1_x4(x1, x2, x3, x4);
                vu32x4 x6 = rotate_sha1_x4(x2, x3, x4, x5);
                vu32x4 x7 = rotate_sha1_x4(x3, x4, x5, x6);
                vu32x4 x8 = rotate_sha1_x4(x0, x1, x2, x3, x4, x5, x6, x7);
                vu32x4 x9 = rotate_sha1_x4(x1, x2, x3, x4, x5, x6, x7, x8);
                {
                    auto k1 = u32x4(k[1]);
                    auto ck5 = to_array(x5 + k1);
                    auto ck6 = to_array(x6 + k1);
                    auto ck7 = to_array(x7 + k1);
                    auto ck8 = to_array(x8 + k1);
                    auto ck9 = to_array(x9 + k1);
                    compress_sha1<20 + 0>(x, ck5[0]);
                    compress_sha1<20 + 1>(x, ck5[1]);
                    compress_sha1<20 + 2>(x, ck5[2]);
                    compress_sha1<20 + 3>(x, ck5[3]);
                    compress_sha1<20 + 4>(x, ck6[0]);
                    compress_sha1<20 + 0>(x, ck6[1]);
                    compress_sha1<20 + 1>(x, ck6[2]);
                    compress_sha1<20 + 2>(x, ck6[3]);
                    compress_sha1<20 + 3>(x, ck7[0]);
                    compress_sha1<20 + 4>(x, ck7[1]);
                    compress_sha1<20 + 0>(x, ck7[2]);
                    compress_sha1<20 + 1>(x, ck7[3]);
                    compress_sha1<20 + 2>(x, ck8[0]);
                    compress_sha1<20 + 3>(x, ck8[1]);
                    compress_sha1<20 + 4>(x, ck8[2]);
                    compress_sha1<20 + 0>(x, ck8[3]);
                    compress_sha1<20 + 1>(x, ck9[0]);
                    compress_sha1<20 + 2>(x, ck9[1]);
                    compress_sha1<20 + 3>(x, ck9[2]);
                    compress_sha1<20 + 4>(x, ck9[3]);
                }

                vu32x4 x10 = rotate_sha1_x4(x2, x3, x4, x5, x6, x7, x8, x9);
                vu32x4 x11 = rotate_sha1_x4(x3, x4, x5, x6, x7, x8, x9, x10);
                vu32x4 x12 = rotate_sha1_x4(x4, x5, x6, x7, x8, x9, x10, x11);
                vu32x4 x13 = rotate_sha1_x4(x5, x6, x7, x8, x9, x10, x11, x12);
                vu32x4 x14 = rotate_sha1_x4(x6, x7, x8, x9, x10, x11, x12, x13);
                {
                    auto k2 = u32x4(k[2]);
                    auto ck10 = to_array(x10 + k2);
                    auto ck11 = to_array(x11 + k2);
                    auto ck12 = to_array(x12 + k2);
                    auto ck13 = to_array(x13 + k2);
                    auto ck14 = to_array(x14 + k2);
                    compress_sha1<40 + 0>(x, ck10[0]);
                    compress_sha1<40 + 1>(x, ck10[1]);
                    compress_sha1<40 + 2>(x, ck10[2]);
                    compress_sha1<40 + 3>(x, ck10[3]);
                    compress_sha1<40 + 4>(x, ck11[0]);
                    compress_sha1<40 + 0>(x, ck11[1]);
                    compress_sha1<40 + 1>(x, ck11[2]);
                    compress_sha1<40 + 2>(x, ck11[3]);
                    compress_sha1<40 + 3>(x, ck12[0]);
                    compress_sha1<40 + 4>(x, ck12[1]);
                    compress_sha1<40 + 0>(x, ck12[2]);
                    compress_sha1<40 + 1>(x, ck12[3]);
                    compress_sha1<40 + 2>(x, ck13[0]);
                    compress_sha1<40 + 3>(x, ck13[1]);
                    compress_sha1<40 + 4>(x, ck13[2]);
                    compress_sha1<40 + 0>(x, ck13[3]);
                    compress_sha1<40 + 1>(x, ck14[0]);
                    compress_sha1<40 + 2>(x, ck14[1]);
                    compress_sha1<40 + 3>(x, ck14[2]);
                    compress_sha1<40 + 4>(x, ck14[3]);
                }

                vu32x4 x15 = rotate_sha1_x4(x7, x8, x9, x10, x11, x12, x13, x14);
                vu32x4 x16 = rotate_sha1_x4(x8, x9, x10, x11, x12, x13, x14, x15);
                vu32x4 x17 = rotate_sha1_x4(x9, x10, x11, x12, x13, x14, x15, x16);
                vu32x4 x18 = rotate_sha1_x4(x10, x11, x12, x13, x14, x15, x16, x17);
                vu32x4 x19 = rotate_sha1_x4(x11, x12, x13, x14, x15, x16, x17, x18);
                {
                    auto k3 = u32x4(k[3]);
                    auto ck15 = to_array(x15 + k3);
                    auto ck16 = to_array(x16 + k3);
                    auto ck17 = to_array(x17 + k3);
                    auto ck18 = to_array(x18 + k3);
                    auto ck19 = to_array(x19 + k3);
                    compress_sha1<60 + 0>(x, ck15[0]);
                    compress_sha1<60 + 1>(x, ck15[1]);
                    compress_sha1<60 + 2>(x, ck15[2]);
                    compress_sha1<60 + 3>(x, ck15[3]);
                    compress_sha1<60 + 4>(x, ck16[0]);
                    compress_sha1<60 + 0>(x, ck16[1]);
                    compress_sha1<60 + 1>(x, ck16[2]);
                    compress_sha1<60 + 2>(x, ck16[3]);
                    compress_sha1<60 + 3>(x, ck17[0]);
                    compress_sha1<60 + 4>(x, ck17[1]);
                    compress_sha1<60 + 0>(x, ck17[2]);
                    compress_sha1<60 + 1>(x, ck17[3]);
                    compress_sha1<60 + 2>(x, ck18[0]);
                    compress_sha1<60 + 3>(x, ck18[1]);
                    compress_sha1<60 + 4>(x, ck18[2]);
                    compress_sha1<60 + 0>(x, ck18[3]);
                    compress_sha1<60 + 1>(x, ck19[0]);
                    compress_sha1<60 + 2>(x, ck19[1]);
                    compress_sha1<60 + 3>(x, ck19[2]);
                    compress_sha1<60 + 4>(x, ck19[3]);
                }

                {
                    auto vv = xmm::load_u(reinterpret_cast<vu32x8*>(vec.data()));
                    auto xx = xmm::load_u(reinterpret_cast<vu32x8*>(x.data()));
                    xmm::store_u(reinterpret_cast<vu32x8*>(vec.data()), vv + xx);
                }
            }

            template <int s00, int s01, int s02, int s10, int s11, int s12>
            ARKXMM_API rotate_sha2_x4(xmm::vu32x4& x0, xmm::vu32x4& x1, xmm::vu32x4& x2, xmm::vu32x4& x3)
            {
                //  in: x0 = w[-16] w[-15] w[-14] w[-13]
                //  in: x1 = w[-12] w[-11] w[-10] w[ -9]
                //  in: x2 = w[ -8] w[ -7] w[ -6] w[ -5]
                //  in: x3 = w[ -4] w[ -3] w[ -2] w[ -1]
                // out: x0 = w[  0] w[  1] w[  2] w[  3]

                using namespace xmm;

                vu32x4 w15 = byte_align_r_128<4>(x0, x1); // w[-15,-14,-13,-12]
                vu32x4 w7 = byte_align_r_128<4>(x2, x3);  // w[-7,-6.-5,-4]
                vu32x4 s0 = rotr(w15, s00) ^ rotr(w15, s01) ^ (w15 >> s02);
                vu32x4 t = x0 + w7 + s0;

                // rotr32(u32 v,i) = u64(v<<32|v) >> i
                vu32x4 w2_aabb = shuffle<0b11'11'10'10>(x3); // w[-2,-2,-1,-1]
                vu32x4 s1_abab = shuffle<0b10'00'10'00>(
                    reinterpret<vu32x4>(reinterpret<vu64x2>(w2_aabb) >> s10) ^
                    reinterpret<vu32x4>(reinterpret<vu64x2>(w2_aabb) >> s11) ^
                    w2_aabb >> s12);

                // rotr32(u32 v,i) = u64(v<<32|v) >> i
                vu32x4 w2_ccdd = shuffle<0b01'01'00'00>(t + s1_abab); // w[0,0,1,1]
                vu32x4 s1_cbcd = shuffle<0b10'00'10'00>(
                    reinterpret<vu32x4>(reinterpret<vu64x2>(w2_ccdd) >> s10) ^
                    reinterpret<vu32x4>(reinterpret<vu64x2>(w2_ccdd) >> s11) ^
                    w2_ccdd >> s12);

                vu32x4 s1 = byte_align_r_128<8>(s1_abab, s1_cbcd);

                x0 = t + s1; // w[0,1,2,3]
            }


            template <int s00, int s01, int s02, int s10, int s11, int s12>
            ARKXMM_API rotate_sha2_x4(xmm::vu64x4& x0, xmm::vu64x4& x1, xmm::vu64x4& x2, xmm::vu64x4& x3)
            {
                //  in: x0 = w[-16] w[-15] w[-14] w[-13]
                //  in: x1 = w[-12] w[-11] w[-10] w[ -9]
                //  in: x2 = w[ -8] w[ -7] w[ -6] w[ -5]
                //  in: x3 = w[ -4] w[ -3] w[ -2] w[ -1]
                // out: x0 = w[  0] w[  1] w[  2] w[  3]

                using namespace xmm;
                vu64x4 w15 = byte_align_r_128<8>(x0, permute128<1, 2>(x0, x1)); // w[-15,-14,-13,-12]
                vu64x4 w7 = byte_align_r_128<8>(x2, permute128<1, 2>(x2, x3));  // w[-7,-6.-5,-4]
                vu64x4 s0 = rotr(w15, s00) ^ rotr(w15, s01) ^ (w15 >> s02);
                vu64x4 t = x0 + w7 + s0;

                vu64x4 w2_abxx = permute64<2, 3, 2, 3>(x3); // w[-2,-1,_,_]
                vu64x4 s1_abxx = rotr(w2_abxx, s10) ^ rotr(w2_abxx, s11) ^ (w2_abxx >> s12);
                vu64x4 w2_cdxx = t + s1_abxx;                   // w[0,1,_,_]
                vu64x4 w2 = permute128<0, 2>(w2_abxx, w2_cdxx); // w[-2,-1,0,1]
                vu64x4 s1 = rotr(w2, s10) ^ rotr(w2, s11) ^ (w2 >> s12);

                x0 = t + s1; // w[0,1,2,3]
            }

            template <
                class T, class V,
                class round_constants,
                int s00, int s01, int s02, int s10, int s11, int s12,
                int S00, int S01, int S02, int S10, int S11, int S12>
            static void process_chunk_sha2(vector_t<T>& vec, const chunk_t<T>& input) noexcept
            {
                vector_t<T> x = vec;
                chunk_t<T> ck = input;

                V x0 = byteswap(xmm::load_u(reinterpret_cast<const V*>(ck.data() + 0)));
                V x1 = byteswap(xmm::load_u(reinterpret_cast<const V*>(ck.data() + 4)));
                V x2 = byteswap(xmm::load_u(reinterpret_cast<const V*>(ck.data() + 8)));
                V x3 = byteswap(xmm::load_u(reinterpret_cast<const V*>(ck.data() + 12)));

                for (size_t i = 0; i < (round_constants::rounds / 16); i++)
                {
                    auto k = round_constants::constants + i * 0x10;
                    xmm::store_u(reinterpret_cast<V*>(ck.data() + 0x0), x0 + xmm::load_u(reinterpret_cast<const V*>(k + 0x0)));
                    xmm::store_u(reinterpret_cast<V*>(ck.data() + 0x4), x1 + xmm::load_u(reinterpret_cast<const V*>(k + 0x4)));
                    xmm::store_u(reinterpret_cast<V*>(ck.data() + 0x8), x2 + xmm::load_u(reinterpret_cast<const V*>(k + 0x8)));
                    xmm::store_u(reinterpret_cast<V*>(ck.data() + 0xC), x3 + xmm::load_u(reinterpret_cast<const V*>(k + 0xC)));
                    compress_sha2<S00, S01, S02, S10, S11, S12, 0>(vec, ck[0]);
                    compress_sha2<S00, S01, S02, S10, S11, S12, 1>(vec, ck[1]);
                    compress_sha2<S00, S01, S02, S10, S11, S12, 2>(vec, ck[2]);
                    compress_sha2<S00, S01, S02, S10, S11, S12, 3>(vec, ck[3]);
                    compress_sha2<S00, S01, S02, S10, S11, S12, 4>(vec, ck[4]);
                    compress_sha2<S00, S01, S02, S10, S11, S12, 5>(vec, ck[5]);
                    compress_sha2<S00, S01, S02, S10, S11, S12, 6>(vec, ck[6]);
                    compress_sha2<S00, S01, S02, S10, S11, S12, 7>(vec, ck[7]);
                    compress_sha2<S00, S01, S02, S10, S11, S12, 0>(vec, ck[8]);
                    compress_sha2<S00, S01, S02, S10, S11, S12, 1>(vec, ck[9]);
                    compress_sha2<S00, S01, S02, S10, S11, S12, 2>(vec, ck[10]);
                    compress_sha2<S00, S01, S02, S10, S11, S12, 3>(vec, ck[11]);
                    compress_sha2<S00, S01, S02, S10, S11, S12, 4>(vec, ck[12]);
                    compress_sha2<S00, S01, S02, S10, S11, S12, 5>(vec, ck[13]);
                    compress_sha2<S00, S01, S02, S10, S11, S12, 6>(vec, ck[14]);
                    compress_sha2<S00, S01, S02, S10, S11, S12, 7>(vec, ck[15]);
                    rotate_sha2_x4<s00, s01, s02, s10, s11, s12>(x0, x1, x2, x3);
                    rotate_sha2_x4<s00, s01, s02, s10, s11, s12>(x1, x2, x3, x0);
                    rotate_sha2_x4<s00, s01, s02, s10, s11, s12>(x2, x3, x0, x1);
                    rotate_sha2_x4<s00, s01, s02, s10, s11, s12>(x3, x0, x1, x2);
                }

                xmm::store_u(reinterpret_cast<V*>(vec.data() + 0), xmm::load_u(reinterpret_cast<V*>(vec.data() + 0)) + xmm::load_u(reinterpret_cast<const V*>(x.data() + 0)));
                xmm::store_u(reinterpret_cast<V*>(vec.data() + 4), xmm::load_u(reinterpret_cast<V*>(vec.data() + 4)) + xmm::load_u(reinterpret_cast<const V*>(x.data() + 4)));
            }

            static inline void process_chunk_sha256(vector_t<uint32_t>& v, const chunk_t<uint32_t>& input)
            {
                return process_chunk_sha2<
                    uint32_t, xmm::vu32x4,
                    functions::round_constants_sha256,
                    7, 18, 3, 17, 19, 10,
                    2, 13, 22, 6, 11, 25>(v, input);
            }

            static inline void process_chunk_sha512(vector_t<uint64_t>& v, const chunk_t<uint64_t>& input)
            {
                return process_chunk_sha2<
                    uint64_t, xmm::vu64x4,
                    functions::round_constants_sha512,
                    1, 8, 7, 19, 61, 6,
                    28, 34, 39, 14, 18, 41>(v, input);
            }
        }

        using ref::sha1_state_t;
        using ref::sha224_state_t;
        using ref::sha256_state_t;
        using ref::sha384_state_t;
        using ref::sha512_state_t;
        using ref::create_sha1_state;
        using ref::create_sha224_state;
        using ref::create_sha256_state;
        using ref::create_sha384_state;
        using ref::create_sha512_state;
        static inline void process_bytes(sha1_state_t& stt, const void* data, size_t len) noexcept { return functions::process_bytes<sha1_state_t, impl::process_chunk_sha1>(stt, data, len); }
        static inline void process_bytes(sha224_state_t& stt, const void* data, size_t len) noexcept { return functions::process_bytes<sha224_state_t, impl::process_chunk_sha256>(stt, data, len); }
        static inline void process_bytes(sha256_state_t& stt, const void* data, size_t len) noexcept { return functions::process_bytes<sha256_state_t, impl::process_chunk_sha256>(stt, data, len); }
        static inline void process_bytes(sha384_state_t& stt, const void* data, size_t len) noexcept { return functions::process_bytes<sha384_state_t, impl::process_chunk_sha512>(stt, data, len); }
        static inline void process_bytes(sha512_state_t& stt, const void* data, size_t len) noexcept { return functions::process_bytes<sha512_state_t, impl::process_chunk_sha512>(stt, data, len); }
        static inline sha1_digest_t finalize_and_get_digest(sha1_state_t& stt) noexcept { return functions::finalize_and_get_digest<sha1_state_t, impl::process_chunk_sha1>(stt); }
        static inline sha224_digest_t finalize_and_get_digest(sha224_state_t& stt) noexcept { return functions::finalize_and_get_digest<sha224_state_t, impl::process_chunk_sha256>(stt); }
        static inline sha256_digest_t finalize_and_get_digest(sha256_state_t& stt) noexcept { return functions::finalize_and_get_digest<sha256_state_t, impl::process_chunk_sha256>(stt); }
        static inline sha384_digest_t finalize_and_get_digest(sha384_state_t& stt) noexcept { return functions::finalize_and_get_digest<sha384_state_t, impl::process_chunk_sha512>(stt); }
        static inline sha512_digest_t finalize_and_get_digest(sha512_state_t& stt) noexcept { return functions::finalize_and_get_digest<sha512_state_t, impl::process_chunk_sha512>(stt); }
    }
}
