/// @file
/// @brief	arkana::ark::intrinsics
/// @author Copyright(c) 2021 ttsuki
/// 
/// This software is released under the MIT License.
/// https://opensource.org/licenses/MIT

#pragma once

#if defined(_MSC_VER)
#include <intrin.h>
#define ARKANA_FORCEINLINE inline __forceinline
#define ARKANA_ASSUME __assume
#else
#include <x86intrin.h>
#define ARKANA_FORCEINLINE inline __attribute__((always_inline))
#define ARKANA_ASSUME do { if (!(cond)) __builtin_unreachable(); } while (0)
#endif

#ifndef NDEBUG
#undef ARKANA_FORCEINLINE
#define ARKANA_FORCEINLINE
#undef ARKANA_ASSUME
#define ARKANA_ASSUME assert
#endif

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cassert>

#include <utility>
#include <type_traits>
#include <numeric>

#if __has_include("hexilit.h")
#  include "hexilit.h"
#endif

namespace arkana::intrinsics
{
    using std::memcpy;
    using std::memset;

    /// Loads T from unaligned memory pointer
    template <class T, std::enable_if_t<std::is_trivially_copyable_v<T>>* = nullptr>
    static inline T load_u(const void* src) noexcept
    {
        T t;
        memcpy(&t, src, sizeof(T));
        return t;
    }

    /// Stores T to unaligned memory pointer
    template <class T, std::enable_if_t<std::is_trivially_copyable_v<T>>* = nullptr>
    static inline auto store_u(void* d, const std::decay_t<T>& s) noexcept
    {
        memcpy(d, &s, sizeof(T));
    }

    /// bit_cast
    template <class To, class From,
              std::enable_if_t<
                  sizeof(To) == sizeof(From) &&
                  std::is_trivially_copyable_v<To> &&
                  std::is_trivially_copyable_v<From>>* = nullptr>
    static inline To bit_cast(const From& f) noexcept
    {
        return load_u<To>(&f);
    }

    /// type_punning_cast
    template <class To, class From,
              std::enable_if_t<
                  std::is_lvalue_reference_v<To> &&
                  sizeof(std::remove_reference_t<To>) == sizeof(From) &&
                  std::is_trivially_copyable_v<std::remove_reference_t<To>> &&
                  std::is_trivially_copyable_v<From>> * = nullptr>
    static inline std::remove_reference_t<To>& type_punning_cast(From& f) noexcept
    {
        return reinterpret_cast<std::remove_reference_t<To>&>(f); // **UB**
    }

    // mem_zero

#if defined(_MSC_VER)
    static_assert(sizeof(unsigned long) == sizeof(uint32_t));
    static inline void secure_memzero(uint8_t* memory, size_t count) noexcept { ::__stosb(memory, 0, count); }
    static inline void secure_memzero(uint16_t* memory, size_t count) noexcept { ::__stosw(memory, 0, count); }
    static inline void secure_memzero(uint32_t* memory, size_t count) noexcept { ::__stosd(reinterpret_cast<unsigned long*>(memory), 0, count); }
#if defined(_MSC_VER) && defined(_M_X64)
    static inline void secure_memzero(uint64_t* memory, size_t count) noexcept { ::__stosq(memory, 0, count); }
#else
    static inline void secure_memzero(uint64_t* memory, size_t count) noexcept { ::__stosd(reinterpret_cast<unsigned long*>(memory), 0, 2 * count); }
#endif
#else
    static inline void secure_memzero(uint8_t* memory, size_t count) { for (size_t i = 0; i < count; i++) static_cast<volatile uint8_t*>(memory)[i] = 0; }
    static inline void secure_memzero(uint16_t* memory, size_t count) { for (size_t i = 0; i < count; i++) static_cast<volatile uint16_t*>(memory)[i] = 0; }
    static inline void secure_memzero(uint32_t* memory, size_t count) { for (size_t i = 0; i < count; i++) static_cast<volatile uint32_t*>(memory)[i] = 0; }
    static inline void secure_memzero(uint64_t* memory, size_t count) { for (size_t i = 0; i < count; i++) static_cast<volatile uint64_t*>(memory)[i] = 0; }
#endif

    template <class T, std::enable_if_t<std::is_trivially_destructible_v<T> && sizeof(T) % sizeof(uint32_t) != 0>* = nullptr>
    static inline void secure_be_zero(T& t) noexcept { secure_memzero(reinterpret_cast<uint8_t*>(&t), sizeof(t)); }

    template <class T, std::enable_if_t<std::is_trivially_destructible_v<T> && sizeof(T) % sizeof(uint32_t) == 0 && sizeof(T) % sizeof(uint64_t) != 0>* = nullptr>
    static inline void secure_be_zero(T& t) noexcept { secure_memzero(reinterpret_cast<uint32_t*>(&t), sizeof(t) / sizeof(uint32_t)); }

    template <class T, std::enable_if_t<std::is_trivially_destructible_v<T> && sizeof(T) % sizeof(uint64_t) == 0>* = nullptr>
    static inline void secure_be_zero(T& t) noexcept { secure_memzero(reinterpret_cast<uint64_t*>(&t), sizeof(t) / sizeof(uint64_t)); }
}

namespace arkana::intrinsics
{
    using carry_flag_t = unsigned char;
    template <class uint_t> static inline carry_flag_t adc(carry_flag_t cf, uint_t& a, uint_t b) noexcept = delete;
    template <class uint_t> static inline carry_flag_t sbb(carry_flag_t cf, uint_t& a, uint_t b) noexcept = delete;
    template <class uint_t> static inline auto muld(uint_t a, uint_t b) noexcept = delete;
    template <class uint_t> static inline uint_t rotl(uint_t v, int i) noexcept = delete;
    template <class uint_t> static inline uint_t rotr(uint_t v, int i) noexcept = delete;
    template <class uint_t> static inline uint_t byteswap(uint_t v) noexcept = delete;
    template <class uint_t> static inline uint_t shld(uint_t l, uint_t h, int i) noexcept = delete;
    template <class uint_t> static inline uint_t shrd(uint_t l, uint_t h, int i) noexcept = delete;

    inline namespace uint_type_traits
    {
        template <class uint_t>
        static constexpr bool is_unsigned_integral_v = std::is_integral_v<uint_t> && std::is_unsigned_v<uint_t>;

        template <class uint_t1, class uint_t2, std::enable_if_t<is_unsigned_integral_v<uint_t1> && std::is_unsigned_v<uint_t2>>* = nullptr>
        static constexpr bool uint_size_le = (std::numeric_limits<uint_t1>::digits) <= (std::numeric_limits<uint_t2>::digits);
    }

    template <class uint_t>
    struct uint_pair
    {
        using half_type = uint_t;
        static constexpr inline int half_bits = std::numeric_limits<uint_t>::digits;

        /// lower half
        uint_t l{};

        /// higher half
        uint_t h{};

        constexpr uint_pair() = default;
        constexpr explicit uint_pair(uint_t l, uint_t h) : l(l), h(h) {}
        constexpr uint_pair(const uint_pair& other) = default;
        constexpr uint_pair(uint_pair&& other) noexcept = default;
        constexpr uint_pair& operator=(const uint_pair& other) = default;
        constexpr uint_pair& operator=(uint_pair&& other) noexcept = default;

        // is non-zero
        constexpr explicit operator bool() const noexcept { return static_cast<bool>(l | h); }

        // from small type
        template <class uint_from_t, std::enable_if_t<uint_size_le<uint_from_t, uint_t>>* = nullptr>
        constexpr uint_pair(uint_from_t l) : l{l}, h{} {}

        // from large type
        template <class uint_from_t, std::enable_if_t<!uint_size_le<uint_from_t, uint_t>>* = nullptr>
        constexpr explicit uint_pair(uint_from_t l) : l{static_cast<uint_t>(l)}, h{static_cast<uint_t>(l >> half_bits)} {}

        // to small type
        template <class uint_to_t, std::enable_if_t<uint_size_le<uint_to_t, uint_t>>* = nullptr>
        constexpr explicit operator uint_to_t() const noexcept { return static_cast<uint_to_t>(l); }

        // to large type
        template <class uint_to_t, std::enable_if_t<!uint_size_le<uint_to_t, uint_t>>* = nullptr>
        constexpr explicit operator uint_to_t() const noexcept { return static_cast<uint_to_t>(h) << half_bits | l; }
    };

    using uint32x2_t = uint_pair<uint32_t>;
    using uint64x2_t = uint_pair<uint64_t>;
    template <class uint_t> static inline constexpr uint_pair<uint_t> decompose(const uint_pair<uint_t>& p) noexcept { return p; }


    template <class uint> static inline constexpr auto decompose(uint p) noexcept = delete;
    static inline constexpr uint32x2_t decompose(uint64_t v) noexcept { return uint32x2_t{static_cast<uint32_t>(v), static_cast<uint32_t>(v >> 32)}; }
#if defined(__SIZEOF_INT128__) // compiler has uint128
    static inline constexpr uint64x2_t decompose(unsigned __int128 v) noexcept { return uint64x2_t{ static_cast<uint64_t>(v), static_cast<uint64_t>(v >> 64) }; }
#endif

    // 32

    template <> inline carry_flag_t adc(carry_flag_t cf, uint32_t& a, uint32_t b) noexcept { return _addcarry_u32(cf, a, b, &a); }
    template <> inline carry_flag_t sbb(carry_flag_t cf, uint32_t& a, uint32_t b) noexcept { return _subborrow_u32(cf, a, b, &a); }
    template <> inline auto /* uint64_t */ muld(uint32_t a, uint32_t b) noexcept { return static_cast<uint64_t>(a) * b; } //* emits sufficiently efficient code (even without __emulu).

    // 64

    template <> inline carry_flag_t adc(carry_flag_t cf, uint64_t& a, uint64_t b) noexcept
    {
#if (defined(_MSC_VER) && _MSC_VER >= 1920 && defined(_M_X64)) || defined(__x86_64__) // 64bit MSVC(2019 or later), GCC, clang
        static_assert(sizeof(uint64_t) == sizeof(unsigned long long));
        return _addcarry_u64(cf, a, b, reinterpret_cast<unsigned long long*>(&a));
#else // delegates to 32bit operations
        uint32_t al = static_cast<uint32_t>(a), ah = static_cast<uint32_t>(a >> 32);
        cf = adc(cf, al, static_cast<uint32_t>(b));
        cf = adc(cf, ah, static_cast<uint32_t>(b >> 32));
        a = static_cast<uint64_t>(ah) << 32 | al;
        return cf;
#endif
    }

    template <> inline carry_flag_t sbb(carry_flag_t cf, uint64_t& a, uint64_t b) noexcept
    {
#if (defined(_MSC_VER) && _MSC_VER >= 1920 && defined(_M_X64)) || defined(__x86_64__)    // 64bit MSVC(2019 or later), GCC, clang
        static_assert(sizeof(uint64_t) == sizeof(unsigned long long));
        return _subborrow_u64(cf, a, b, reinterpret_cast<unsigned long long*>(&a));
#else // delegates to 32bit operations
        uint32_t al = static_cast<uint32_t>(a), ah = static_cast<uint32_t>(a >> 32);
        cf = sbb(cf, al, static_cast<uint32_t>(b));
        cf = sbb(cf, ah, static_cast<uint32_t>(b >> 32));
        a = static_cast<uint64_t>(ah) << 32 | al;
        return cf;
#endif
    }

    template <> inline auto muld(uint64_t a, uint64_t b) noexcept
    {
#if defined(__SIZEOF_INT128__) // compiler has uint128
        return decompose(static_cast<unsigned __int128>(a) * b);
#elif defined(_MSC_VER) && defined(_M_X64) // 64bit MSVC
        uint64_t h;
        uint64_t l = _umul128(a, b, &h);
        return uint64x2_t{l, h};
#else // naive
        auto [al, ah] = decompose(a);
        auto [bl, bh] = decompose(b);
        auto [ll, lh] = decompose(muld(al, bl)); // ___a * ___b = __ll
        auto [xl, xh] = decompose(muld(ah, bl)); // __a_ * ___b = _xx_
        auto [yl, yh] = decompose(muld(al, bh)); // ___a * __b_ = _yy_
        auto [hl, hh] = decompose(muld(ah, bh)); // __a_ * __b_ = hh__
        hh += adc(adc(0, lh, xl), hl, xh);
        hh += adc(adc(0, lh, yl), hl, yh);
        return uint64x2_t{ static_cast<uint64_t>(lh) << 32 | ll, static_cast<uint64_t>(hh) << 32 | hl };
#endif
    }

    namespace naive
    {
        template <class uint> static inline constexpr uint rotl(uint v, int i) noexcept = delete;
        template <class uint> static inline constexpr uint rotr(uint v, int i) noexcept = delete;
        template <class uint> static inline constexpr uint byteswap(uint v) noexcept = delete;
        template <class uint> static inline constexpr uint shld(uint l, uint h, int i) noexcept = delete;
        template <class uint> static inline constexpr uint shrd(uint l, uint h, int i) noexcept = delete;

        template <> inline constexpr uint8_t rotl<uint8_t>(uint8_t v, int i) noexcept { return static_cast<uint8_t>(v << (i & 7) | v >> (-i & 7)); }
        template <> inline constexpr uint16_t rotl<uint16_t>(uint16_t v, int i) noexcept { return static_cast<uint16_t>(v << (i & 15) | v >> (-i & 15)); }
        template <> inline constexpr uint32_t rotl<uint32_t>(uint32_t v, int i) noexcept { return static_cast<uint32_t>(v << (i & 31) | v >> (-i & 31)); }
        template <> inline constexpr uint64_t rotl<uint64_t>(uint64_t v, int i) noexcept { return static_cast<uint64_t>(v << (i & 63) | v >> (-i & 63)); }
        template <> inline constexpr uint8_t rotr<uint8_t>(uint8_t v, int i) noexcept { return rotl(v, -i); }
        template <> inline constexpr uint16_t rotr<uint16_t>(uint16_t v, int i) noexcept { return rotl(v, -i); }
        template <> inline constexpr uint32_t rotr<uint32_t>(uint32_t v, int i) noexcept { return rotl(v, -i); }
        template <> inline constexpr uint64_t rotr<uint64_t>(uint64_t v, int i) noexcept { return rotl(v, -i); }
        template <> inline constexpr uint16_t byteswap<uint16_t>(uint16_t v) noexcept { return static_cast<uint16_t>(v << 8 | v >> 8); }
        template <> inline constexpr uint32_t byteswap<uint32_t>(uint32_t v) noexcept { return v << 24 | (v >> 8 & 0xFF00) | (v & 0xFF00) << 8 | v >> 24; }

        template <> inline constexpr uint64_t byteswap<uint64_t>(uint64_t v) noexcept
        {
            v = (v >> 8 & 0x00FF00FF00FF00FFull) | (v & 0x00FF00FF00FF00FFull) << 8;
            v = (v >> 16 & 0x0000FFFF0000FFFFull) | (v & 0x0000FFFF0000FFFFull) << 16;
            v = (v >> 32 & 0x00000000FFFFFFFFull) | (v & 0x00000000FFFFFFFFull) << 32;
            return v;
        }

        template <> inline constexpr uint32_t shld<uint32_t>(uint32_t l, uint32_t h, int i) noexcept { return (i & 31) ? h << (i & 31) | l >> (-i & 31) : h; }
        template <> inline constexpr uint64_t shld<uint64_t>(uint64_t l, uint64_t h, int i) noexcept { return (i & 63) ? h << (i & 63) | l >> (-i & 63) : h; }
        template <> inline constexpr uint32_t shrd<uint32_t>(uint32_t l, uint32_t h, int i) noexcept { return (i & 31) ? l >> (i & 31) | h << (-i & 31) : l; }
        template <> inline constexpr uint64_t shrd<uint64_t>(uint64_t l, uint64_t h, int i) noexcept { return (i & 63) ? l >> (i & 63) | h << (-i & 63) : l; }

#if defined(__SIZEOF_INT128__) // compiler has uint128
        template <> inline constexpr unsigned __int128 rotl<unsigned __int128>(unsigned __int128 v, int i) noexcept { return v << (i & 127) | v >> (-i & 127); }
        template <> inline constexpr unsigned __int128 rotr<unsigned __int128>(unsigned __int128 v, int i) noexcept { return rotl(v, -i); }
        template <> inline constexpr unsigned __int128 byteswap<unsigned __int128>(unsigned __int128 v) noexcept { return static_cast<unsigned __int128>(byteswap<uint64_t>(static_cast<uint64_t>(v))) << 64 | byteswap<uint64_t>(static_cast<uint64_t>(v >> 64)); }
        template <> inline constexpr unsigned __int128 shld< unsigned __int128>(unsigned __int128 l, unsigned __int128 h, int i) noexcept { return (i & 127) ? h << (i & 127) | l >> (-i & 127) : h; }
        template <> inline constexpr unsigned __int128 shrd< unsigned __int128>(unsigned __int128 l, unsigned __int128 h, int i) noexcept { return (i & 127) ? l >> (i & 127) | h << (-i & 127) : l; }
#endif
    }

#if defined(_MSC_VER)
    template <> inline uint8_t rotl(uint8_t v, int i) noexcept { return ::_rotl8(v, static_cast<unsigned char>(i)); }
    template <> inline uint16_t rotl(uint16_t v, int i) noexcept { return ::_rotl16(v, static_cast<unsigned char>(i)); }
    template <> inline uint32_t rotl(uint32_t v, int i) noexcept { return ::_rotl(v, i); }
    template <> inline uint64_t rotl(uint64_t v, int i) noexcept { return ::_rotl64(v, i); }
    template <> inline uint8_t rotr(uint8_t v, int i) noexcept { return ::_rotr8(v, static_cast<unsigned char>(i)); }
    template <> inline uint16_t rotr(uint16_t v, int i) noexcept { return ::_rotr16(v, static_cast<unsigned char>(i)); }
    template <> inline uint32_t rotr(uint32_t v, int i) noexcept { return ::_rotr(v, i); }
    template <> inline uint64_t rotr(uint64_t v, int i) noexcept { return ::_rotr64(v, i); }
    template <> inline uint16_t byteswap(uint16_t v) noexcept { return ::_byteswap_ushort(v); }
    template <> inline uint32_t byteswap(uint32_t v) noexcept { return ::_byteswap_ulong(v); }
    template <> inline uint64_t byteswap(uint64_t v) noexcept { return ::_byteswap_uint64(v); }
#elif defined(__clang__)
    template <> inline uint8_t rotl(uint8_t v, int i) noexcept { return ::__builtin_rotateleft8(v, static_cast<unsigned char>(i)); }
    template <> inline uint16_t rotl(uint16_t v, int i) noexcept { return ::__builtin_rotateleft16(v, static_cast<unsigned char>(i)); }
    template <> inline uint32_t rotl(uint32_t v, int i) noexcept { return ::__builtin_rotateleft32(v, static_cast<unsigned char>(i)); }
    template <> inline uint64_t rotl(uint64_t v, int i) noexcept { return ::__builtin_rotateleft64(v, static_cast<unsigned char>(i)); }
    template <> inline uint8_t rotr(uint8_t v, int i) noexcept { return ::__builtin_rotateright8(v, static_cast<unsigned char>(i)); }
    template <> inline uint16_t rotr(uint16_t v, int i) noexcept { return ::__builtin_rotateright16(v, static_cast<unsigned char>(i)); }
    template <> inline uint32_t rotr(uint32_t v, int i) noexcept { return ::__builtin_rotateright32(v, static_cast<unsigned char>(i)); }
    template <> inline uint64_t rotr(uint64_t v, int i) noexcept { return ::__builtin_rotateright64(v, static_cast<unsigned char>(i)); }
    template <> inline uint16_t byteswap(uint16_t v) noexcept { return ::__builtin_bswap16(v); }
    template <> inline uint32_t byteswap(uint32_t v) noexcept { return ::__builtin_bswap32(v); }
    template <> inline uint64_t byteswap(uint64_t v) noexcept { return ::__builtin_bswap64(v); }
#elif defined(__GNUC__)
    template <> inline uint8_t rotl(uint8_t v, int i) noexcept { return ::__rolb(v, i); }
    template <> inline uint16_t rotl(uint16_t v, int i) noexcept { return ::__rolw(v, i); }
    template <> inline uint32_t rotl(uint32_t v, int i) noexcept { return ::__rold(v, i); }
    template <> inline uint64_t rotl(uint64_t v, int i) noexcept { return ::__rolq(v, i); }
    template <> inline uint8_t rotr(uint8_t v, int i) noexcept { return ::__rorb(v, i); }
    template <> inline uint16_t rotr(uint16_t v, int i) noexcept { return ::__rorw(v, i); }
    template <> inline uint32_t rotr(uint32_t v, int i) noexcept { return ::__rord(v, i); }
    template <> inline uint64_t rotr(uint64_t v, int i) noexcept { return ::__rorq(v, i); }
    template <> inline uint16_t byteswap(uint16_t v) noexcept { return ::__builtin_bswap16(v); }
    template <> inline uint32_t byteswap(uint32_t v) noexcept { return ::__builtin_bswap32(v); }
    template <> inline uint64_t byteswap(uint64_t v) noexcept { return ::__builtin_bswap64(v); }
#else
    template <> inline uint8_t rotl(uint8_t v, int i) noexcept { return naive::rotl(v, i); }
    template <> inline uint16_t rotl(uint16_t v, int i) noexcept { return naive::rotl(v, i); }
    template <> inline uint32_t rotl(uint32_t v, int i) noexcept { return naive::rotl(v, i); }
    template <> inline uint64_t rotl(uint64_t v, int i) noexcept { return naive::rotl(v, i); }
    template <> inline uint8_t rotr(uint8_t v, int i) noexcept { return naive::rotr(v, i); }
    template <> inline uint16_t rotr(uint16_t v, int i) noexcept { return naive::rotr(v, i); }
    template <> inline uint32_t rotr(uint32_t v, int i) noexcept { return naive::rotr(v, i); }
    template <> inline uint64_t rotr(uint64_t v, int i) noexcept { return naive::rotr(v, i); }
    template <> inline uint16_t byteswap(uint16_t v) noexcept { return naive::byteswap(v); }
    template <> inline uint32_t byteswap(uint32_t v) noexcept { return naive::byteswap(v); }
    template <> inline uint64_t byteswap(uint64_t v) noexcept { return naive::byteswap(v); }
#endif

    template <> inline uint32_t shld(uint32_t l, uint32_t h, int i) noexcept { return naive::shld(l, h, i); }
    template <> inline uint32_t shrd(uint32_t l, uint32_t h, int i) noexcept { return naive::shrd(l, h, i); }

#if defined(_MSC_VER) && defined(_M_X64)
    template <> inline uint64_t shld(uint64_t l, uint64_t h, int i) noexcept { return __shiftleft128(l, h, static_cast<unsigned char>(i)); }
    template <> inline uint64_t shrd(uint64_t l, uint64_t h, int i) noexcept { return __shiftright128(l, h, static_cast<unsigned char>(i)); }
#else
    template <> inline uint64_t shld(uint64_t l, uint64_t h, int i) noexcept { return naive::shld(l, h, i); }
    template <> inline uint64_t shrd(uint64_t l, uint64_t h, int i) noexcept { return naive::shrd(l, h, i); }
#endif

#if defined(__SIZEOF_INT128__) // compiler has uint128
    template <> inline constexpr unsigned __int128 rotl(unsigned __int128 v, int i) noexcept { return naive::rotl(v, i); }
    template <> inline constexpr unsigned __int128 rotr(unsigned __int128 v, int i) noexcept { return naive::rotr(v, i); }
    template <> inline constexpr unsigned __int128 byteswap(unsigned __int128 v) noexcept { return  naive::byteswap(v); }
    template <> inline constexpr unsigned __int128 shld(unsigned __int128 l, unsigned __int128 h, int i) noexcept { return naive::shld(l, h, i); }
    template <> inline constexpr unsigned __int128 shrd(unsigned __int128 l, unsigned __int128 h, int i) noexcept { return naive::shrd(l, h, i); }
#endif

    // 64bit-pair operators

    static inline bool operator ==(uint64x2_t a, uint64x2_t b) noexcept { return !((a.h ^ b.h) | (a.l ^ b.l)); }
    static inline bool operator !=(uint64x2_t a, uint64x2_t b) noexcept { return !(a == b); }
    static inline bool operator <(uint64x2_t a, uint64x2_t b) noexcept { return sbb(sbb(0, a.l, b.l), a.h, b.h); }
    static inline bool operator >(uint64x2_t a, uint64x2_t b) noexcept { return b < a; }
    static inline bool operator <=(uint64x2_t a, uint64x2_t b) noexcept { return !(a > b); }
    static inline bool operator >=(uint64x2_t a, uint64x2_t b) noexcept { return !(a < b); }

    static inline uint64x2_t& operator +=(uint64x2_t& a, uint64x2_t b) noexcept { return (void)adc(adc(0, a.l, b.l), a.h, b.h), a; }
    static inline uint64x2_t& operator -=(uint64x2_t& a, uint64x2_t b) noexcept { return (void)sbb(sbb(0, a.l, b.l), a.h, b.h), a; }

    static inline uint64x2_t& operator *=(uint64x2_t& a, uint64x2_t b) noexcept
    {
        uint64x2_t r{};
        r += muld(a.l, b.l);
        r.h += a.l * b.h;
        r.h += a.h * b.l;
        return a = r;
    }

    static inline uint64x2_t operator +(uint64x2_t a, uint64x2_t b) noexcept { return a += b; }
    static inline uint64x2_t operator -(uint64x2_t a, uint64x2_t b) noexcept { return a -= b; }
    static inline uint64x2_t operator *(uint64x2_t a, uint64x2_t b) noexcept { return a *= b; }

    static inline constexpr uint64x2_t operator ~(uint64x2_t a) noexcept { return uint64x2_t{~a.l, ~a.h}; }
    static inline constexpr uint64x2_t operator &(uint64x2_t a, uint64x2_t b) noexcept { return uint64x2_t{a.l & b.l, a.h & b.h}; }
    static inline constexpr uint64x2_t operator |(uint64x2_t a, uint64x2_t b) noexcept { return uint64x2_t{a.l | b.l, a.h | b.h}; }
    static inline constexpr uint64x2_t operator ^(uint64x2_t a, uint64x2_t b) noexcept { return uint64x2_t{a.l ^ b.l, a.h ^ b.h}; }
    static inline constexpr uint64x2_t& operator &=(uint64x2_t& a, uint64x2_t b) noexcept { return a.h &= b.h, a.l &= b.l, a; }
    static inline constexpr uint64x2_t& operator |=(uint64x2_t& a, uint64x2_t b) noexcept { return a.h |= b.h, a.l |= b.l, a; }
    static inline constexpr uint64x2_t& operator ^=(uint64x2_t& a, uint64x2_t b) noexcept { return a.h ^= b.h, a.l ^= b.l, a; }

    static inline uint64x2_t operator <<(uint64x2_t a, int i) noexcept { return i & 64 ? uint64x2_t{0, a.l << (i & 63)} : uint64x2_t{a.l << (i & 63), shld(a.l, a.h, i)}; }
    static inline uint64x2_t operator >>(uint64x2_t a, int i) noexcept { return i & 64 ? uint64x2_t{a.h >> (i & 63), 0} : uint64x2_t{shrd(a.l, a.h, i), a.h >> (i & 63)}; }
    static inline uint64x2_t& operator <<=(uint64x2_t& a, int i) noexcept { return a = a << i; }
    static inline uint64x2_t& operator >>=(uint64x2_t& a, int i) noexcept { return a = a >> i; }

    template <> inline carry_flag_t adc(carry_flag_t cf, uint64x2_t& a, uint64x2_t b) noexcept { return adc(adc(cf, a.l, b.l), a.h, b.h); }
    template <> inline carry_flag_t sbb(carry_flag_t cf, uint64x2_t& a, uint64x2_t b) noexcept { return sbb(sbb(cf, a.l, b.l), a.h, b.h); }
    template <> inline auto muld(uint64x2_t a, uint64x2_t b) noexcept = delete; // is demanded?
    template <> inline uint64x2_t rotl(uint64x2_t v, int i) noexcept { return i & 64 ? uint64x2_t{shld(v.l, v.h, i), shld(v.h, v.l, i)} : uint64x2_t{shld(v.h, v.l, i), shld(v.l, v.h, i)}; }
    template <> inline uint64x2_t rotr(uint64x2_t v, int i) noexcept { return i & 64 ? uint64x2_t{shrd(v.h, v.l, i), shrd(v.l, v.h, i)} : uint64x2_t{shrd(v.l, v.h, i), shrd(v.h, v.l, i)}; }
    template <> inline uint64x2_t byteswap(uint64x2_t a) noexcept { return uint64x2_t{byteswap(a.h), byteswap(a.l)}; }
    template <> inline uint64x2_t shld(uint64x2_t l, uint64x2_t h, int i) noexcept = delete; // is demanded?
    template <> inline uint64x2_t shrd(uint64x2_t l, uint64x2_t h, int i) noexcept = delete; // is demanded?

#if __has_include("hexilit.h")
    inline namespace literals
    {
        template <char... cs>
        static inline constexpr uint64x2_t operator ""_u64x2()
        {
            using namespace hexilit::parser;
            constexpr auto arr = parse_hexint_array<uint64_t, cs...>(is_in_range<1, 32>, always, never);
            static_assert(arr.size() == 1 || arr.size() == 2);
            if constexpr (arr.size() == 1) { return uint64x2_t{arr[0]}; }
            else { return uint64x2_t{arr[1], arr[0]}; }
        }
    }
#endif
}

namespace arkana::bit
{
    using intrinsics::memcpy;
    using intrinsics::memset;
    using intrinsics::load_u;
    using intrinsics::store_u;
    using intrinsics::bit_cast;
    using intrinsics::type_punning_cast;
    using intrinsics::secure_memzero;
    using intrinsics::secure_be_zero;

    using intrinsics::rotl;
    using intrinsics::rotr;
    using intrinsics::byteswap;
    using intrinsics::shld;
    using intrinsics::shrd;
}

namespace arkintr
{
    using namespace arkana::intrinsics;
}

namespace arkbit
{
    using namespace arkana::bit;
}
