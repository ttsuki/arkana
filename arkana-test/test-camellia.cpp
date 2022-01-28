#include "pch.h"

#include <arkana/bits/hex-int-literals.h>
#include <arkana/camellia/camellia.h>
#include "./helper.h"

using namespace arkana;
using namespace hex_int_literals;

template <class key_t, class value_t>
void test_ia32(key_t key, value_t plain, value_t cipher)
{
    //enc
    {
        auto kev = camellia::prepare_key_vector(key, true);
        auto p = load_u<camellia::block_t>(plain.data());
        auto c = camellia::process_block_ecb(p, kev);
        auto r = load_u<decltype(cipher)>(&c);
        EXPECT_EQ(r, cipher);
    }

    //dec
    {
        auto kev = camellia::prepare_key_vector(key, false);
        auto c = load_u<camellia::block_t>(cipher.data());
        auto p = camellia::process_block_ecb(c, kev);
        auto r = load_u<decltype(cipher)>(&p);
        EXPECT_EQ(r, plain);
    }
}

TEST(CAMELLIA, ia32)
{
    test_ia32(
        0x01'23'45'67'89'ab'cd'ef'fe'dc'ba'98'76'54'32'10_byte_array,
        0x01'23'45'67'89'ab'cd'ef'fe'dc'ba'98'76'54'32'10_byte_array,
        0x67'67'31'38'54'96'69'73'08'57'06'56'48'ea'be'43_byte_array);

    test_ia32(
        0x01'23'45'67'89'ab'cd'ef'fe'dc'ba'98'76'54'32'10'00'11'22'33'44'55'66'77_byte_array,
        0x01'23'45'67'89'ab'cd'ef'fe'dc'ba'98'76'54'32'10_byte_array,
        0xb4'99'34'01'b3'e9'96'f8'4e'e5'ce'e7'd7'9b'09'b9_byte_array);

    test_ia32(
        0x01'23'45'67'89'ab'cd'ef'fe'dc'ba'98'76'54'32'10'00'11'22'33'44'55'66'77'88'99'aa'bb'cc'dd'ee'ff_byte_array,
        0x01'23'45'67'89'ab'cd'ef'fe'dc'ba'98'76'54'32'10_byte_array,
        0x9a'cc'23'7d'ff'16'd7'6c'20'ef'7c'91'9e'3a'75'09_byte_array);
}

