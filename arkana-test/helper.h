#pragma once

#include <cstddef>
#include <vector>
#include <random>
#include <algorithm>

static inline std::vector<std::byte> random_bytes(size_t length)
{
    std::vector<std::byte> plain(length + 7 & ~7);
    std::independent_bits_engine<std::default_random_engine, 64, uint64_t> generator{};
    std::generate(
        reinterpret_cast<uint64_t*>(plain.data()),
        reinterpret_cast<uint64_t*>(plain.data() + plain.size() / sizeof(uint64_t)),
        [&generator]() -> uint64_t { return generator(); });
    plain.resize(length);
    return plain;
}

static inline const std::vector<std::byte>& static_random_bytes_1k()
{
    static auto random_bytes_1k = random_bytes(1024);
    return random_bytes_1k;
}

static inline const std::vector<std::byte>& static_random_bytes_1m()
{
    static auto random_bytes_1m = random_bytes(1048576);
    return random_bytes_1m;
}

#ifdef NDEBUG
static inline const std::vector<std::byte>& static_random_bytes_256m()
{
    static auto random_bytes_256m = random_bytes(268435456);
    return random_bytes_256m;
}
#endif
