/// @file
/// @brief	arkana::bits - memcpy
/// @author Copyright(c) 2021 ttsuki
/// 
/// This software is released under the MIT License.
/// https://opensource.org/licenses/MIT

#pragma once

#ifdef _MSC_VER
#include <intrin.h>
#endif

#include <cstddef>
#include <cstring>
#include <type_traits>

namespace arkana
{
    using std::memcpy;
    using std::memset;

    static inline void memzero(void* p, size_t size) noexcept { memset(p, 0, size); }

    static inline void secure_memzero(void* p, size_t size) noexcept
    {
#ifdef _MSC_VER
        ::__stosb(static_cast<unsigned char*>(p), 0, size);
#else
        for (size_t i = 0; i < size; i++) static_cast<volatile unsigned char*>(p)[i] = 0;
#endif
    }

    /// Loads T from unaligned memory pointer
    template <class T, std::enable_if_t<std::is_trivially_copyable_v<T>>* = nullptr>
    static inline constexpr T load_u(const void* src) noexcept
    {
        T t;
        memcpy(&t, src, sizeof(T));
        return t;
    }

    /// Stores T to unaligned memory pointer
    template <class T, std::enable_if_t<std::is_trivially_copyable_v<T>> * = nullptr>
    static inline constexpr auto store_u(void* d, const std::decay_t<T>& s) noexcept
    {
        memcpy(d, &s, sizeof(T));
    }

    /// bit_cast
    template <class To, class From,
              std::enable_if_t<
                  sizeof(To) == sizeof(From) &&
                  std::is_trivially_copyable_v<To> &&
                  std::is_trivially_copyable_v<From>>* = nullptr>
    static inline constexpr To bit_cast(const From& f) noexcept
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
    static inline constexpr To& type_punning_cast(From& f) noexcept
    {
        return reinterpret_cast<To&>(f); // UB
    }

    template <class T, std::enable_if_t<std::is_trivially_copyable_v<T>> * = nullptr>
    static inline constexpr void be_zero(T& d) noexcept
    {
        memzero(&d, sizeof(d));
    }

    template <class T, std::enable_if_t<std::is_trivially_copyable_v<T>>* = nullptr>
    static inline constexpr void secure_be_zero(T& d) noexcept
    {
        secure_memzero(&d, sizeof(d));
    }
}
