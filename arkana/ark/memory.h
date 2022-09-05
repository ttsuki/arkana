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
    template <class T>
    static inline constexpr auto load_u(const void* src) noexcept
    -> std::enable_if_t<std::is_trivially_copyable_v<T>, T>
    {
        T t;
        memcpy(&t, src, sizeof(T));
        return t;
    }

    /// Stores T to unaligned memory pointer
    template <class T>
    static inline constexpr auto store_u(void* d, const std::decay_t<T>& s) noexcept
    -> std::enable_if_t<std::is_trivially_copyable_v<T>, void>
    {
        memcpy(d, &s, sizeof(T));
    }

    /// bit_cast
    template <class To, class From>
    static inline constexpr auto bit_cast(const From& f) noexcept
    -> std::enable_if_t<
        sizeof(To) == sizeof(From) &&
        std::is_trivially_copyable_v<To> &&
        std::is_trivially_copyable_v<From>, To>
    {
        return load_u<To>(&f);
    }

    template <class T>
    static inline constexpr auto be_zero(T& d) noexcept
    -> std::enable_if_t<std::is_trivially_copyable_v<T>, void>
    {
        memzero(&d, sizeof(d));
    }

    template <class T>
    static inline constexpr auto secure_be_zero(T& d) noexcept
        -> std::enable_if_t<std::is_trivially_copyable_v<T>, void>
    {
        secure_memzero(&d, sizeof(d));
    }
}
