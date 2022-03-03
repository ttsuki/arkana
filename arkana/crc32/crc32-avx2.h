/// @file
/// @brief	arkana::crc32
///			- An implementation of CRC32
/// @author Copyright(c) 2020 ttsuki
/// 
/// This software is released under the MIT License.
/// https://opensource.org/licenses/MIT

#pragma once

#include "crc32-ia32.h"

#include "../bits/xmm.h"

namespace arkana::crc32
{
    namespace avx2
    {
        template <uint32_t polynomial>
        static inline crc32_value_t calculate_crc32(const void* data, size_t length, crc32_value_t current = 0)
        {
            using namespace arkana::xmm;
            using vu32x8x4 = PAIR<PAIR<vu32x8>>;

            const byte_t* p = static_cast<const byte_t*>(data);

            // process unaligned bytes
            constexpr size_t align_to = alignof(vu32x8);
            if (size_t l = std::min<size_t>(length, static_cast<size_t>(-reinterpret_cast<intptr_t>(p) & (align_to - 1))))
            {
                current = crc32::calculate_crc32<polynomial>(p, l, current);
                length -= l;
                p += l;
            }

            // process aligned 128-byte blocks
            if (std::size_t blockCount = length / sizeof(vu32x8x4))
            {
                vu32x8 v = from_values<vu32x8>(~current, 0, 0, 0, 0, 0, 0, 0);

                while (blockCount > 1)
                {
                    vu32x8x4 s = load_s(reinterpret_cast<const vu32x8x4*>(p));
                    p += sizeof(vu32x8x4);
                    prefetch_nta(p + sizeof(vu32x8x4)); // prefetch next next block

                    s = transpose_32x4x4(s);

                    s.l.l ^= v;

                    v =
                        gather<vu32x8>(tables::crc32_table_n<polynomial, 112 + 0>.data(), extract_byte<3>(s.r.r)) ^
                        gather<vu32x8>(tables::crc32_table_n<polynomial, 112 + 1>.data(), extract_byte<2>(s.r.r)) ^
                        gather<vu32x8>(tables::crc32_table_n<polynomial, 112 + 2>.data(), extract_byte<1>(s.r.r)) ^
                        gather<vu32x8>(tables::crc32_table_n<polynomial, 112 + 3>.data(), extract_byte<0>(s.r.r)) ^
                        gather<vu32x8>(tables::crc32_table_n<polynomial, 112 + 4>.data(), extract_byte<3>(s.r.l)) ^
                        gather<vu32x8>(tables::crc32_table_n<polynomial, 112 + 5>.data(), extract_byte<2>(s.r.l)) ^
                        gather<vu32x8>(tables::crc32_table_n<polynomial, 112 + 6>.data(), extract_byte<1>(s.r.l)) ^
                        gather<vu32x8>(tables::crc32_table_n<polynomial, 112 + 7>.data(), extract_byte<0>(s.r.l)) ^
                        gather<vu32x8>(tables::crc32_table_n<polynomial, 112 + 8>.data(), extract_byte<3>(s.l.r)) ^
                        gather<vu32x8>(tables::crc32_table_n<polynomial, 112 + 9>.data(), extract_byte<2>(s.l.r)) ^
                        gather<vu32x8>(tables::crc32_table_n<polynomial, 112 + 10>.data(), extract_byte<1>(s.l.r)) ^
                        gather<vu32x8>(tables::crc32_table_n<polynomial, 112 + 11>.data(), extract_byte<0>(s.l.r)) ^
                        gather<vu32x8>(tables::crc32_table_n<polynomial, 112 + 12>.data(), extract_byte<3>(s.l.l)) ^
                        gather<vu32x8>(tables::crc32_table_n<polynomial, 112 + 13>.data(), extract_byte<2>(s.l.l)) ^
                        gather<vu32x8>(tables::crc32_table_n<polynomial, 112 + 14>.data(), extract_byte<1>(s.l.l)) ^
                        gather<vu32x8>(tables::crc32_table_n<polynomial, 112 + 15>.data(), extract_byte<0>(s.l.l));

                    --blockCount;
                }

                // process last 128-byte block
                {
                    auto block = to_array(load_s(reinterpret_cast<const vu32x8x4*>(p)));
                    p += sizeof(vu32x8x4);

                    auto varr = to_array(permute32<0, 4, 1, 5, 2, 6, 3, 7>(v));
                    block[0] ^= varr[0];
                    block[4] ^= varr[1];
                    block[8] ^= varr[2];
                    block[12] ^= varr[3];
                    block[16] ^= varr[4];
                    block[20] ^= varr[5];
                    block[24] ^= varr[6];
                    block[28] ^= varr[7];

                    current = ia32::calculate_crc32<polynomial>(block.data(), sizeof(block), ~0u);
                }

                length %= sizeof(vu32x8x4);
            }

            // process remain bytes
            return crc32::calculate_crc32<polynomial>(p, length, current);
        }
    }
}
