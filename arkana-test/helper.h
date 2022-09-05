#pragma once

#include <cstddef>
#include <vector>
#include <random>

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

static auto inline static_random_bytes_1k = random_bytes(1024);
static auto inline static_random_bytes_1m = random_bytes(1048576);

#ifdef NDEBUG
static auto inline static_random_bytes_256m = random_bytes(268435456);
#endif
