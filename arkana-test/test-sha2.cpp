#include "./gtest.h"
#include "../arkana/bits.h"
#include "../arkana/sha2.h"

using namespace arkana::hex_int_literals;
using namespace arkana::sha2;


template <typename T>
struct Sha2Test : testing::Test
{
#ifndef NDEBUG
    static const auto& source_for_benchmark() { return static_random_bytes_1m; }
#else
    static const auto& source_for_benchmark() { return static_random_bytes_256m; }
#endif
};

TYPED_TEST_CASE_P(Sha2Test);

namespace
{
    template <class context_t>
    class sha2_context
    {
    public:
        context_t ctx;

        sha2_context(context_t ctx) : ctx(std::move(ctx))
        {
        }

        sha2_context& process_bytes(const void* data, size_t len) noexcept { return ctx->process_bytes(data, len), *this; }
        sha2_context& process_string(std::string_view text) noexcept { return process_bytes(text.data(), text.length()); }
        auto finalize() noexcept { return ctx->finalize(); }
    };
}

TYPED_TEST_P(Sha2Test, Sha224_TestVectors)
{
    EXPECT_EQ(TypeParam::sha224_context().finalize(), 0xD14A028C'2A3A2BC9'476102BB'288234C4'15A2B01F'828EA62A'C5B3E42F_byte_array);
    EXPECT_EQ(TypeParam::sha224_context().process_string("abc").finalize(), 0x23097D22'3405D822'8642A477'BDA255B3'2AADBCE4'BDA0B3F7'E36C9DA7_byte_array);
    EXPECT_EQ(TypeParam::sha224_context().process_string("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq").finalize(), 0x75388B16'512776CC'5DBA5DA1'FD890150'B0C6455C'B4F58B19'52522525_byte_array);
    EXPECT_EQ(TypeParam::sha224_context().process_string("abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu").finalize(), 0xC97CA9A5'59850CE9'7A04A96D'EF6D99A9'E0E0E2AB'14E6B8DF'265FC0B3_byte_array);
    EXPECT_EQ(TypeParam::sha224_context().process_string(std::string(1000000, 'a')).finalize(), 0x20794655'980C91D8'BBB4C1EA'97618A4B'F03F4258'1948B2EE'4EE7AD67_byte_array);

#ifdef NDEBUG
    {
        auto ctx = TypeParam::sha224_context();
        std::string s = "abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmno";
        for (int i = 0; i < 16777216; i++) ctx.process_string(s);
        EXPECT_EQ(ctx.finalize(), 0xB5989713'CA4FE47A'009F8621'980B34E6'D63ED306'3B2A0A2C'867D8A85_byte_array);
    }
#endif
}

TYPED_TEST_P(Sha2Test, Sha256_TestVectors)
{
    EXPECT_EQ(TypeParam::sha256_context().finalize(), 0xE3B0C442'98FC1C14'9AFBF4C8'996FB924'27AE41E4'649B934C'A495991B'7852B855_byte_array);
    EXPECT_EQ(TypeParam::sha256_context().process_string("abc").finalize(), 0xBA7816BF'8F01CFEA'414140DE'5DAE2223'B00361A3'96177A9C'B410FF61'F20015AD_byte_array);
    EXPECT_EQ(TypeParam::sha256_context().process_string("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq").finalize(), 0x248D6A61'D20638B8'E5C02693'0C3E6039'A33CE459'64FF2167'F6ECEDD4'19DB06C1_byte_array);
    EXPECT_EQ(TypeParam::sha256_context().process_string("abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu").finalize(), 0xCF5B16A7'78AF8380'036CE59E'7B049237'0B249B11'E8F07A51'AFAC4503'7AFEE9D1_byte_array);
    EXPECT_EQ(TypeParam::sha256_context().process_string(std::string(1000000, 'a')).finalize(), 0xCDC76E5C'9914FB92'81A1C7E2'84D73E67'F1809A48'A497200E'046D39CC'C7112CD0_byte_array);

#ifdef NDEBUG
    {
        auto ctx = TypeParam::sha256_context();
        std::string s = "abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmno";
        for (int i = 0; i < 16777216; i++) ctx.process_string(s);
        EXPECT_EQ(ctx.finalize(), 0x50E72A0E'26442FE2'552DC393'8AC58658'228C0CBF'B1D2CA87'2AE43526'6FCD055E_byte_array);
    }
#endif
}

TYPED_TEST_P(Sha2Test, Sha384_TestVectors)
{
    EXPECT_EQ(TypeParam::sha384_context().finalize(), 0x38B060A751AC9638'4CD9327EB1B1E36A'21FDB71114BE0743'4C0CC7BF63F6E1DA'274EDEBFE76F65FB'D51AD2F14898B95B_byte_array);
    EXPECT_EQ(TypeParam::sha384_context().process_string("abc").finalize(), 0xCB00753F45A35E8B'B5A03D699AC65007'272C32AB0EDED163'1A8B605A43FF5BED'8086072BA1E7CC23'58BAECA134C825A7_byte_array);
    EXPECT_EQ(TypeParam::sha384_context().process_string("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq").finalize(), 0x3391FDDDFC8DC739'3707A65B1B470939'7CF8B1D162AF05AB'FE8F450DE5F36BC6'B0455A8520BC4E6F'5FE95B1FE3C8452B_byte_array);
    EXPECT_EQ(TypeParam::sha384_context().process_string("abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu").finalize(), 0x09330C33F71147E8'3D192FC782CD1B47'53111B173B3B05D2'2FA08086E3B0F712'FCC7C71A557E2DB9'66C3E9FA91746039_byte_array);
    EXPECT_EQ(TypeParam::sha384_context().process_string(std::string(1000000, 'a')).finalize(), 0x9D0E1809716474CB'086E834E310A4A1C'ED149E9C00F24852'7972CEC5704C2A5B'07B8B3DC38ECC4EB'AE97DDD87F3D8985_byte_array);

#ifdef NDEBUG
    {
        auto ctx = TypeParam::sha384_context();
        std::string s = "abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmno";
        for (int i = 0; i < 16777216; i++) ctx.process_string(s);
        EXPECT_EQ(ctx.finalize(), 0x5441235CC0235341'ED806A64FB354742'B5E5C02A3C5CB71B'5F63FB793458D8FD'AE599C8CD8884943'C04F11B31B89F023_byte_array);
    }
#endif
}

TYPED_TEST_P(Sha2Test, Sha512_TestVectors)
{
    EXPECT_EQ(TypeParam::sha512_context().finalize(), 0xCF83E1357EEFB8BD'F1542850D66D8007'D620E4050B5715DC'83F4A921D36CE9CE'47D0D13C5D85F2B0'FF8318D2877EEC2F'63B931BD47417A81'A538327AF927DA3E_byte_array);
    EXPECT_EQ(TypeParam::sha512_context().process_string("abc").finalize(), 0xDDAF35A193617ABA'CC417349AE204131'12E6FA4E89A97EA2'0A9EEEE64B55D39A'2192992A274FC1A8'36BA3C23A3FEEBBD'454D4423643CE80E'2A9AC94FA54CA49F_byte_array);
    EXPECT_EQ(TypeParam::sha512_context().process_string("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq").finalize(), 0x204A8FC6DDA82F0A'0CED7BEB8E08A416'57C16EF468B228A8'279BE331A703C335'96FD15C13B1B07F9'AA1D3BEA57789CA0'31AD85C7A71DD703'54EC631238CA3445_byte_array);
    EXPECT_EQ(TypeParam::sha512_context().process_string("abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu").finalize(), 0x8E959B75DAE313DA'8CF4F72814FC143F'8F7779C6EB9F7FA1'7299AEADB6889018'501D289E4900F7E4'331B99DEC4B5433A'C7D329EEB6DD2654'5E96E55B874BE909_byte_array);
    EXPECT_EQ(TypeParam::sha512_context().process_string(std::string(1000000, 'a')).finalize(), 0xE718483D0CE76964'4E2E42C7BC15B463'8E1F98B13B204428'5632A803AFA973EB'DE0FF244877EA60A'4CB0432CE577C31B'EB009C5C2C49AA2E'4EADB217AD8CC09B_byte_array);

#ifdef NDEBUG
    {
        auto ctx = TypeParam::sha512_context();
        std::string s = "abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmno";
        for (int i = 0; i < 16777216; i++) ctx.process_string(s);
        EXPECT_EQ(ctx.finalize(), 0xB47C933421EA2DB1'49AD6E10FCE6C7F9'3D0752380180FFD7'F4629A712134831D'77BE6091B819ED35'2C2967A2E2D4FA50'50723C9630691F1A'05A7281DBE6C1086_byte_array);
    }
#endif
}

REGISTER_TYPED_TEST_CASE_P(
    Sha2Test,
    Sha224_TestVectors,
    Sha256_TestVectors,
    Sha384_TestVectors,
    Sha512_TestVectors);

struct ref_impl
{
    static auto sha224_context() { return sha2_context(create_sha224_context_ref()); }
    static auto sha256_context() { return sha2_context(create_sha256_context_ref()); }
    static auto sha384_context() { return sha2_context(create_sha384_context_ref()); }
    static auto sha512_context() { return sha2_context(create_sha512_context_ref()); }
};

INSTANTIATE_TYPED_TEST_CASE_P(ref, Sha2Test, ref_impl);

struct avx2_impl
{
    static auto sha224_context() { return sha2_context(create_sha224_context_avx2()); }
    static auto sha256_context() { return sha2_context(create_sha256_context_avx2()); }
    static auto sha384_context() { return sha2_context(create_sha384_context_avx2()); }
    static auto sha512_context() { return sha2_context(create_sha512_context_avx2()); }
};

INSTANTIATE_TYPED_TEST_CASE_P(avx2, Sha2Test, avx2_impl);
