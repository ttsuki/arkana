#include "pch.h"

#include <arkana/bits/hex-int-literals.h>
#include <arkana/camellia/camellia.h>
#include <arkana/camellia/camellia-avx2.h>
#include "./helper.h"

using namespace arkana;
using namespace camellia;
using namespace hex_int_literals;

namespace
{
    template <class t_key_t, class t_unit_t, auto t_generate_key_vector, auto t_process_blocks>
    struct ecb_context
    {
        using key_t = t_key_t;
        using key_vector_t = decltype(t_generate_key_vector(std::declval<key_t>()));
        using unit_t = t_unit_t;

        const key_vector_t key_vector;

        explicit ecb_context(const key_t* key)
            : key_vector(t_generate_key_vector(*key))
        {
        }

        void process_blocks(void* dst, const void* src, size_t length) const
        {
            if (length % 16 != 0)
                throw std::invalid_argument("invalid length.");

            const size_t unit_count = length / sizeof(unit_t);
            t_process_blocks(static_cast<unit_t*>(dst), static_cast<const unit_t*>(src), unit_count, key_vector);

            if (const size_t remain_bytes = length % sizeof(unit_t))
            {
                unit_t buf{};
                static_assert(std::is_trivial_v<unit_t>);
                memcpy(&buf, static_cast<const byte_t*>(src) + unit_count * sizeof(unit_t), remain_bytes);
                t_process_blocks(&buf, &buf, 1, key_vector);
                memcpy(static_cast<byte_t*>(dst) + unit_count * sizeof(unit_t), &buf, remain_bytes);
            }
        }
    };
}

template <typename T>
struct CamelliaTest : testing::Test
{
};

TYPED_TEST_CASE_P(CamelliaTest);

TYPED_TEST_P(CamelliaTest, test)
{
    auto test_vector = [](auto context, auto input, auto expected)
    {
        decltype(expected) buffer{};
        context.process_blocks(buffer.data(), input.data(), input.size());
        EXPECT_EQ(buffer, expected);
    };

    // rfc3713 test vector
    {
        static constexpr key_128bit_t key = 0x01'23'45'67'89'ab'cd'ef'fe'dc'ba'98'76'54'32'10_byte_array;
        static constexpr byte_array<16> plain = 0x01'23'45'67'89'ab'cd'ef'fe'dc'ba'98'76'54'32'10_byte_array;
        static constexpr byte_array<16> cipher = 0x67'67'31'38'54'96'69'73'08'57'06'56'48'ea'be'43_byte_array;
        test_vector(TypeParam::camellia128_encrypt_context_t(&key), plain, cipher);
        test_vector(TypeParam::camellia128_decrypt_context_t(&key), cipher, plain);
    }

    // rfc3713 test vector
    {
        static constexpr key_192bit_t key = 0x01'23'45'67'89'ab'cd'ef'fe'dc'ba'98'76'54'32'10'00'11'22'33'44'55'66'77_byte_array;
        static constexpr byte_array<16> plain = 0x01'23'45'67'89'ab'cd'ef'fe'dc'ba'98'76'54'32'10_byte_array;
        static constexpr byte_array<16> cipher = 0xb4'99'34'01'b3'e9'96'f8'4e'e5'ce'e7'd7'9b'09'b9_byte_array;
        test_vector(TypeParam::camellia192_encrypt_context_t(&key), plain, cipher);
        test_vector(TypeParam::camellia192_decrypt_context_t(&key), cipher, plain);
    }

    // rfc3713 test vector
    {
        static constexpr key_256bit_t key = 0x01'23'45'67'89'ab'cd'ef'fe'dc'ba'98'76'54'32'10'00'11'22'33'44'55'66'77'88'99'aa'bb'cc'dd'ee'ff_byte_array;
        static constexpr byte_array<16> plain = 0x01'23'45'67'89'ab'cd'ef'fe'dc'ba'98'76'54'32'10_byte_array;
        static constexpr byte_array<16> cipher = 0x9a'cc'23'7d'ff'16'd7'6c'20'ef'7c'91'9e'3a'75'09_byte_array;
        test_vector(TypeParam::camellia256_encrypt_context_t(&key), plain, cipher);
        test_vector(TypeParam::camellia256_decrypt_context_t(&key), cipher, plain);
    }
}

TYPED_TEST_P(CamelliaTest, benchmark128)
{
    auto key = 0x01'23'45'67'89'ab'cd'ef'fe'dc'ba'98'76'54'32'10_byte_array;
#ifdef NDEBUG
    auto& source = static_random_bytes_256m;
#else
    auto& source = static_random_bytes_1m;
#endif
    auto buffer = source;
    TypeParam::camellia128_encrypt_context_t(&key).process_blocks(buffer.data(), buffer.data(), buffer.size());
    TypeParam::camellia128_decrypt_context_t(&key).process_blocks(buffer.data(), buffer.data(), buffer.size());
    EXPECT_EQ(source, buffer);
}

TYPED_TEST_P(CamelliaTest, benchmark256)
{
    auto key = 0x01'23'45'67'89'ab'cd'ef'fe'dc'ba'98'76'54'32'10'00'11'22'33'44'55'66'77'88'99'aa'bb'cc'dd'ee'ff_byte_array;
#ifdef NDEBUG
    auto& source = static_random_bytes_256m;
#else
    auto& source = static_random_bytes_1m;
#endif
    auto buffer = source;
    TypeParam::camellia256_encrypt_context_t(&key).process_blocks(buffer.data(), buffer.data(), buffer.size());
    TypeParam::camellia256_decrypt_context_t(&key).process_blocks(buffer.data(), buffer.data(), buffer.size());
    EXPECT_EQ(source, buffer);
}

REGISTER_TYPED_TEST_CASE_P(CamelliaTest, test, benchmark128, benchmark256);

struct ia32_ecb_contexts
{
    using camellia128_encrypt_context_t = ecb_context<key_128bit_t, ia32::unit_t, ia32::generate_key_vector<128, true>, ia32::process_blocks_ecb<128>>;
    using camellia192_encrypt_context_t = ecb_context<key_192bit_t, ia32::unit_t, ia32::generate_key_vector<192, true>, ia32::process_blocks_ecb<192>>;
    using camellia256_encrypt_context_t = ecb_context<key_256bit_t, ia32::unit_t, ia32::generate_key_vector<256, true>, ia32::process_blocks_ecb<256>>;
    using camellia128_decrypt_context_t = ecb_context<key_128bit_t, ia32::unit_t, ia32::generate_key_vector<128, false>, ia32::process_blocks_ecb<128>>;
    using camellia192_decrypt_context_t = ecb_context<key_192bit_t, ia32::unit_t, ia32::generate_key_vector<192, false>, ia32::process_blocks_ecb<192>>;
    using camellia256_decrypt_context_t = ecb_context<key_256bit_t, ia32::unit_t, ia32::generate_key_vector<256, false>, ia32::process_blocks_ecb<256>>;
};

INSTANTIATE_TYPED_TEST_CASE_P(ia32, CamelliaTest, ia32_ecb_contexts);

struct avx2_ecb_contexts
{
    using camellia128_encrypt_context_t = ecb_context<key_128bit_t, avx2::unit_t, avx2::generate_key_vector<128, true>, avx2::process_blocks_ecb<128>>;
    using camellia192_encrypt_context_t = ecb_context<key_192bit_t, avx2::unit_t, avx2::generate_key_vector<192, true>, avx2::process_blocks_ecb<192>>;
    using camellia256_encrypt_context_t = ecb_context<key_256bit_t, avx2::unit_t, avx2::generate_key_vector<256, true>, avx2::process_blocks_ecb<256>>;
    using camellia128_decrypt_context_t = ecb_context<key_128bit_t, avx2::unit_t, avx2::generate_key_vector<128, false>, avx2::process_blocks_ecb<128>>;
    using camellia192_decrypt_context_t = ecb_context<key_192bit_t, avx2::unit_t, avx2::generate_key_vector<192, false>, avx2::process_blocks_ecb<192>>;
    using camellia256_decrypt_context_t = ecb_context<key_256bit_t, avx2::unit_t, avx2::generate_key_vector<256, false>, avx2::process_blocks_ecb<256>>;
};

INSTANTIATE_TYPED_TEST_CASE_P(avx2, CamelliaTest, avx2_ecb_contexts);
