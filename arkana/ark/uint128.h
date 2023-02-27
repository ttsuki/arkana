/// @file
/// @brief	arkana::bits - 128 bit uint operations
/// @author Copyright(c) 2021 ttsuki
/// 
/// This software is released under the MIT License.
/// https://opensource.org/licenses/MIT

#pragma once
#include "./intrinsics.h"

#if __has_include("./hexilit.h")
#  include "./hexilit.h"
#endif

namespace arkana::uint128
{
#ifdef __SIZEOF_INT128__ // if compiler has __int128

    using uint128_t = unsigned __int128;

#if __has_include("./hexilit.h")
    inline namespace literals
    {
        template <char...cs>
        static inline constexpr uint128_t operator ""_u128()
        {
            using namespace hexilit::parser;
            constexpr auto arr = parse_hexint_array<uint64_t, cs...>(is_in_range<1, 32>, always, never);
            static_assert(arr.size() == 1 || arr.size() == 2);
            if constexpr (arr.size() == 1) { return static_cast<uint128_t>(arr[0]); }
            else { return static_cast<uint128_t>(arr[0]) << 64 | arr[1]; }
        }
    }
#endif

#else

    using uint128_t = intrinsics::uint64x2_t;

#if __has_include("./hexilit.h")
    inline namespace literals
    {
        template <char... cs>
        static inline constexpr uint128_t operator ""_u128()
        {
            using namespace hexilit::parser;
            constexpr auto arr = parse_hexint_array<uint64_t, cs...>(is_in_range<1, 32>, always, never);
            static_assert(arr.size() == 1 || arr.size() == 2);
            if constexpr (arr.size() == 1) { return static_cast<uint128_t>(arr[0]); }
            else { return static_cast<uint128_t>(arr[0]) << 64 | arr[1]; }
        }
    }
#endif

#endif

    using intrinsics::rotr;
    using intrinsics::rotl;
    using intrinsics::byteswap;
    using intrinsics::shld;
    using intrinsics::shrd;
}
