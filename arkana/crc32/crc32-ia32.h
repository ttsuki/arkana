/// @file
/// @brief	arkana::crc32
///			- An implementation of CRC32
/// @author Copyright(c) 2020 ttsuki
/// 
/// This software is released under the MIT License.
/// https://opensource.org/licenses/MIT

#pragma once

#include "crc32.h"

namespace arkana::crc32
{
    namespace ia32
    {
        template <uint32_t polynomial>
        static inline crc32_value_t calculate_crc32(const void* data, size_t length, crc32_value_t current = 0)
        {
            const byte_t* p = static_cast<const byte_t*>(data);
            
            // process unaligned bytes
            constexpr size_t align_to = 16;
            if (size_t l = std::min<size_t>(length, static_cast<size_t>(-reinterpret_cast<intptr_t>(p) & (align_to - 1))))
            {
                current = crc32::calculate_crc32<polynomial>(p, l, current);
                length -= l;
                p += l;
            }

            // process aligned 64-byte blocks
            uint32_t crc = ~current;
            while (length >= 64)
            {
                for (int i = 0; i < 4; i++)
                {
                    uint32_t s[4];
                    memcpy(s, p, sizeof(s));

                    s[0] ^= crc;
                    p += sizeof(s);
                    crc = crc32_table_n<polynomial, 4 * 0 + 0>[static_cast<uint8_t>(s[3] >> 24)]
                        ^ crc32_table_n<polynomial, 4 * 0 + 1>[static_cast<uint8_t>(s[3] >> 16)]
                        ^ crc32_table_n<polynomial, 4 * 0 + 2>[static_cast<uint8_t>(s[3] >> 8)]
                        ^ crc32_table_n<polynomial, 4 * 0 + 3>[static_cast<uint8_t>(s[3] >> 0)]
                        ^ crc32_table_n<polynomial, 4 * 1 + 0>[static_cast<uint8_t>(s[2] >> 24)]
                        ^ crc32_table_n<polynomial, 4 * 1 + 1>[static_cast<uint8_t>(s[2] >> 16)]
                        ^ crc32_table_n<polynomial, 4 * 1 + 2>[static_cast<uint8_t>(s[2] >> 8)]
                        ^ crc32_table_n<polynomial, 4 * 1 + 3>[static_cast<uint8_t>(s[2] >> 0)]
                        ^ crc32_table_n<polynomial, 4 * 2 + 0>[static_cast<uint8_t>(s[1] >> 24)]
                        ^ crc32_table_n<polynomial, 4 * 2 + 1>[static_cast<uint8_t>(s[1] >> 16)]
                        ^ crc32_table_n<polynomial, 4 * 2 + 2>[static_cast<uint8_t>(s[1] >> 8)]
                        ^ crc32_table_n<polynomial, 4 * 2 + 3>[static_cast<uint8_t>(s[1] >> 0)]
                        ^ crc32_table_n<polynomial, 4 * 3 + 0>[static_cast<uint8_t>(s[0] >> 24)]
                        ^ crc32_table_n<polynomial, 4 * 3 + 1>[static_cast<uint8_t>(s[0] >> 16)]
                        ^ crc32_table_n<polynomial, 4 * 3 + 2>[static_cast<uint8_t>(s[0] >> 8)]
                        ^ crc32_table_n<polynomial, 4 * 3 + 3>[static_cast<uint8_t>(s[0] >> 0)];
                }
                length -= 64;
            }
            current = ~crc;

            // process remain bytes
            return crc32::calculate_crc32<polynomial>(p, length, current);
        }
    }
}
