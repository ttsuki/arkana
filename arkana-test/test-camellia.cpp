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

    struct ctr_context
    {
        std::unique_ptr<ctr_context_t> ctx;

        explicit ctr_context(std::unique_ptr<ctr_context_t>&& ctx)
            : ctx(std::move(ctx))
        {
        }

        void process_bytes(void* dst, const void* src, size_t pos, size_t length) const
        {
            ctx->process_bytes(dst, src, pos, length);
        }
    };
}

template <typename T>
struct CamelliaTest : testing::Test
{
};

TYPED_TEST_CASE_P(CamelliaTest);


template <typename T>
struct CamelliaCtrTest : testing::Test
{
};

TYPED_TEST_CASE_P(CamelliaCtrTest);

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

TYPED_TEST_P(CamelliaCtrTest, test)
{
    auto test_vector = [](auto context, auto input, auto expected)
    {
        decltype(expected) buffer{};
        context.process_bytes(buffer.data(), input.data(), 0, input.size());
        EXPECT_EQ(buffer, expected);
        context.process_bytes(buffer.data(), buffer.data(), 0, buffer.size());
        EXPECT_EQ(buffer, input);
    };

    {
        static constexpr key_128bit_t key = 0xAE'68'52'F8'12'10'67'CC'4B'F7'A5'76'55'77'F3'9E_byte_array;
        static constexpr ctr_iv_t iv = 0x00'00'00'00'00'00'00'00_byte_array;
        static constexpr ctr_nonce_t nonce = 0x00'00'00'30_byte_array;
        static constexpr auto plain = 0x53'69'6E'67'6C'65'20'62'6C'6F'63'6B'20'6D'73'67_byte_array;
        static constexpr auto cipher = 0xD0'9D'C2'9A'82'14'61'9A'20'87'7C'76'DB'1F'0B'3F_byte_array;
        test_vector(TypeParam::camellia128_ctr_context_t(&key, &iv, &nonce), plain, cipher);
    }

    {
        static constexpr key_128bit_t key = 0x7E'24'06'78'17'FA'E0'D7'43'D6'CE'1F'32'53'91'63_byte_array;
        static constexpr ctr_iv_t iv = 0xC0'54'3B'59'DA'48'D9'0B_byte_array;
        static constexpr ctr_nonce_t nonce = 0x00'6C'B6'DB_byte_array;
        static constexpr auto plain = 0x00'01'02'03'04'05'06'07'08'09'0A'0B'0C'0D'0E'0F'10'11'12'13'14'15'16'17'18'19'1A'1B'1C'1D'1E'1F_byte_array;
        static constexpr auto cipher = 0xDB'F3'C7'8D'C0'83'96'D4'DA'7C'90'77'65'BB'CB'44'2B'8E'8E'0F'31'F0'DC'A7'2C'74'17'E3'53'60'E0'48_byte_array;
        test_vector(TypeParam::camellia128_ctr_context_t(&key, &iv, &nonce), plain, cipher);
    }

    {
        static constexpr key_128bit_t key = 0x76'91'BE'03'5E'50'20'A8'AC'6E'61'85'29'F9'A0'DC_byte_array;
        static constexpr ctr_iv_t iv = 0x27'77'7F'3F'4A'17'86'F0_byte_array;
        static constexpr ctr_nonce_t nonce = 0x00'E0'01'7B_byte_array;
        static constexpr auto plain = 0x00'01'02'03'04'05'06'07'08'09'0A'0B'0C'0D'0E'0F'10'11'12'13'14'15'16'17'18'19'1A'1B'1C'1D'1E'1F'20'21'22'23_byte_array;
        static constexpr auto cipher = 0xB1'9D'1F'CD'CB'75'EB'88'2F'84'9C'E2'4D'85'CF'73'9C'E6'4B'2B'5C'9D'73'F1'4F'2D'5D'9D'CE'98'89'CD'DF'50'86'96_byte_array;
        test_vector(TypeParam::camellia128_ctr_context_t(&key, &iv, &nonce), plain, cipher);
    }

    {
        static constexpr key_192bit_t key = 0x16'AF'5B'14'5F'C9'F5'79'C1'75'F9'3E'3B'FB'0E'ED'86'3D'06'CC'FD'B7'85'15_byte_array;
        static constexpr ctr_iv_t iv = 0x36'73'3C'14'7D'6D'93'CB_byte_array;
        static constexpr ctr_nonce_t nonce = 0x00'00'00'48_byte_array;
        static constexpr auto plain = 0x53'69'6E'67'6C'65'20'62'6C'6F'63'6B'20'6D'73'67_byte_array;
        static constexpr auto cipher = 0x23'79'39'9E'8A'8D'2B'2B'16'70'2F'C7'8B'9E'96'96_byte_array;
        test_vector(TypeParam::camellia192_ctr_context_t(&key, &iv, &nonce), plain, cipher);
    }

    {
        static constexpr key_192bit_t key = 0x7C'5C'B2'40'1B'3D'C3'3C'19'E7'34'08'19'E0'F6'9C'67'8C'3D'B8'E6'F6'A9'1A_byte_array;
        static constexpr ctr_iv_t iv = 0x02'0C'6E'AD'C2'CB'50'0D_byte_array;
        static constexpr ctr_nonce_t nonce = 0x00'96'B0'3B_byte_array;
        static constexpr auto plain = 0x00'01'02'03'04'05'06'07'08'09'0A'0B'0C'0D'0E'0F'10'11'12'13'14'15'16'17'18'19'1A'1B'1C'1D'1E'1F_byte_array;
        static constexpr auto cipher = 0x7D'EF'34'F7'A5'D0'E4'15'67'4B'7F'FC'AE'67'C7'5D'D0'18'B8'6F'F2'30'51'E0'56'39'2A'99'F3'5A'4C'ED_byte_array;
        test_vector(TypeParam::camellia192_ctr_context_t(&key, &iv, &nonce), plain, cipher);
    }

    {
        static constexpr key_192bit_t key = 0x02'BF'39'1E'E8'EC'B1'59'B9'59'61'7B'09'65'27'9B'F5'9B'60'A7'86'D3'E0'FE_byte_array;
        static constexpr ctr_iv_t iv = 0x5C'BD'60'27'8D'CC'09'12_byte_array;
        static constexpr ctr_nonce_t nonce = 0x00'07'BD'FD_byte_array;
        static constexpr auto plain = 0x00'01'02'03'04'05'06'07'08'09'0A'0B'0C'0D'0E'0F'10'11'12'13'14'15'16'17'18'19'1A'1B'1C'1D'1E'1F'20'21'22'23_byte_array;
        static constexpr auto cipher = 0x57'10'E5'56'E1'48'7A'20'B5'AC'0E'73'F1'9E'4E'78'76'F3'7F'DC'91'B1'EF'4D'4D'AD'E8'E6'66'A6'4D'0E'D5'57'AB'57_byte_array;
        test_vector(TypeParam::camellia192_ctr_context_t(&key, &iv, &nonce), plain, cipher);
    }

    {
        static constexpr key_256bit_t key = 0x77'6B'EF'F2'85'1D'B0'6F'4C'8A'05'42'C8'69'6F'6C'6A'81'AF'1E'EC'96'B4'D3'7F'C1'D6'89'E6'C1'C1'04_byte_array;
        static constexpr ctr_iv_t iv = 0xDB'56'72'C9'7A'A8'F0'B2_byte_array;
        static constexpr ctr_nonce_t nonce = 0x00'00'00'60_byte_array;
        static constexpr auto plain = 0x53'69'6E'67'6C'65'20'62'6C'6F'63'6B'20'6D'73'67_byte_array;
        static constexpr auto cipher = 0x34'01'F9'C8'24'7E'FF'CE'BD'69'94'71'4C'1B'BB'11_byte_array;
        test_vector(TypeParam::camellia256_ctr_context_t(&key, &iv, &nonce), plain, cipher);
    }

    {
        static constexpr key_256bit_t key = 0xF6'D6'6D'6B'D5'2D'59'BB'07'96'36'58'79'EF'F8'86'C6'6D'D5'1A'5B'6A'99'74'4B'50'59'0C'87'A2'38'84_byte_array;
        static constexpr ctr_iv_t iv = 0xC1'58'5E'F1'5A'43'D8'75_byte_array;
        static constexpr ctr_nonce_t nonce = 0x00'FA'AC'24_byte_array;
        static constexpr auto plain = 0x00'01'02'03'04'05'06'07'08'09'0A'0B'0C'0D'0E'0F'10'11'12'13'14'15'16'17'18'19'1A'1B'1C'1D'1E'1F_byte_array;
        static constexpr auto cipher = 0xD6'C3'03'92'24'6F'78'08'A8'3C'2B'22'A8'83'9E'45'E5'1C'D4'8A'1C'DF'40'6E'BC'9C'C2'D3'AB'83'41'08_byte_array;
        test_vector(TypeParam::camellia256_ctr_context_t(&key, &iv, &nonce), plain, cipher);
    }

    {
        static constexpr key_256bit_t key = 0xFF'7A'61'7C'E6'91'48'E4'F1'72'6E'2F'43'58'1D'E2'AA'62'D9'F8'05'53'2E'DF'F1'EE'D6'87'FB'54'15'3D_byte_array;
        static constexpr ctr_iv_t iv = 0x51'A5'1D'70'A1'C1'11'48_byte_array;
        static constexpr ctr_nonce_t nonce = 0x00'1C'C5'B7_byte_array;
        static constexpr auto plain = 0x00'01'02'03'04'05'06'07'08'09'0A'0B'0C'0D'0E'0F'10'11'12'13'14'15'16'17'18'19'1A'1B'1C'1D'1E'1F'20'21'22'23_byte_array;
        static constexpr auto cipher = 0xA4'DA'23'FC'E6'A5'FF'AA'6D'64'AE'9A'06'52'A4'2C'D1'61'A3'4B'65'F9'67'9F'75'C0'1F'10'1F'71'27'6F'15'EF'0D'8D_byte_array;
        test_vector(TypeParam::camellia256_ctr_context_t(&key, &iv, &nonce), plain, cipher);
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
REGISTER_TYPED_TEST_CASE_P(CamelliaCtrTest, test);

struct ia32_ecb_contexts
{
    static auto camellia128_encrypt_context_t(const key_128bit_t* key) { return ecb_context(create_ecb_encrypt_context_ia32(key)); }
    static auto camellia192_encrypt_context_t(const key_192bit_t* key) { return ecb_context(create_ecb_encrypt_context_ia32(key)); }
    static auto camellia256_encrypt_context_t(const key_256bit_t* key) { return ecb_context(create_ecb_encrypt_context_ia32(key)); }
    static auto camellia128_decrypt_context_t(const key_128bit_t* key) { return ecb_context(create_ecb_decrypt_context_ia32(key)); }
    static auto camellia192_decrypt_context_t(const key_192bit_t* key) { return ecb_context(create_ecb_decrypt_context_ia32(key)); }
    static auto camellia256_decrypt_context_t(const key_256bit_t* key) { return ecb_context(create_ecb_decrypt_context_ia32(key)); }
    static auto camellia128_ctr_context_t(const key_128bit_t* key, const ctr_iv_t* iv, const ctr_nonce_t* nonce) { return ctr_context(create_ctr_context_ia32(key, iv, nonce)); }
    static auto camellia192_ctr_context_t(const key_192bit_t* key, const ctr_iv_t* iv, const ctr_nonce_t* nonce) { return ctr_context(create_ctr_context_ia32(key, iv, nonce)); }
    static auto camellia256_ctr_context_t(const key_256bit_t* key, const ctr_iv_t* iv, const ctr_nonce_t* nonce) { return ctr_context(create_ctr_context_ia32(key, iv, nonce)); }
};

INSTANTIATE_TYPED_TEST_CASE_P(ia32, CamelliaTest, ia32_ecb_contexts);
INSTANTIATE_TYPED_TEST_CASE_P(ia32, CamelliaCtrTest, ia32_ecb_contexts);

struct avx2_ecb_contexts
{
    static auto camellia128_encrypt_context_t(const key_128bit_t* key) { return ecb_context(create_ecb_encrypt_context_avx2(key)); }
    static auto camellia192_encrypt_context_t(const key_192bit_t* key) { return ecb_context(create_ecb_encrypt_context_avx2(key)); }
    static auto camellia256_encrypt_context_t(const key_256bit_t* key) { return ecb_context(create_ecb_encrypt_context_avx2(key)); }
    static auto camellia128_decrypt_context_t(const key_128bit_t* key) { return ecb_context(create_ecb_decrypt_context_avx2(key)); }
    static auto camellia192_decrypt_context_t(const key_192bit_t* key) { return ecb_context(create_ecb_decrypt_context_avx2(key)); }
    static auto camellia256_decrypt_context_t(const key_256bit_t* key) { return ecb_context(create_ecb_decrypt_context_avx2(key)); }
};

INSTANTIATE_TYPED_TEST_CASE_P(avx2, CamelliaTest, avx2_ecb_contexts);

struct avx2aesni_ecb_contexts
{
    static auto camellia128_encrypt_context_t(const key_128bit_t* key) { return ecb_context(create_ecb_encrypt_context_avx2aesni(key)); }
    static auto camellia192_encrypt_context_t(const key_192bit_t* key) { return ecb_context(create_ecb_encrypt_context_avx2aesni(key)); }
    static auto camellia256_encrypt_context_t(const key_256bit_t* key) { return ecb_context(create_ecb_encrypt_context_avx2aesni(key)); }
    static auto camellia128_decrypt_context_t(const key_128bit_t* key) { return ecb_context(create_ecb_decrypt_context_avx2aesni(key)); }
    static auto camellia192_decrypt_context_t(const key_192bit_t* key) { return ecb_context(create_ecb_decrypt_context_avx2aesni(key)); }
    static auto camellia256_decrypt_context_t(const key_256bit_t* key) { return ecb_context(create_ecb_decrypt_context_avx2aesni(key)); }
};

INSTANTIATE_TYPED_TEST_CASE_P(avx2aesni, CamelliaTest, avx2aesni_ecb_contexts);
