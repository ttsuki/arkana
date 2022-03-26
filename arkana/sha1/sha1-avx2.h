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
            using functions::compress;

            using xmm::vu32x4;

            ARKXMM_API rotate4x(vu32x4& x0, vu32x4& x1, vu32x4& x2, vu32x4& x3)
            {
                //  in: x0 = w[-16] w[-15] w[-14] w[-13]
                //  in: x1 = w[-12] w[-11] w[-10] w[ -9]
                //  in: x2 = w[ -8] w[ -7] w[ -6] w[ -5]
                //  in: x3 = w[ -4] w[ -3] w[ -2] w[ -1]
                // out: x0 = w[  0] w[  1] w[  2] w[  3]

                using namespace xmm;

                vu32x4 w16 = x0;
                vu32x4 w14 = byte_align_r_128<8>(x0, x1);
                vu32x4 w8 = x2;
                vu32x4 w3 = byte_shift_r_128<4>(x3);
                x0 = rotl(w16 ^ w14 ^ w8 ^ w3, 1);
                x0 ^= byte_shift_l_128<12>(rotl(x0, 1));
            }

            template <size_t i>
            ARKXMM_API round16x(vu32x4& x0, vu32x4& x1, vu32x4& x2, vu32x4& x3, vector_t& x, chunk_t& input) noexcept
            {
                xmm::store_u(reinterpret_cast<vu32x4*>(input.data() + 0), x0);
                xmm::store_u(reinterpret_cast<vu32x4*>(input.data() + 4), x1);
                xmm::store_u(reinterpret_cast<vu32x4*>(input.data() + 8), x2);
                xmm::store_u(reinterpret_cast<vu32x4*>(input.data() + 12), x3);

                compress<i * 16 + 0>(x, input[0]);
                compress<i * 16 + 1>(x, input[1]);
                compress<i * 16 + 2>(x, input[2]);
                compress<i * 16 + 3>(x, input[3]);
                compress<i * 16 + 4>(x, input[4]);
                compress<i * 16 + 5>(x, input[5]);
                compress<i * 16 + 6>(x, input[6]);
                compress<i * 16 + 7>(x, input[7]);
                compress<i * 16 + 8>(x, input[8]);
                compress<i * 16 + 9>(x, input[9]);
                compress<i * 16 + 10>(x, input[10]);
                compress<i * 16 + 11>(x, input[11]);
                compress<i * 16 + 12>(x, input[12]);
                compress<i * 16 + 13>(x, input[13]);
                compress<i * 16 + 14>(x, input[14]);
                compress<i * 16 + 15>(x, input[15]);

                rotate4x(x0, x1, x2, x3);
                rotate4x(x1, x2, x3, x0);
                rotate4x(x2, x3, x0, x1);
                rotate4x(x3, x0, x1, x2);
            }

            static inline void process_chunk(vector_t& vec, chunk_t input) noexcept
            {
                vector_t x = vec;

                vu32x4 x0 = byteswap(xmm::load_u(reinterpret_cast<const vu32x4*>(input.data() + 0)));
                vu32x4 x1 = byteswap(xmm::load_u(reinterpret_cast<const vu32x4*>(input.data() + 4)));
                vu32x4 x2 = byteswap(xmm::load_u(reinterpret_cast<const vu32x4*>(input.data() + 8)));
                vu32x4 x3 = byteswap(xmm::load_u(reinterpret_cast<const vu32x4*>(input.data() + 12)));

                round16x<0>(x0, x1, x2, x3, x, input);
                round16x<1>(x0, x1, x2, x3, x, input);
                round16x<2>(x0, x1, x2, x3, x, input);
                round16x<3>(x0, x1, x2, x3, x, input);
                round16x<4>(x0, x1, x2, x3, x, input);

                xmm::store_u(reinterpret_cast<vu32x4*>(vec.data() + 0), xmm::load_u(reinterpret_cast<vu32x4*>(vec.data() + 0)) + xmm::load_u(reinterpret_cast<const vu32x4*>(x.data() + 0)));
                xmm::store_u(reinterpret_cast<vu32x4*>(vec.data() + 4), xmm::load_u(reinterpret_cast<vu32x4*>(vec.data() + 4)) + xmm::load_u(reinterpret_cast<const vu32x4*>(x.data() + 4)));
            }
        }

        using ref::sha1_state_t;
        using ref::create_sha1_state;
        static inline void process_bytes(sha1_state_t& stt, const void* data, size_t len) noexcept { return functions::process_bytes<impl::process_chunk>(stt, data, len); }
        static inline sha1_digest_t finalize_and_get_digest(sha1_state_t& stt) noexcept { return functions::finalize_and_get_digest<impl::process_chunk>(stt); }
    }
}
