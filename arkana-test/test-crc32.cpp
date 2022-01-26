#include "pch.h"

#include <arkana/bits/hex-int-literals.h>
#include <arkana/crc32/crc32.h>
#include <arkana/crc32/crc32-ia32.h>

#include "./helper.h"

using namespace arkana;
using namespace arkana::crc32;

static inline constexpr uint32_t polynominal = 0xEDB88320;

TEST(CRC32, zero)
{
    constexpr array<byte_t, 16> zero_data{};
    constexpr array<crc32_value_t, 16> expect_zero
    {
        0x00000000, 0xD202EF8D, 0x41D912FF, 0xFF41D912,
        0x2144DF1C, 0xC622F71D, 0xB1C2A1A3, 0x9D6CDF7E,
        0x6522DF69, 0xE60914AE, 0xE38A6876, 0x6B87B1EC,
        0x7BD5C66F, 0x0F744682, 0xD1BB79C7, 0xD7D303E7,
    };

    for (size_t i = 0; i < 16; i++)
    {
        EXPECT_EQ(calculate_crc32<polynominal>(zero_data.data(), i), expect_zero[i]);
        EXPECT_EQ(ia32::calculate_crc32<polynominal>(zero_data.data(), i), expect_zero[i]);
    }
}

TEST(CRC32, eq)
{
    auto a = calculate_crc32<polynominal>(static_random_bytes_1m.data(), static_random_bytes_1m.size());
    auto b = ia32::calculate_crc32<polynominal>(static_random_bytes_1m.data(), static_random_bytes_1m.size());
    EXPECT_EQ(a, b);
}

#ifdef NDEBUG

crc32_value_t expect = calculate_crc32<polynominal>(static_random_bytes_256m.data(), static_random_bytes_256m.size());

TEST(CRC32, bench_ref)
{
    EXPECT_EQ(expect, calculate_crc32<polynominal>(static_random_bytes_256m.data(), static_random_bytes_256m.size()));
}

TEST(CRC32, bench_ia32)
{
    EXPECT_EQ(expect, ia32::calculate_crc32<polynominal>(static_random_bytes_256m.data(), static_random_bytes_256m.size()));
}
#endif
