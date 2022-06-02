/// @file
/// @brief	arkana::bits - 128 bit uint operations
/// @author Copyright(c) 2021 ttsuki
/// 
/// This software is released under the MIT License.
/// https://opensource.org/licenses/MIT

#pragma once

#include <cstdint>
#include <cstdlib>
#include <utility>

#if defined(_MSC_VER)
#include <intrin.h>
#else
#include <x86intrin.h>
#endif

#if __has_include("./hex-int-literals.h")
#  include "./hex-int-literals.h"
#endif

namespace arkana::bits::intrinsics
{
    static inline constexpr auto decompose64(uint64_t v) noexcept
    {
        return std::pair{static_cast<uint32_t>(v), static_cast<uint32_t>(v >> 32)};
    }

    static inline uint64_t shld64(uint64_t l, uint64_t h, int i) noexcept
    {
#if defined(_MSC_VER) && defined(_M_X64)
        // 64bit MSVC
        return __shiftleft128(l, h, static_cast<unsigned char>(i));
#else
        return (i & 63) ? h << (i & 63) | l >> (-i & 63) : h;
#endif
    }

    static inline uint64_t shrd64(uint64_t l, uint64_t h, int i) noexcept
    {
#if defined(_MSC_VER) && defined(_M_X64)
        // 64bit MSVC
        return __shiftright128(l, h, static_cast<unsigned char>(i));
#else
        return (i & 63) ? l >> (i & 63) | h << (-i & 63) : l;
#endif
    }

    static inline uint8_t adc32(uint8_t cf, uint32_t a, uint32_t b, uint32_t* result) noexcept
    {
        return _addcarry_u32(cf, a, b, result);
    }

    static inline uint8_t sbb32(uint8_t cf, uint32_t a, uint32_t b, uint32_t* result) noexcept
    {
        return _subborrow_u32(cf, a, b, result);
    }

    static inline uint64_t mul32(uint32_t a, uint32_t b) noexcept
    {
#if defined(_MSC_VER)
        // 32bit MSVC
        return __emulu(a, b);
#else
        return static_cast<uint64_t>(a) * b;
#endif
    }

    static inline uint8_t adc64(uint8_t cf, uint64_t a, uint64_t b, uint64_t* result) noexcept
    {
#if (defined(_MSC_VER) && _MSC_VER >= 1920 && defined(_M_X64)) || defined(__x86_64__)
        // 64bit MSVC(2019 or later: VS2017 has bug?), GCC, clang
        static_assert(sizeof(uint64_t) == sizeof(unsigned long long));
        return _addcarry_u64(cf, a, b, reinterpret_cast<unsigned long long*>(result));
#else
        uint32_t l32, h32;
        cf = adc32(cf, static_cast<uint32_t>(a), static_cast<uint32_t>(b), &l32);
        cf = adc32(cf, static_cast<uint32_t>(a >> 32), static_cast<uint32_t>(b >> 32), &h32);
        *result = static_cast<uint64_t>(h32) << 32 | l32;
        return cf;
#endif
    }

    static inline uint8_t sbb64(uint8_t cf, uint64_t a, uint64_t b, uint64_t* result) noexcept
    {
#if (defined(_MSC_VER) && _MSC_VER >= 1920 && defined(_M_X64)) || defined(__x86_64__)
        // 64bit MSVC(2019 or later: VS2017 has bug?), GCC, clang
        static_assert(sizeof(uint64_t) == sizeof(unsigned long long));
        return _subborrow_u64(cf, a, b, reinterpret_cast<unsigned long long*>(result));
#else
        uint32_t l32, h32;
        cf = sbb32(cf, static_cast<uint32_t>(a), static_cast<uint32_t>(b), &l32);
        cf = sbb32(cf, static_cast<uint32_t>(a >> 32), static_cast<uint32_t>(b >> 32), &h32);
        *result = static_cast<uint64_t>(h32) << 32 | l32;
        return cf;
#endif
    }

    static inline uint64_t muld64(uint64_t a, uint64_t b, uint64_t* h) noexcept
    {
#if defined(_MSC_VER) && defined(_M_X64)
        // 64bit MSVC
        return _umul128(a, b, h);
#else
        auto [al, ah] = decompose64(a);
        auto [bl, bh] = decompose64(b);

        auto [ll, lh] = decompose64(mul32(al, bl)); // ___a * ___b = __ll
        auto [xl, xh] = decompose64(mul32(ah, bl)); // __a_ * ___b = _xx_
        auto [yl, yh] = decompose64(mul32(al, bh)); // ___a * __b_ = _yy_
        auto [hl, hh] = decompose64(mul32(ah, bh)); // __a_ * __b_ = hh__

        hh += adc32(adc32(0, lh, xl, &lh), hl, xh, &hl);
        hh += adc32(adc32(0, lh, yl, &lh), hl, yh, &hl);

        *h = static_cast<uint64_t>(hh) << 32 | hl;
        return static_cast<uint64_t>(lh) << 32 | ll;
#endif
    }

    static inline uint64_t bswap64(uint64_t v) noexcept
    {
#if defined(_MSC_VER)
        return _byteswap_uint64(v);
#elif defined (__GNUC__) || defined(__clang__)
        return __builtin_bswap64(v);
#else
        v = (v >> 8 & 0x00FF00FF00FF00FFull) | (v & 0x00FF00FF00FF00FFull) << 8;
        v = (v >> 16 & 0x0000FFFF0000FFFFull) | (v & 0x0000FFFF0000FFFFull) << 16;
        v = (v >> 32 & 0x00000000FFFFFFFFull) | (v & 0x00000000FFFFFFFFull) << 32;
        return v;
#endif
    }
}

namespace arkana::bits::uint128
{
#ifdef __SIZEOF_INT128__ // if compiler has __int128

    using uint128_t = unsigned __int128;

    static inline constexpr uint128_t byteswap(uint128_t a) noexcept
    {
        uint64_t l = static_cast<uint64_t>(a);
        uint64_t h = static_cast<uint64_t>(a >> 64);
        return static_cast<uint128_t>(__builtin_bswap64(l)) << 64 | __builtin_bswap64(h);
    }

    static inline constexpr uint128_t rotl(uint128_t a, int i) noexcept
    {
        return a << (i & 127) | a >> (-i & 127);
    }

    static inline constexpr uint128_t rotr(uint128_t a, int i) noexcept
    {
        return rotl(a, -i);
    }

#if __has_include("./hex-int-literals.h")
    inline namespace literals
    {
        template <char...cs>
        static inline constexpr uint128_t operator "" _u128()
        {
            using namespace hex_int_literals::parser;
            constexpr auto arr = parse_hexint_array<uint64_t, cs...>(is_in_range<1, 32>, always, never);
            static_assert(arr.size() == 1 || arr.size() == 2);
            if constexpr (arr.size() == 1) { return static_cast<uint128_t>(arr[0]); }
            else { return static_cast<uint128_t>(arr[0]) << 64 | arr[1]; }
        }
    }
#endif
#else

    struct uint128_t
    {
        uint64_t l{}, h{};

        inline constexpr uint128_t() = default;

        inline constexpr uint128_t(uint64_t l, uint64_t h = {})
            : l{l}
            , h{h}
        {
        }

        inline constexpr explicit operator bool() const noexcept { return l | h; }
        inline constexpr explicit operator uint64_t() const noexcept { return l; }
    };

    static inline bool operator ==(uint128_t a, uint128_t b) noexcept { return !((a.h ^ b.h) | (a.l ^ b.l)); }
    static inline bool operator !=(uint128_t a, uint128_t b) noexcept { return !(a == b); }
    static inline bool operator <(uint128_t a, uint128_t b) noexcept { return intrinsics::sbb64(a.l < b.l, a.h, b.h, &a.h); }
    static inline bool operator >(uint128_t a, uint128_t b) noexcept { return b < a; }
    static inline bool operator <=(uint128_t a, uint128_t b) noexcept { return !(a > b); }
    static inline bool operator >=(uint128_t a, uint128_t b) noexcept { return !(a < b); }

    static inline uint128_t operator +(uint128_t a, uint128_t b) noexcept
    {
        a.h += b.h + intrinsics::adc64(0, a.l, b.l, &a.l);
        return a;
    }

    static inline uint128_t operator -(uint128_t a, uint128_t b) noexcept
    {
        a.h -= b.h + intrinsics::sbb64(0, a.l, b.l, &a.l);
        return a;
    }

    static inline uint128_t operator *(uint128_t a, uint128_t b) noexcept
    {
        uint64_t h;
        uint64_t l = intrinsics::muld64(a.l, b.l, &h);
        h += a.l * b.h + a.h * b.l;
        return {l, h};
    }

    static inline uint128_t operator ~(uint128_t a) noexcept { return {~a.l, ~a.h}; }
    static inline uint128_t operator &(uint128_t a, uint128_t b) noexcept { return {a.l & b.l, a.h & b.h}; }
    static inline uint128_t operator |(uint128_t a, uint128_t b) noexcept { return {a.l | b.l, a.h | b.h}; }
    static inline uint128_t operator ^(uint128_t a, uint128_t b) noexcept { return {a.l ^ b.l, a.h ^ b.h}; }

    static inline uint128_t operator <<(uint128_t a, int i) noexcept
    {
        uint64_t l = a.l << (i & 63);
        uint64_t h = intrinsics::shld64(a.l, a.h, static_cast<unsigned char>(i));
        return i & 64 ? uint128_t{0, l} : uint128_t{l, h};
    }

    static inline uint128_t operator >>(uint128_t a, int i) noexcept
    {
        uint64_t l = intrinsics::shrd64(a.l, a.h, static_cast<unsigned char>(i));
        uint64_t h = a.h >> (i & 63);
        return i & 64 ? uint128_t{h, 0} : uint128_t{l, h};
    }

    static inline uint128_t rotl(const uint128_t& v, int i) noexcept
    {
        auto l = intrinsics::shld64(v.h, v.l, i);
        auto h = intrinsics::shld64(v.l, v.h, i);
        return i & 64 ? uint128_t{h, l} : uint128_t{l, h};
    }

    static inline uint128_t rotr(const uint128_t& v, int i) noexcept
    {
        auto l = intrinsics::shrd64(v.l, v.h, i);
        auto h = intrinsics::shrd64(v.h, v.l, i);
        return i & 64 ? uint128_t{h, l} : uint128_t{l, h};
    }

    static inline uint128_t byteswap(const uint128_t& a) noexcept
    {
        return {intrinsics::bswap64(a.h), intrinsics::bswap64(a.l)};
    }

    static inline uint128_t& operator +=(uint128_t& a, uint128_t b) noexcept { return a = a + b; }
    static inline uint128_t& operator -=(uint128_t& a, uint128_t b) noexcept { return a = a - b; }
    static inline uint128_t& operator *=(uint128_t& a, uint128_t b) noexcept { return a = a * b; }
    static inline uint128_t& operator &=(uint128_t& a, uint128_t b) noexcept { return a = a & b; }
    static inline uint128_t& operator |=(uint128_t& a, uint128_t b) noexcept { return a = a | b; }
    static inline uint128_t& operator ^=(uint128_t& a, uint128_t b) noexcept { return a = a ^ b; }
    static inline uint128_t& operator <<=(uint128_t& a, int b) noexcept { return a = a << b; }
    static inline uint128_t& operator >>=(uint128_t& a, int b) noexcept { return a = a << b; }

#if __has_include("./hex-int-literals.h")
    inline namespace literals
    {
        template <char...cs>
        static inline constexpr uint128_t operator "" _u128()
        {
            using namespace hex_int_literals::parser;
            constexpr auto arr = parse_hexint_array<uint64_t, cs...>(is_in_range<1, 32>, always, never);
            static_assert(arr.size() == 1 || arr.size() == 2);
            if constexpr (arr.size() == 1) { return uint128_t{arr[0]}; }
            else { return uint128_t{arr[1], arr[0]}; }
        }
    }
#endif

#endif
}

namespace arkana::bits
{
    using uint128::rotl;
    using uint128::rotr;
    using uint128::byteswap;
}

