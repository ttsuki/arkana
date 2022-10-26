#pragma once

#include <cstddef>
#include <cstdint>
#include <array>

namespace arkana
{
    using byte_t = std::byte;
    using std::uint8_t;
    using std::uint16_t;
    using std::uint32_t;
    using std::uint64_t;
    using std::size_t;
    using std::uintmax_t;

    using std::array;
    using std::tuple;

    template <bool b> using bool_constant_t = std::bool_constant<b>;
    using true_t = bool_constant_t<true>;
    using false_t = bool_constant_t<false>;

    template <size_t S> using byte_array = array<byte_t, S>;

    template <class T, class... Ts> using is_any_of = std::disjunction<std::is_same<T, Ts>...>;
    template <class T, class... Ts> static constexpr bool is_any_of_v = is_any_of<T, Ts...>::value;

    /// tagged_memory_buffer
    template <class tag, size_t size, class alignment = void*>
    struct alignas(alignment) tagged_memory_buffer final
    {
        using tag_t = tag;
        byte_array<size> buffer;
    };
}
