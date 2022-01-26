/// @file
/// @brief	arkana::crc32
///			- An implementation of CRC32
/// @author Copyright(c) 2020 ttsuki
/// 
/// This software is released under the MIT License.
/// https://opensource.org/licenses/MIT

#pragma once

#include "../bits/types.h"
#include "../bits/table.h"

namespace arkana::crc32
{
    using crc32_value_t = uint32_t;

    template <uint32_t polynomial>
    static inline constexpr table<uint32_t, 256> crc32_table0 = generate_table<uint32_t, 256>(
        [](auto i)
        {
            uint32_t v = static_cast<uint32_t>(i);
            for (uint32_t j = 0; j < 8; j++)
                v = (v >> 1) ^ ((v & 1) * polynomial);
            return v;
        });

    template <uint32_t polynomial>
    static inline constexpr crc32_value_t calculate_crc32(const void* data, size_t length, crc32_value_t current = 0)
    {
        auto& table = crc32_table0<polynomial>;
        const byte_t* p = static_cast<const byte_t*>(data);
        const byte_t* q = p + length;

        crc32_value_t crc = ~current;
        while (p != q)
        {
            crc = (crc >> 8) ^ table[(crc & 0xFF) ^ static_cast<uint8_t>(*p++)];
        }

        return ~crc;
    }

    template <uint32_t polynomial, size_t n>
    static inline constexpr table<uint32_t, 256> crc32_table_n = generate_table<uint32_t, 256>(
        [](auto i)
        {
            auto& table = crc32_table0<polynomial>;
            auto v = crc32_table_n<polynomial, n - 1>[i];
            return (v >> 8) ^ table[(v & 0xFF)];
        });

    template <uint32_t polynomial>
    static inline constexpr table<uint32_t, 256> crc32_table_n<polynomial, 0> = crc32_table0<polynomial>;
}
