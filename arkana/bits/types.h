#pragma once

#include <cstddef>
#include <cstdint>
#include <array>

namespace arkana
{
    using byte_t = std::byte;
    using uint8_t = std::uint8_t;
    using uint16_t = std::uint16_t;
    using uint32_t = std::uint32_t;
    using uint64_t = std::uint64_t;
    using size_t = std::size_t;
    using uintmax_t = std::uintmax_t;
    template <class T, size_t S> using array = std::array<T, S>;
    template <size_t S> using byte_array = array<byte_t, S>;
}
