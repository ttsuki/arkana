#include "./gtest.h"
#include "../arkana/bits.h"
#include "../arkana/camellia.h"
#include "./helper.h"

using namespace arkana::hex_int_literals;
using namespace arkana::camellia;
using arkana::byte_array;

namespace
{
    struct ecb_context
    {
        std::unique_ptr<ecb_context_t> ctx;

        explicit ecb_context(std::unique_ptr<ecb_context_t>&& ctx)
            : ctx(std::move(ctx))
        {
        }

        void process_blocks(void* dst, const void* src, size_t length) const
        {
            ctx->process_blocks(dst, src, length);
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
    static auto camellia128_encrypt_context_t(const key_128bit_t* key) { return ecb_context(create_ecb_context_ia32(key, true)); }
    static auto camellia192_encrypt_context_t(const key_192bit_t* key) { return ecb_context(create_ecb_context_ia32(key, true)); }
    static auto camellia256_encrypt_context_t(const key_256bit_t* key) { return ecb_context(create_ecb_context_ia32(key, true)); }
    static auto camellia128_decrypt_context_t(const key_128bit_t* key) { return ecb_context(create_ecb_context_ia32(key, false)); }
    static auto camellia192_decrypt_context_t(const key_192bit_t* key) { return ecb_context(create_ecb_context_ia32(key, false)); }
    static auto camellia256_decrypt_context_t(const key_256bit_t* key) { return ecb_context(create_ecb_context_ia32(key, false)); }
};

INSTANTIATE_TYPED_TEST_CASE_P(ia32, CamelliaTest, ia32_ecb_contexts);

struct avx2_ecb_contexts
{
    static auto camellia128_encrypt_context_t(const key_128bit_t* key) { return ecb_context(create_ecb_context_avx2(key, true)); }
    static auto camellia192_encrypt_context_t(const key_192bit_t* key) { return ecb_context(create_ecb_context_avx2(key, true)); }
    static auto camellia256_encrypt_context_t(const key_256bit_t* key) { return ecb_context(create_ecb_context_avx2(key, true)); }
    static auto camellia128_decrypt_context_t(const key_128bit_t* key) { return ecb_context(create_ecb_context_avx2(key, false)); }
    static auto camellia192_decrypt_context_t(const key_192bit_t* key) { return ecb_context(create_ecb_context_avx2(key, false)); }
    static auto camellia256_decrypt_context_t(const key_256bit_t* key) { return ecb_context(create_ecb_context_avx2(key, false)); }
};

INSTANTIATE_TYPED_TEST_CASE_P(avx2, CamelliaTest, avx2_ecb_contexts);

struct avx2aesni_ecb_contexts
{
    static auto camellia128_encrypt_context_t(const key_128bit_t* key) { return ecb_context(create_ecb_context_avx2aesni(key, true)); }
    static auto camellia192_encrypt_context_t(const key_192bit_t* key) { return ecb_context(create_ecb_context_avx2aesni(key, true)); }
    static auto camellia256_encrypt_context_t(const key_256bit_t* key) { return ecb_context(create_ecb_context_avx2aesni(key, true)); }
    static auto camellia128_decrypt_context_t(const key_128bit_t* key) { return ecb_context(create_ecb_context_avx2aesni(key, false)); }
    static auto camellia192_decrypt_context_t(const key_192bit_t* key) { return ecb_context(create_ecb_context_avx2aesni(key, false)); }
    static auto camellia256_decrypt_context_t(const key_256bit_t* key) { return ecb_context(create_ecb_context_avx2aesni(key, false)); }
};

INSTANTIATE_TYPED_TEST_CASE_P(avx2aesni, CamelliaTest, avx2aesni_ecb_contexts);
