/// @file
/// @brief	arkana::base64 - compile-time base64
/// @author Copyright(c) 2022 ttsuki
/// 
/// This software is released under the MIT License.
/// https://opensource.org/licenses/MIT

#pragma once

#include <cstddef>
#include <cstdint>
#include <array>

namespace arkana::base64::compile_time
{
    template <std::size_t N, std::size_t L = N - (N + 6) / 4>
    static constexpr std::array<std::byte, L> decode_base64(const char(&input)[N])
    {
        static_assert(N % 4 != 2, "decode_base64_constexpr: Invalid input length.");
        static_assert(L == N - (N + 6) / 4, "decode_base64_constexpr: Invalid output length.");

        std::array<std::byte, L> result{};
        std::uint32_t buf = {};

        for (std::size_t i = 0; i < N - 1; i++)
        {
            constexpr std::uint8_t _x_ = 0xFF;
            constexpr std::uint8_t tbl[] = {
                _x_, _x_, _x_, _x_, _x_, _x_, _x_, _x_, _x_, _x_, _x_, 076, _x_, _x_, _x_, 077,
                064, 065, 066, 067, 070, 071, 072, 073, 074, 075, _x_, _x_, _x_, _x_, _x_, _x_,
                _x_, 000, 001, 002, 003, 004, 005, 006, 007, 010, 011, 012, 013, 014, 015, 016,
                017, 020, 021, 022, 023, 024, 025, 026, 027, 030, 031, _x_, _x_, _x_, _x_, _x_,
                _x_, 032, 033, 034, 035, 036, 037, 040, 041, 042, 043, 044, 045, 046, 047, 050,
                051, 052, 053, 054, 055, 056, 057, 060, 061, 062, 063, _x_, _x_, _x_, _x_, _x_,
            };

            if (input[i] < 0x20) throw "decode_base64_constexpr: Invalid character detected.";
            if (input[i] == '=') throw "decode_base64_constexpr: Invalid character detected. Please remove trailing '='";
            if (tbl[input[i] - 0x20] == _x_) throw "decode_base64_constexpr: Invalid character detected.";

            buf = buf << 6 | tbl[input[i] - 0x20];

            if (i % 4 == 1) result[i / 4 * 3 + 0] = static_cast<std::byte>(buf >> 4);
            if (i % 4 == 2) result[i / 4 * 3 + 1] = static_cast<std::byte>(buf >> 2);
            if (i % 4 == 3) result[i / 4 * 3 + 2] = static_cast<std::byte>(buf >> 0);
        }

        return result;
    }
}
