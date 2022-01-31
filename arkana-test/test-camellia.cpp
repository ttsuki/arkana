#include "pch.h"

#include <arkana/bits/hex-int-literals.h>
#include <arkana/camellia/camellia.h>
#include "./helper.h"

using namespace arkana;
using namespace hex_int_literals;


template <template <size_t key_bits, bool for_encryption> class TDriver, class TKey, class TValue>
static void test_vector(TKey key, TValue plain, TValue cipher)
{
    constexpr size_t key_bits = sizeof(key) * 8;

    //enc
    {
        TDriver<key_bits, true> ctx(key.data());
        TValue x{};
        ctx.process_blocks(x.data(), plain.data(), 1);
        EXPECT_EQ(x, cipher);
    }

    //dec
    {
        TDriver<key_bits, false> ctx(key.data());
        TValue x{};
        ctx.process_blocks(x.data(), cipher.data(), 1);
        EXPECT_EQ(x, plain);
    }
}

template <template <size_t key_bits, bool for_encryption> class TDriver, class TKey>
static void benchmark(TKey key)
{
#if NDEBUG
    constexpr size_t key_bits = sizeof(key) * 8;
    constexpr size_t unit_size = sizeof(typename TDriver<key_bits, true>::unit_t);

    auto& src = static_random_bytes_256m;
    auto dst = src;
    auto count = src.size() / unit_size;

    TDriver<key_bits, true> enc(key.data());
    enc.process_blocks(dst.data(), src.data(), count);

    TDriver<key_bits, false> dec(key.data());
    dec.process_blocks(dst.data(), dst.data(), count);

    EXPECT_EQ(src, dst);
#endif
}

template <size_t key_bits, bool for_encryption>
struct ecb_ia32_driver
{
    using key_t = camellia::ia32::key_t<key_bits>;
    using key_vector_t = camellia::ia32::key_vector_t<key_bits>;
    using unit_t = camellia::ia32::unit_t;

    const key_vector_t key_vector;

    explicit ecb_ia32_driver(const void* key)
        : key_vector(camellia::ia32::generate_key_vector<key_bits, for_encryption>(*static_cast<const key_t*>(key)))
    {
    }

    void process_blocks(void* dst, const void* src, size_t unit_count)
    {
        camellia::ia32::process_blocks_ecb<key_bits>(
            static_cast<unit_t*>(dst),
            static_cast<const unit_t*>(src),
            unit_count,
            key_vector);
    }
};

TEST(CAMELLIA, ia32)
{
    test_vector<ecb_ia32_driver>(
        0x01'23'45'67'89'ab'cd'ef'fe'dc'ba'98'76'54'32'10_byte_array,
        0x01'23'45'67'89'ab'cd'ef'fe'dc'ba'98'76'54'32'10_byte_array,
        0x67'67'31'38'54'96'69'73'08'57'06'56'48'ea'be'43_byte_array);

    test_vector<ecb_ia32_driver>(
        0x01'23'45'67'89'ab'cd'ef'fe'dc'ba'98'76'54'32'10'00'11'22'33'44'55'66'77_byte_array,
        0x01'23'45'67'89'ab'cd'ef'fe'dc'ba'98'76'54'32'10_byte_array,
        0xb4'99'34'01'b3'e9'96'f8'4e'e5'ce'e7'd7'9b'09'b9_byte_array);

    test_vector<ecb_ia32_driver>(
        0x01'23'45'67'89'ab'cd'ef'fe'dc'ba'98'76'54'32'10'00'11'22'33'44'55'66'77'88'99'aa'bb'cc'dd'ee'ff_byte_array,
        0x01'23'45'67'89'ab'cd'ef'fe'dc'ba'98'76'54'32'10_byte_array,
        0x9a'cc'23'7d'ff'16'd7'6c'20'ef'7c'91'9e'3a'75'09_byte_array);
}

#ifdef NDEBUG
TEST(CAMELLIA, bench_ia32_128)
{
    benchmark<ecb_ia32_driver>(0x01'23'45'67'89'ab'cd'ef'fe'dc'ba'98'76'54'32'10_byte_array);
}

TEST(CAMELLIA, bench_ia32_256)
{
    benchmark<ecb_ia32_driver>(0x01'23'45'67'89'ab'cd'ef'fe'dc'ba'98'76'54'32'10'00'11'22'33'44'55'66'77'88'99'aa'bb'cc'dd'ee'ff_byte_array);
}
#endif
