#pragma once

#include <cstddef>
#include <vector>
#include <random>

static inline std::vector<std::byte> random_bytes(size_t len)
{
    std::vector<std::byte> plain(len);
    std::independent_bits_engine<std::default_random_engine, 8, uint32_t> e{};
    std::generate(begin(plain), end(plain), [gen = std::ref(e)]
    {
        return static_cast<std::byte>(gen());
    });
    return plain;
}

static auto inline static_random_bytes_1k = random_bytes(1024);
static auto inline static_random_bytes_1m = random_bytes(1048576);

#ifdef NDEBUG
static auto inline static_random_bytes_256m = random_bytes(268435456);
#endif
