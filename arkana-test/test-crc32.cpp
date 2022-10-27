#include "./gtest.h"
#include "../arkana/ark.h"
#include "../arkana/crc32/crc32.h"
#include "./helper.h"

using namespace arkana::crc32;

template <class context_t>
crc32_value_t calculate_crc32(context_t ctx, const void* data, size_t length)
{
    ctx->update(data, length);
    return ctx->current();
}

struct Crc32TestBase : testing::Test
{
#ifndef NDEBUG
    static inline const auto& data = static_random_bytes_1m;
#else
    static inline const auto& data = static_random_bytes_256m;
#endif

    static inline const crc32_value_t expected_a = calculate_crc32(create_crc32_context_ref(), data.data() + 0, data.size() - 0);
    static inline const crc32_value_t expected_u = calculate_crc32(create_crc32_context_ref(), data.data() + 1, data.size() - 2);
};


template <typename T>
struct Crc32Test : Crc32TestBase
{
};

TYPED_TEST_SUITE_P(Crc32Test);

TYPED_TEST_P(Crc32Test, ZeroVector)
{
    constexpr std::array<std::byte, 257> zero_data{};
    constexpr std::array<crc32_value_t, 256> expect_zero
    {
        0x00000000, 0xD202EF8D, 0x41D912FF, 0xFF41D912, 0x2144DF1C, 0xC622F71D, 0xB1C2A1A3, 0x9D6CDF7E,
        0x6522DF69, 0xE60914AE, 0xE38A6876, 0x6B87B1EC, 0x7BD5C66F, 0x0F744682, 0xD1BB79C7, 0xD7D303E7,
        0xECBB4B55, 0xC9EFF1BD, 0x671BCF4D, 0xDA08C96F, 0x0FD59B8D, 0x4104C58B, 0xA829B1E0, 0x72A02444,
        0xA3C1CA20, 0xE9CF0E8F, 0xAFECBE32, 0x1A7A52B3, 0x807077E9, 0x0B54C526, 0x00043EB5, 0x6909ACB0,
        0x190A55AD, 0x7A7C3A8D, 0x41716C2A, 0x09F85737, 0x6AB6B2D5, 0x24D17F64, 0x98F99BB3, 0x80F2F420,
        0xE9EC3DB1, 0x6E8D80AA, 0xE46F28FB, 0xF889AB31, 0x8324661C, 0xC68097A4, 0x037F08EB, 0xE5D9AB75,
        0xF288B395, 0x529500F5, 0x1F877C1E, 0x28125592, 0xCC2B0FB0, 0x19AF770E, 0x35A36DFD, 0x113BC241,
        0xD3C8A549, 0xDDD1DE1C, 0xC6DE621C, 0xC6C56DA0, 0x04128908, 0xDCDD7536, 0x1D64A761, 0xE8AADAE4,
        0x758D6336, 0x1DCDF777, 0x1C7EC6E5, 0x027E87BC, 0x10D76EAD, 0x7A75E7B6, 0xF07A8CD3, 0xCD28166F,
        0x0FC2BB52, 0x57681DEE, 0x95E748EF, 0xE222F72C, 0xE038A199, 0x5B31FCCC, 0x408B50EA, 0x929D6FBB,
        0x8E2318E6, 0x9BE58BD8, 0x5A9150E0, 0x72529CA5, 0x74CCEA76, 0x6B10F76E, 0x7863B325, 0x997E5879,
        0xFB42584D, 0xDA9490F8, 0x61BE0133, 0x6DB3309A, 0xC2B526E7, 0xECAE2D70, 0x82EB309C, 0x2B39DBD2,
        0xBAF465AE, 0xE3D69507, 0x4C85ACBB, 0x8EFD0025, 0x9988C6CA, 0xA92A4CE5, 0x02CBD336, 0x1DBAB1C7,
        0xD71F022F, 0x7904CDD6, 0xBDCB9CA1, 0x736EB76F, 0x0F7CFDF3, 0xF6B930D6, 0xBD44215C, 0xB062B673,
        0x1BBEADBD, 0x67C99E11, 0xB8D506E1, 0x05B7E865, 0xEFDFCDB2, 0xF782E2E0, 0x72FF8F17, 0x51A395C5,
        0x395D7A27, 0x7731079C, 0x2BCC01E5, 0x0249357B, 0x15D70E0C, 0xDBA174A8, 0x0AD46523, 0x706F4A9A,
        0xC2A8FA9D, 0x5C7EA88E, 0xD85E3F02, 0x3CD4D09E, 0xC589851E, 0x28C85B6B, 0x084A9F06, 0x3B690027,
        0x773333E6, 0x9B1C9BF3, 0xF62D50B0, 0x19957151, 0xCE771B9E, 0xC57B26D5, 0x247EB2F0, 0x6F9B6323,
        0x700A059C, 0x2BCB3AE7, 0xEC47536C, 0x96EA85AD, 0x7AF3DA5D, 0xC7A2311E, 0x28CA70DF, 0xC446EAB8,
        0x177C92D9, 0x2D1AF96F, 0x0F2289BD, 0x67DD0235, 0x84D6F6AC, 0x0DE6D6B8, 0x17B532E5, 0x02754C48,
        0xAA075363, 0x0613D83C, 0xFD6B80D2, 0xBA2237F5, 0x1F6FCB29, 0x90AF182A, 0x09298943, 0x3DDED62E,
        0x0EE93C94, 0x256E51EC, 0x7B9B2F8F, 0xAF7EEA13, 0x5613D0B9, 0x60E9F775, 0xF20D83C9, 0x3048981A,
        0x2F505E6F, 0x0F20C31A, 0x2F6F3634, 0xF399740E, 0x35495BFE, 0x883279CD, 0x375F63F9, 0x1654FA56,
        0x501C4FB6, 0xF050E57B, 0x152517DC, 0x5D725465, 0xEF87080E, 0x35554582, 0xD18158C4, 0x4EDA687C,
        0x8BFF08F2, 0x813A83B5, 0x6988920D, 0xACDA1BA2, 0xEA76F752, 0x578DA9A2, 0xEA8DA0E0, 0x72E28055,
        0xC971A876, 0x6BAD4A2C, 0xE0B12E24, 0xEEE1BA72, 0x6CE71E27, 0x7764BDF8, 0x6113F11E, 0x286CC11F,
        0x5F228EB9, 0x60E0C62B, 0x7EDEF60B, 0x45AEE8F3, 0xF6F3E2C3, 0xD0998F65, 0xEF0AE3D5, 0x2454C335,
        0x84957F6D, 0xE1856717, 0x5130EF2D, 0x978C8317, 0x5146E6C9, 0x30EBD37F, 0x128868F3, 0xF6A4C443,
        0x3D215B63, 0x0684FE34, 0xF3B09FC6, 0xA0F63897, 0xBCC91F52, 0x57DB164A, 0x445C9C15, 0xBF9B57FA,
        0x8FD56FD8, 0x5A856004, 0xD535AEF4, 0x6807EC26, 0x00676D9C, 0x2BBB578F, 0xAF2ECA6B, 0x08CD7997,
        0xBC612413, 0x5600CF77, 0x1C350BDD, 0x2A7C74EF, 0xE29D6C10, 0xCF576285, 0x4F1FCF40, 0xA491B1D2,
        0xBA7BCDC4, 0x4EB192E9, 0x0B9A04C3, 0xD064E683, 0xA66359F1, 0x181E4E5E, 0x5EC98D30, 0xF48516AC,
    };

    for (size_t i = 0; i < 256; i++)
    {
        // aligned
        EXPECT_EQ(calculate_crc32(TypeParam::create_context(), zero_data.data(), i), expect_zero[i]);
    }

    for (size_t i = 0; i < 256; i++)
    {
        // unaligned
        EXPECT_EQ(calculate_crc32(TypeParam::create_context(), zero_data.data() + 1, i), expect_zero[i]);
    }
}

TYPED_TEST_P(Crc32Test, MatchWithRefImpl)
{
    EXPECT_EQ(calculate_crc32(TypeParam::create_context(), TestFixture::data.data() + 0, TestFixture::data.size() - 0), TestFixture::expected_a);
    EXPECT_EQ(calculate_crc32(TypeParam::create_context(), TestFixture::data.data() + 1, TestFixture::data.size() - 2), TestFixture::expected_u);
}

REGISTER_TYPED_TEST_SUITE_P(Crc32Test, ZeroVector, MatchWithRefImpl);

struct ref_impl
{
    static auto create_context() { return create_crc32_context_ref(); }
};

INSTANTIATE_TYPED_TEST_SUITE_P(ref, Crc32Test, ref_impl);

struct ia32_impl
{
    static auto create_context() { return create_crc32_context_ia32(); }
};

INSTANTIATE_TYPED_TEST_SUITE_P(ia32, Crc32Test, ia32_impl);

struct avx2_impl
{
    static auto create_context() { return create_crc32_context_avx2(); }
};

INSTANTIATE_TYPED_TEST_SUITE_P(avx2, Crc32Test, avx2_impl);

struct avx2clmul_impl
{
    static auto create_context() { return create_crc32_context_avx2clmul(); }
};

INSTANTIATE_TYPED_TEST_SUITE_P(avx2clmul, Crc32Test, avx2clmul_impl);
