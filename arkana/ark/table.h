/// @file
/// @brief	arkana::table - LUT generating helper
/// @author Copyright(c) 2022 ttsuki
/// 
/// This software is released under the MIT License.
/// https://opensource.org/licenses/MIT

#pragma once

#include <array>
#include <type_traits>

namespace arkana
{
    template <class T, size_t Size>
    using table = std::array<T, Size>;

    template <class To, size_t Size, class Func>
    static inline constexpr table<To, Size> generate_table(Func&& func)
    {
        table<To, Size> to{};
        for (size_t i = 0; i < to.size(); i++) to[i] = func(i);
        return to;
    }

    template <class From, size_t Size, class Func>
    static inline constexpr auto transform(table<From, Size> from, Func&& func)
    -> table<std::invoke_result_t<Func, From>, Size>
    {
        using To = std::invoke_result_t<Func, From>;
        table<To, Size> to{};
        for (size_t i = 0; i < to.size(); i++) to[i] = func(from[i]);
        return to;
    }

    template <class From, size_t Size, class Func>
    static inline constexpr auto transform(table<From, Size> from, Func&& func)
    -> table<std::invoke_result_t<Func, From, size_t>, Size>
    {
        using To = std::invoke_result_t<Func, From, size_t>;
        table<To, Size> to{};
        for (size_t i = 0; i < to.size(); i++) to[i] = func(from[i], i);
        return to;
    }
}
