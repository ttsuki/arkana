/// @file
/// @brief	arkana::sha1
///			- An implementation of SHA-1
/// @author Copyright(c) 2022 ttsuki
/// 
/// This software is released under the MIT License.
/// https://opensource.org/licenses/MIT

#pragma once

#include "../bits/xmm.h"

#include "sha1.h"

namespace arkana::sha1
{
    namespace avx2
    {
        namespace impl
        {
            using functions::vector_t;
            using functions::chunk_t;
            using functions::compress_sha1;

            using xmm::vu32x4;

            ARKXMM_API rotate_sha1_x4(const vu32x4 x0, const vu32x4 x1, const vu32x4 x2, const vu32x4 x3) noexcept -> vu32x4
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

            ARKXMM_API rotate_sha1_x4(const vu32x4 x0, const vu32x4 x1, const vu32x4, const vu32x4, const vu32x4 x4, const vu32x4, const vu32x4 x6, const vu32x4 x7) noexcept -> vu32x4
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

            static void process_chunk_sha1(vector_t<uint32_t>& vec, chunk_t<uint32_t> input) noexcept
            {
                using bits::byteswap;

                vector_t<uint32_t> x = vec;

                vu32x4 x0 = byteswap(xmm::load_u(reinterpret_cast<const vu32x4*>(input.data() + 0)));
                vu32x4 x1 = byteswap(xmm::load_u(reinterpret_cast<const vu32x4*>(input.data() + 4)));
                vu32x4 x2 = byteswap(xmm::load_u(reinterpret_cast<const vu32x4*>(input.data() + 8)));
                vu32x4 x3 = byteswap(xmm::load_u(reinterpret_cast<const vu32x4*>(input.data() + 12)));
                vu32x4 x4 = rotate_sha1_x4(x0, x1, x2, x3);
                {
                    auto ck0 = to_array(x0 + xmm::u32x4(0x5A827999));
                    auto ck1 = to_array(x1 + xmm::u32x4(0x5A827999));
                    auto ck2 = to_array(x2 + xmm::u32x4(0x5A827999));
                    auto ck3 = to_array(x3 + xmm::u32x4(0x5A827999));
                    auto ck4 = to_array(x4 + xmm::u32x4(0x5A827999));
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
                    auto ck5 = to_array(x5 + xmm::u32x4(0x6ED9EBA1));
                    auto ck6 = to_array(x6 + xmm::u32x4(0x6ED9EBA1));
                    auto ck7 = to_array(x7 + xmm::u32x4(0x6ED9EBA1));
                    auto ck8 = to_array(x8 + xmm::u32x4(0x6ED9EBA1));
                    auto ck9 = to_array(x9 + xmm::u32x4(0x6ED9EBA1));
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
                    auto ck10 = to_array(x10 + xmm::u32x4(0x8F1BBCDC));
                    auto ck11 = to_array(x11 + xmm::u32x4(0x8F1BBCDC));
                    auto ck12 = to_array(x12 + xmm::u32x4(0x8F1BBCDC));
                    auto ck13 = to_array( x13 + xmm::u32x4(0x8F1BBCDC));
                    auto ck14 = to_array( x14 + xmm::u32x4(0x8F1BBCDC));
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
                    auto ck15 = to_array(x15 + xmm::u32x4(0xCA62C1D6));
                    auto ck16 = to_array(x16 + xmm::u32x4(0xCA62C1D6));
                    auto ck17 = to_array(x17 + xmm::u32x4(0xCA62C1D6));
                    auto ck18 = to_array(x18 + xmm::u32x4(0xCA62C1D6));
                    auto ck19 = to_array(x19 + xmm::u32x4(0xCA62C1D6));
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
                    auto vv = xmm::load_u(reinterpret_cast<xmm::vu32x8*>(vec.data()));
                    auto xx = xmm::load_u(reinterpret_cast<xmm::vu32x8*>(x.data()));
                    xmm::store_u(reinterpret_cast<xmm::vu32x8*>(vec.data()), vv + xx);
                }
            }
        }

        using ref::sha1_state_t;
        using ref::create_sha1_state;
        static inline void process_bytes(sha1_state_t& stt, const void* data, size_t len) noexcept { return functions::process_bytes<sha1_state_t, impl::process_chunk_sha1>(stt, data, len); }
        static inline sha1_digest_t finalize_and_get_digest(sha1_state_t& stt) noexcept { return functions::finalize_and_get_digest<sha1_state_t, impl::process_chunk_sha1>(stt); }
    }
}
