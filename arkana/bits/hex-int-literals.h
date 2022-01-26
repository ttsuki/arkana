/// @file
/// @brief	arkana::hex_int_literals
///         - a hexadecimal integer literal parser
/// @author Copyright(c) 2021 ttsuki
/// 
/// This software is released under the MIT License.
/// https://opensource.org/licenses/MIT

#pragma once

#include <cstddef>
#include <cstdint>
#include <array>

namespace arkana::hex_int_literals
{
    namespace parser
    {
        static inline constexpr bool never(std::size_t) { return false; }
        static inline constexpr bool always(std::size_t) { return true; }
        template <std::size_t X> static inline constexpr bool isXn(std::size_t i) { return i % X == 0; }
        static inline constexpr bool is2n(std::size_t i) { return isXn<2>(i); }
        static inline constexpr bool is4n(std::size_t i) { return isXn<4>(i); }
        static inline constexpr bool is8n(std::size_t i) { return isXn<8>(i); }
        static inline constexpr bool is16n(std::size_t i) { return isXn<16>(i); }
        static inline constexpr bool is32n(std::size_t i) { return isXn<32>(i); }
        template <std::size_t Fixed> static inline constexpr bool is(std::size_t i) { return i == Fixed; }
        template <std::size_t Minimum, std::size_t Maximum> static inline constexpr bool is_in_range(std::size_t i) { return i >= Minimum && i <= Maximum; }

        template <class T,
            char CharZero, char CharHex, char...HexDigitChars,
            class LengthIsAcceptable = decltype(always),
            class SeparatorIsAcceptable = decltype(always),
            class SeparatorIsRequired = decltype(never)>
            static inline constexpr auto parse_hexint_array(
                LengthIsAcceptable length_is_acceptable = always,
                SeparatorIsAcceptable separator_is_acceptable = always,
                SeparatorIsRequired separator_is_required = never)
        {
            // accepts only hexadecimal integer
            if constexpr (CharZero != '0') throw "Only hexadecimal number are accepted.";
            if constexpr (CharHex != 'x' && CharHex != 'X') throw "Only hexadecimal number are accepted.";

            // digit part
            constexpr std::array<char, sizeof...(HexDigitChars)> input{ HexDigitChars... };

            constexpr auto is_separator = [](char input) -> bool
            {
                return input == '\'';
            };

            constexpr auto parse_to_nibble = [](char input) -> int
            {
                if (input >= '0' && input <= '9') return input - '0';
                if (input >= 'A' && input <= 'F') return input - 'A' + 10;
                if (input >= 'a' && input <= 'f') return input - 'a' + 10;
                throw "Only hexadecimal integer is accepted.";
            };

            // count output length
            constexpr std::size_t nibble_count = [&]()
            {
                std::size_t nibble_index = 0;
                for (std::size_t i = 0; i < input.size(); i++)
                {
                    char c = input[i];
                    if (is_separator(c)) continue;
                    parse_to_nibble(input[i]);
                    nibble_index++;
                }
                return nibble_index;
            }();

            if (!length_is_acceptable(nibble_count))
                throw "Invalid length";

            // makes output 
            constexpr std::size_t nibble_count_per_element = sizeof(T) * 2;
            constexpr std::size_t element_count = (nibble_count + nibble_count_per_element - 1) / nibble_count_per_element;
            std::array<T, element_count> result{};

            std::size_t nibble_index = (~nibble_count + 1) & (nibble_count_per_element - 1);
            bool last_character_is_separator = true;

            for (std::size_t i = 0; i < input.size(); i++)
            {
                char c = input[i];
                if (is_separator(c))
                {
                    if (!separator_is_acceptable(nibble_index))
                        throw "Invalid separator position.";
                }
                else
                {
                    if (!last_character_is_separator
                        && separator_is_required(nibble_index))
                        throw "Invalid separator position.";

                    result[nibble_index / nibble_count_per_element] <<= 4;
                    result[nibble_index / nibble_count_per_element] |= static_cast<T>(parse_to_nibble(c));
                    ++nibble_index;
                }

                last_character_is_separator = is_separator(c);
            }

            return result;
        }
    }

    template <char...cs>
    static inline constexpr auto operator ""_byte_array()
    {
        using namespace parser;
        return parse_hexint_array<std::byte, cs...>(is2n, is2n, never);
    }

    template <char...cs>
    static inline constexpr auto operator ""_u8_array()
    {
        using namespace parser;
        return parse_hexint_array<std::uint8_t, cs...>(is2n, is2n, never);
    }

    template <char...cs>
    static inline constexpr auto operator ""_u16_array()
    {
        using namespace parser;
        return parse_hexint_array<std::uint16_t, cs...>(is4n, is4n, is4n);
    }

    template <char...cs>
    static inline constexpr auto operator ""_u32_array()
    {
        using namespace parser;
        return parse_hexint_array<std::uint32_t, cs...>(is8n, is8n, is8n);
    }

    template <char...cs>
    static inline constexpr auto operator ""_u64_array()
    {
        using namespace parser;
        return parse_hexint_array<std::uint64_t, cs...>(is16n, is16n, is16n);
    }
}
