/// @file
/// @brief	arkana::camellia
///			- An implementation of Camellia cipher
/// @author Copyright(c) 2021 ttsuki
/// 
/// This software is released under the MIT License.
/// https://opensource.org/licenses/MIT
///
/// - camellia: https://info.isl.ntt.co.jp/crypt/camellia/
///
/// This implementation based on
///   "Block Ciphers: Fast Implementations on x86-64 Architecture"
///   -- Oulu : J. Kivilinna, 2013,
///   http://jultika.oulu.fi/Record/nbnfioulu-201305311409

#pragma once

#include "camellia.h"
#include "../ark/xmm.h"

namespace arkana::camellia
{
    namespace avx2aesni
    {
        namespace impl
        {
            using namespace arkana::xmm;

            template <template <class T> class YMM, class T>
            ARKXMM_API transpose_32x4x4(YMM<T>& x0, YMM<T>& x1, YMM<T>& x2, YMM<T>& x3)
            {
                auto i0 = reinterpret<YMM<uint32_t>>(x0);                // i0 = | 00010203|04050607|08090A0B|0C0D0E0F |
                auto i1 = reinterpret<YMM<uint32_t>>(x1);                // i1 = | 10111213|14151617|18191A1B|1C1D1E1F |
                auto i2 = reinterpret<YMM<uint32_t>>(x2);                // i2 = | 20212223|24252627|28292A2B|2C2D2E2F |
                auto i3 = reinterpret<YMM<uint32_t>>(x3);                // i3 = | 30313233|34353637|38393A3B|3C3D3E3F |
                auto t0 = reinterpret<YMM<uint64_t>>(unpack_lo(i0, i1)); // t0 = | 00010203 10111213|04050607 14151617 |
                auto t1 = reinterpret<YMM<uint64_t>>(unpack_hi(i0, i1)); // t1 = | 08090A0B 18191A1B|0C0D0E0F 1C1D1E1F |
                auto t2 = reinterpret<YMM<uint64_t>>(unpack_lo(i2, i3)); // t2 = | 20210223 30313233|24252627 34353637 |
                auto t3 = reinterpret<YMM<uint64_t>>(unpack_hi(i2, i3)); // t3 = | 28290A2B 38393A3B|2C2D2E2F 3C3D3E3F |
                x0 = reinterpret<YMM<T>>(unpack_lo(t0, t2));             // x0 = | 00010203 10111213 20212223 30313233 |
                x1 = reinterpret<YMM<T>>(unpack_hi(t0, t2));             // x1 = | 04050607 14151617 24252627 34353637 |
                x2 = reinterpret<YMM<T>>(unpack_lo(t1, t3));             // x2 = | 08090A0B 18191A1B 28292A2B 38393A3B |
                x3 = reinterpret<YMM<T>>(unpack_hi(t1, t3));             // x3 = | 0C0D0E0F 1C1D1E1F 2C2D2E2F 3C3D3E3F |
            }

            template <class T> ARKXMM_API transpose_8x4x4(XMM<T>& r) { r = byte_shuffle_128(r, i8x16(0x00, 0x04, 0x08, 0x0C, 0x01, 0x05, 0x09, 0x0D, 0x02, 0x06, 0x0A, 0x0E, 0x03, 0x07, 0x0B, 0x0F)); }
            template <class T> ARKXMM_API transpose_8x4x4(YMM<T>& r) { r = byte_shuffle_128(r, i8x32(0x00, 0x04, 0x08, 0x0C, 0x01, 0x05, 0x09, 0x0D, 0x02, 0x06, 0x0A, 0x0E, 0x03, 0x07, 0x0B, 0x0F)); }

            template <template <class T> class YMM, class T>
            ARKXMM_API byte_slice_16x16(
                YMM<T>& x0, YMM<T>& x1, YMM<T>& x2, YMM<T>& x3,
                YMM<T>& x4, YMM<T>& x5, YMM<T>& x6, YMM<T>& x7,
                YMM<T>& x8, YMM<T>& x9, YMM<T>& xA, YMM<T>& xB,
                YMM<T>& xC, YMM<T>& xD, YMM<T>& xE, YMM<T>& xF)
            {
                // input:
                // x0 = | 00 01 02 03  04 05 06 07  08 09 0A 0B  0C 0D 0E 0F |
                // x1 = | 10 11 12 13  14 15 16 17  18 19 1A 1B  1C 1D 1E 1F |
                // x2 = | 20 21 22 23  24 25 26 27  28 29 2A 2B  2C 2D 2E 2F |
                // x3 = | 30 31 32 33  34 35 36 37  38 39 3A 3B  3C 3D 3E 3F |
                // x4 = | 40 41 42 43  44 45 46 47  48 49 4A 4B  4C 4D 4E 4F |
                // x5 = | 50 51 52 53  54 55 56 57  58 59 5A 5B  5C 5D 5E 5F |
                // x6 = | 60 61 62 63  64 65 66 67  68 69 6A 6B  6C 6D 6E 6F |
                // x7 = | 70 71 72 73  74 75 76 77  78 79 7A 7B  7C 7D 7E 7F |
                // x8 = | 80 81 82 83  84 85 86 87  88 89 8A 8B  8C 8D 8E 8F |
                // x9 = | 90 91 92 93  94 95 96 97  98 99 9A 9B  9C 9D 9E 9F |
                // xA = | A0 A1 A2 A3  A4 A5 A6 A7  A8 A9 AA AB  AC AD AE AF |
                // xB = | B0 B1 B2 B3  B4 B5 B6 B7  B8 B9 BA BB  BC BD BE BF |
                // xC = | C0 C1 C2 C3  C4 C5 C6 C7  C8 C9 CA CB  CC CD CE CF |
                // xD = | D0 D1 D2 D3  D4 D5 D6 D7  D8 D9 DA DB  DC DD DE DF |
                // xE = | E0 E1 E2 E3  E4 E5 E6 E7  E8 E9 EA EB  EC ED EE EF |
                // xF = | F0 F1 F2 F3  F4 F5 F6 F7  F8 F9 FA FB  FC FD FE FF |

                transpose_32x4x4(x0, x4, x8, xC);
                transpose_32x4x4(x1, x5, x9, xD);
                transpose_32x4x4(x2, x6, xA, xE);
                transpose_32x4x4(x3, x7, xB, xF);
                // x0 = | 00 01 02 03  40 41 42 43  80 81 82 83  C0 C1 C2 C3 |
                // x4 = | 04 05 06 07  44 45 46 47  84 85 86 87  34 35 36 37 |
                // x8 = | 08 09 0A 0B  48 49 4A 4B  88 89 8A 8B  38 39 3A 3B |
                // xC = | 0C 0D 0E 0F  4C 4D 4E 4F  8C 8D 8E 8F  3C 3D 3E 3F |
                // x1 = | 10 11 12 13  50 51 52 53  90 91 92 93  D0 D1 D2 D3 |
                // x5 = | 14 15 16 17  54 55 56 57  94 95 96 97  D4 D5 D6 D7 |
                // x9 = | 18 19 1A 1B  58 59 5A 5B  98 99 9A 9B  D8 D9 DA DB |
                // xD = | 1C 1D 1E 1F  5C 5D 5E 5F  9C 9D 9E 9F  DC DD DE DF |
                // x2 = | 20 21 22 23  60 61 62 63  A0 A1 A2 A3  E0 E1 E2 E3 |
                // x6 = | 24 25 26 27  64 65 66 67  A4 A5 A6 A7  E4 E5 E6 E7 |
                // xA = | 28 29 2A 2B  68 69 6A 6B  A8 A9 AA AB  E8 E9 EA EB |
                // xE = | 2C 2D 2E 2F  6C 6D 6E 6F  AC AD AE AF  EC ED EE EF |
                // x3 = | 30 31 32 33  70 71 72 73  B0 B1 B2 B3  F0 F1 F2 F3 |
                // x7 = | 34 35 36 37  74 75 76 77  B4 B5 B6 B7  F4 F5 F6 F7 |
                // xB = | 38 39 3A 3B  78 79 7A 7B  B8 B9 BA BB  F8 F9 FA FB |
                // xF = | 3C 3D 3E 3F  7C 7D 7E 7F  BC BD BE BF  FC FD FE FF |

                transpose_8x4x4(x0);
                transpose_8x4x4(x1);
                transpose_8x4x4(x2);
                transpose_8x4x4(x3);
                transpose_8x4x4(x4);
                transpose_8x4x4(x5);
                transpose_8x4x4(x6);
                transpose_8x4x4(x7);
                transpose_8x4x4(x8);
                transpose_8x4x4(x9);
                transpose_8x4x4(xA);
                transpose_8x4x4(xB);
                transpose_8x4x4(xC);
                transpose_8x4x4(xD);
                transpose_8x4x4(xE);
                transpose_8x4x4(xF);
                // x0 = | 00 40 80 C0  01 41 81 C1  02 42 82 C2  03 43 83 C3 |
                // x1 = | 10 50 90 D0  11 51 91 D1  12 52 92 D2  13 53 93 D3 |
                // x2 = | 20 60 A0 E0  21 61 A1 E1  22 62 A2 E2  23 63 A3 E3 |
                // x3 = | 30 70 B0 F0  31 71 B1 F1  32 72 B2 F2  33 73 B3 F3 |
                // x4 = | 04 44 84 C4  05 45 85 C5  06 46 86 C6  07 47 87 C7 |
                // x5 = | 14 54 94 D4  15 55 95 D5  16 56 96 D6  17 57 97 D7 |
                // x6 = | 24 64 A4 E4  25 65 A5 E5  26 66 A6 E6  27 67 A7 E7 |
                // x7 = | 34 74 B4 F4  35 75 B5 F5  36 76 B6 F6  37 77 B7 F7 |
                // x8 = | 08 48 88 C8  09 49 89 C9  0A 4A 8A CA  0B 4B 8B CB |
                // x9 = | 18 58 98 D8  19 59 99 D9  1A 5A 9A DA  1B 5B 9B DB |
                // xA = | 28 68 A8 E8  29 69 A9 E9  2A 6A AA EA  2B 6B AB EB |
                // xB = | 38 78 B8 F8  39 79 B9 F9  3A 7A BA FA  3B 7B BB FB |
                // xC = | 0C 4C 8C CC  0D 4D 8D CD  0E 4E 8E CE  0F 4F 8F CF |
                // xD = | 1C 5C 9C DC  1D 5D 9D DD  1E 5E 9E DE  1F 5F 9F DF |
                // xE = | 2C 6C AC EC  2D 6D AD ED  2E 6E AE EE  2F 6F AF EF |
                // xF = | 3C 7C BC FC  3D 7D BD FD  3E 7E BE FE  3F 7F BF FF |

                transpose_32x4x4(x0, x1, x2, x3);
                transpose_32x4x4(x4, x5, x6, x7);
                transpose_32x4x4(x8, x9, xA, xB);
                transpose_32x4x4(xC, xD, xE, xF);

                // output:
                // x0 = | 00 40 80 C0  10 50 90 D0  20 60 A0 E0  30 70 B0 F0 |
                // x1 = | 01 41 81 C1  11 51 91 D1  21 61 A1 E1  31 71 B1 F1 |
                // x2 = | 02 42 82 C2  12 52 92 D2  22 62 A2 E2  32 72 B2 F2 |
                // x3 = | 03 43 83 C3  13 53 93 D3  23 63 A3 E3  33 73 B3 F3 |
                // x4 = | 04 44 84 C4  14 54 94 D4  24 64 A4 E4  34 74 B4 F4 |
                // x5 = | 05 45 85 C5  15 55 95 D5  25 65 A5 E5  35 75 B5 F5 |
                // x6 = | 06 46 86 C6  16 56 96 D6  26 66 A6 E6  36 76 B6 F6 |
                // x7 = | 07 47 87 C7  17 57 97 D7  27 67 A7 E7  37 77 B7 F7 |
                // x8 = | 08 48 88 C8  18 58 98 D8  28 68 A8 E8  38 78 B8 F8 |
                // x9 = | 09 49 89 C9  19 59 99 D9  29 69 A9 E9  39 79 B9 F9 |
                // xA = | 0A 4A 8A CA  1A 5A 9A DA  2A 6A AA EA  3A 7A BA FA |
                // xB = | 0B 4B 8B CB  1B 5B 9B DB  2B 6B AB EB  3B 7B BB FB |
                // xC = | 0C 4C 8C CC  1C 5C 9C DC  2C 6C AC EC  3C 7C BC FC |
                // xD = | 0D 4D 8D CD  1D 5D 9D DD  2D 6D AD ED  3D 7D BD FD |
                // xE = | 0E 4E 8E CE  1E 5E 9E DE  2E 6E AE EE  3E 7E BE FE |
                // xF = | 0F 4F 8F CF  1F 5F 9F DF  2F 6F AF EF  3F 7F BF FF |
            }


            ARKXMM_API aes_sub_bytes(vu8x32 x) -> vu8x32
            {
                const vi8x32 aes_invshift = i8x32(0x00, 0x0D, 0x0A, 0x07, 0x04, 0x01, 0x0E, 0x0B, 0x08, 0x05, 0x02, 0x0F, 0x0C, 0x09, 0x06, 0x03);
                const vu8x32 aes_input = byte_shuffle_128(x, aes_invshift); // cancel aes shift
                const vu8x16 aes_round_key = zero<vu8x16>();

                // AES S-box
                //    _0 _1 _2 _3 _4 _5 _6 _7 _8 _9 _A _B _C _D _E _F
                // 0_|63 7c 77 7b f2 6b 6f c5 30 01 67 2b fe d7 ab 76
                // 1_|ca 82 c9 7d fa 59 47 f0 ad d4 a2 af 9c a4 72 c0
                // 2_|b7 fd 93 26 36 3f f7 cc 34 a5 e5 f1 71 d8 31 15
                // 3_|04 c7 23 c3 18 96 05 9a 07 12 80 e2 eb 27 b2 75
                // 4_|09 83 2c 1a 1b 6e 5a a0 52 3b d6 b3 29 e3 2f 84
                // 5_|53 d1 00 ed 20 fc b1 5b 6a cb be 39 4a 4c 58 cf
                // 6_|d0 ef aa fb 43 4d 33 85 45 f9 02 7f 50 3c 9f a8
                // 7_|51 a3 40 8f 92 9d 38 f5 bc b6 da 21 10 ff f3 d2
                // 8_|cd 0c 13 ec 5f 97 44 17 c4 a7 7e 3d 64 5d 19 73
                // 9_|60 81 4f dc 22 2a 90 88 46 ee b8 14 de 5e 0b db
                // A_|e0 32 3a 0a 49 06 24 5c c2 d3 ac 62 91 95 e4 79
                // B_|e7 c8 37 6d 8d d5 4e a9 6c 56 f4 ea 65 7a ae 08
                // C_|ba 78 25 2e 1c a6 b4 c6 e8 dd 74 1f 4b bd 8b 8a
                // D_|70 3e b5 66 48 03 f6 0e 61 35 57 b9 86 c1 1d 9e
                // E_|e1 f8 98 11 69 d9 8e 94 9b 1e 87 e9 ce 55 28 df
                // F_|8c a1 89 0d bf e6 42 68 41 99 2d 0f b0 54 bb 16

                const vu8x16 lo128 = {_mm_aesenclast_si128(_mm256_extracti128_si256(aes_input.v, 0), aes_round_key.v)};
                const vu8x16 hi128 = {_mm_aesenclast_si128(_mm256_extracti128_si256(aes_input.v, 1), aes_round_key.v)};
                return from_values<vu8x32>(lo128, hi128);
            }

            ARKXMM_API filter_8bit(vu8x32 x, vu8x32 tl, vu8x32 th) noexcept -> vu8x32
            {
                const vu32x8 mask = u32x8(0x0F0F0F0F);
                const vu32x8 t = reinterpret<vu32x8>(x);
                const vi8x32 il = reinterpret<vi8x32>(t & mask);
                const vi8x32 ih = reinterpret<vi8x32>(andnot(t, mask) >> 4);
                const vu8x32 lo = byte_shuffle_128(tl, il);
                const vu8x32 hi = byte_shuffle_128(th, ih);
                return lo ^ hi;
            }

            struct v32
            {
                vu8x32 x0, x1, x2, x3;
            };

            struct v64
            {
                v32 l, r;
            };

            struct v128
            {
                v64 l, r;
            };

            using key64 = uint64_t;

            ARKXMM_API operator ^=(v32& lhs, v32 rhs) noexcept -> v32&
            {
                lhs.x0 ^= rhs.x0;
                lhs.x1 ^= rhs.x1;
                lhs.x2 ^= rhs.x2;
                lhs.x3 ^= rhs.x3;
                return lhs;
            }

            ARKXMM_API operator ^=(v32& lhs, vu8x32 rhs) noexcept -> v32&
            {
                lhs.x0 ^= rhs;
                lhs.x1 ^= rhs;
                lhs.x2 ^= rhs;
                lhs.x3 ^= rhs;
                return lhs;
            }

            ARKXMM_API rotl_be1(v32 v) noexcept -> v32
            {
                using namespace xmm;
                auto ze = zero<vi8x32>();
                auto msb0 = reinterpret<vu8x32>(abs(reinterpret<vi8x32>(v.x0) < ze)); // x0 >> 7 // msb
                auto msb1 = reinterpret<vu8x32>(abs(reinterpret<vi8x32>(v.x1) < ze)); // x1 >> 7
                auto msb2 = reinterpret<vu8x32>(abs(reinterpret<vi8x32>(v.x2) < ze)); // x2 >> 7
                auto msb3 = reinterpret<vu8x32>(abs(reinterpret<vi8x32>(v.x3) < ze)); // x3 >> 7 // lsb
                v.x0 = (v.x0 + v.x0) | msb1;                                          // x0 << 1 | x3 >> 7
                v.x1 = (v.x1 + v.x1) | msb2;                                          // x1 << 1 | x0 >> 7
                v.x2 = (v.x2 + v.x2) | msb3;                                          // x2 << 1 | x1 >> 7
                v.x3 = (v.x3 + v.x3) | msb0;                                          // x3 << 1 | x2 >> 7
                return v;
            }


            ARKXMM_API camellia_f(v64& l, const v64& r, const key64& k) -> v64&
            {
                auto v = r;

                // k
                const vi8x32 ze = zero<vi8x32>();
                const vu64x4 kx = u64x4(k);
                v.l.x0 ^= reinterpret<vu8x32>(byte_shuffle_128(kx >> 0 * 8, ze)); // u8x32(static_cast<uint8_t>(k >> 0 * 8));
                v.l.x1 ^= reinterpret<vu8x32>(byte_shuffle_128(kx >> 1 * 8, ze)); // u8x32(static_cast<uint8_t>(k >> 1 * 8));
                v.l.x2 ^= reinterpret<vu8x32>(byte_shuffle_128(kx >> 2 * 8, ze)); // u8x32(static_cast<uint8_t>(k >> 2 * 8));
                v.l.x3 ^= reinterpret<vu8x32>(byte_shuffle_128(kx >> 3 * 8, ze)); // u8x32(static_cast<uint8_t>(k >> 3 * 8));
                v.r.x0 ^= reinterpret<vu8x32>(byte_shuffle_128(kx >> 4 * 8, ze)); // u8x32(static_cast<uint8_t>(k >> 4 * 8));
                v.r.x1 ^= reinterpret<vu8x32>(byte_shuffle_128(kx >> 5 * 8, ze)); // u8x32(static_cast<uint8_t>(k >> 5 * 8));
                v.r.x2 ^= reinterpret<vu8x32>(byte_shuffle_128(kx >> 6 * 8, ze)); // u8x32(static_cast<uint8_t>(k >> 6 * 8));
                v.r.x3 ^= reinterpret<vu8x32>(byte_shuffle_128(kx >> 7 * 8, ze)); // u8x32(static_cast<uint8_t>(k >> 7 * 8));

                // s
                const vu8x32 tf0l = u8x32(0x45, 0xe8, 0x40, 0xed, 0x2e, 0x83, 0x2b, 0x86, 0x4b, 0xe6, 0x4e, 0xe3, 0x20, 0x8d, 0x25, 0x88);
                const vu8x32 tf0h = u8x32(0x00, 0x51, 0xf1, 0xa0, 0x8a, 0xdb, 0x7b, 0x2a, 0x09, 0x58, 0xf8, 0xa9, 0x83, 0xd2, 0x72, 0x23);
                const vu8x32 tf1l = u8x32(0x45, 0x40, 0x2e, 0x2b, 0x4b, 0x4e, 0x20, 0x25, 0x14, 0x11, 0x7f, 0x7a, 0x1a, 0x1f, 0x71, 0x74);
                const vu8x32 tf1h = u8x32(0x00, 0xf1, 0x8a, 0x7b, 0x09, 0xf8, 0x83, 0x72, 0xad, 0x5c, 0x27, 0xd6, 0xa4, 0x55, 0x2e, 0xdf);
                const vu8x32 ts0l = u8x32(0x3c, 0xcc, 0xcf, 0x3f, 0x32, 0xc2, 0xc1, 0x31, 0xdc, 0x2c, 0x2f, 0xdf, 0xd2, 0x22, 0x21, 0xd1);
                const vu8x32 ts0h = u8x32(0x00, 0xf9, 0x86, 0x7f, 0xd7, 0x2e, 0x51, 0xa8, 0xa4, 0x5d, 0x22, 0xdb, 0x73, 0x8a, 0xf5, 0x0c);
                const vu8x32 ts1l = u8x32(0x78, 0x99, 0x9f, 0x7e, 0x64, 0x85, 0x83, 0x62, 0xb9, 0x58, 0x5e, 0xbf, 0xa5, 0x44, 0x42, 0xa3);
                const vu8x32 ts1h = u8x32(0x00, 0xf3, 0x0d, 0xfe, 0xaf, 0x5c, 0xa2, 0x51, 0x49, 0xba, 0x44, 0xb7, 0xe6, 0x15, 0xeb, 0x18);
                const vu8x32 ts7l = u8x32(0x1e, 0x66, 0xe7, 0x9f, 0x19, 0x61, 0xe0, 0x98, 0x6e, 0x16, 0x97, 0xef, 0x69, 0x11, 0x90, 0xe8);
                const vu8x32 ts7h = u8x32(0x00, 0xfc, 0x43, 0xbf, 0xeb, 0x17, 0xa8, 0x54, 0x52, 0xae, 0x11, 0xed, 0xb9, 0x45, 0xfa, 0x06);
                v.l.x0 = filter_8bit(aes_sub_bytes(filter_8bit(v.l.x0, tf0l, tf0h)), ts0l, ts0h); // SBox0
                v.l.x1 = filter_8bit(aes_sub_bytes(filter_8bit(v.l.x1, tf0l, tf0h)), ts1l, ts1h); // SBox1
                v.l.x2 = filter_8bit(aes_sub_bytes(filter_8bit(v.l.x2, tf0l, tf0h)), ts7l, ts7h); // SBox2
                v.l.x3 = filter_8bit(aes_sub_bytes(filter_8bit(v.l.x3, tf1l, tf1h)), ts0l, ts0h); // SBox3
                v.r.x0 = filter_8bit(aes_sub_bytes(filter_8bit(v.r.x0, tf0l, tf0h)), ts1l, ts1h); // SBox1
                v.r.x1 = filter_8bit(aes_sub_bytes(filter_8bit(v.r.x1, tf0l, tf0h)), ts7l, ts7h); // SBox2
                v.r.x2 = filter_8bit(aes_sub_bytes(filter_8bit(v.r.x2, tf1l, tf1h)), ts0l, ts0h); // SBox3
                v.r.x3 = filter_8bit(aes_sub_bytes(filter_8bit(v.r.x3, tf0l, tf0h)), ts0l, ts0h); // SBox0

                // p
                v.l.x0 ^= v.r.x1;
                v.l.x1 ^= v.r.x2;
                v.l.x2 ^= v.r.x3;
                v.l.x3 ^= v.r.x0;
                v.r.x0 ^= v.l.x2;
                v.r.x1 ^= v.l.x3;
                v.r.x2 ^= v.l.x0;
                v.r.x3 ^= v.l.x1;
                v.l.x0 ^= v.r.x3;
                v.l.x1 ^= v.r.x0;
                v.l.x2 ^= v.r.x1;
                v.l.x3 ^= v.r.x2;
                v.r.x0 ^= v.l.x3;
                v.r.x1 ^= v.l.x0;
                v.r.x2 ^= v.l.x1;
                v.r.x3 ^= v.l.x2;

                // store
                l.l ^= v.r;
                l.r ^= v.l;

                return l;
            }

            ARKXMM_API camellia_fl(v64& l, const key64& k) -> v64&
            {
                v32 v = l.l;

                const vi8x32 ze = zero<vi8x32>();
                const vu64x4 kx = u64x4(k);

                v.x0 &= reinterpret<vu8x32>(byte_shuffle_128(kx >> 0 * 8, ze)); // u8x32(static_cast<uint8_t>(k >> 0 * 8));
                v.x1 &= reinterpret<vu8x32>(byte_shuffle_128(kx >> 1 * 8, ze)); // u8x32(static_cast<uint8_t>(k >> 1 * 8));
                v.x2 &= reinterpret<vu8x32>(byte_shuffle_128(kx >> 2 * 8, ze)); // u8x32(static_cast<uint8_t>(k >> 2 * 8));
                v.x3 &= reinterpret<vu8x32>(byte_shuffle_128(kx >> 3 * 8, ze)); // u8x32(static_cast<uint8_t>(k >> 3 * 8));
                v = rotl_be1(v);
                v = l.r ^= v;

                v.x0 |= reinterpret<vu8x32>(byte_shuffle_128(kx >> 4 * 8, ze)); // u8x32(static_cast<uint8_t>(k >> 4 * 8));
                v.x1 |= reinterpret<vu8x32>(byte_shuffle_128(kx >> 5 * 8, ze)); // u8x32(static_cast<uint8_t>(k >> 5 * 8));
                v.x2 |= reinterpret<vu8x32>(byte_shuffle_128(kx >> 6 * 8, ze)); // u8x32(static_cast<uint8_t>(k >> 6 * 8));
                v.x3 |= reinterpret<vu8x32>(byte_shuffle_128(kx >> 7 * 8, ze)); // u8x32(static_cast<uint8_t>(k >> 7 * 8));
                v = l.l ^= v;

                return l;
            }

            ARKXMM_API camellia_fl_inv(v64& r, const key64& k) -> v64&
            {
                v32 v = r.r;

                const vi8x32 ze = zero<vi8x32>();
                const vu64x4 kx = u64x4(k);

                v.x0 |= reinterpret<vu8x32>(byte_shuffle_128(kx >> 4 * 8, ze)); // u8x32(static_cast<uint8_t>(k >> 4 * 8));
                v.x1 |= reinterpret<vu8x32>(byte_shuffle_128(kx >> 5 * 8, ze)); // u8x32(static_cast<uint8_t>(k >> 5 * 8));
                v.x2 |= reinterpret<vu8x32>(byte_shuffle_128(kx >> 6 * 8, ze)); // u8x32(static_cast<uint8_t>(k >> 6 * 8));
                v.x3 |= reinterpret<vu8x32>(byte_shuffle_128(kx >> 7 * 8, ze)); // u8x32(static_cast<uint8_t>(k >> 7 * 8));
                v = r.l ^= v;

                v.x0 &= reinterpret<vu8x32>(byte_shuffle_128(kx >> 0 * 8, ze)); // u8x32(static_cast<uint8_t>(k >> 0 * 8));
                v.x1 &= reinterpret<vu8x32>(byte_shuffle_128(kx >> 1 * 8, ze)); // u8x32(static_cast<uint8_t>(k >> 1 * 8));
                v.x2 &= reinterpret<vu8x32>(byte_shuffle_128(kx >> 2 * 8, ze)); // u8x32(static_cast<uint8_t>(k >> 2 * 8));
                v.x3 &= reinterpret<vu8x32>(byte_shuffle_128(kx >> 3 * 8, ze)); // u8x32(static_cast<uint8_t>(k >> 3 * 8));
                v = rotl_be1(v);
                v = r.r ^= v;

                return r;
            }

            ARKXMM_API camellia_prewhite(v128& block, const key64& kl, const key64& kr) -> v128&
            {
                const vu8x32 kx = reinterpret<vu8x32>(u64x4(kl, kr));

                block.l.l ^= kx;
                block.l.r ^= kx;
                block.r.l ^= kx;
                block.r.r ^= kx;

                byte_slice_16x16(
                    block.l.l.x0, block.l.l.x1, block.l.l.x2, block.l.l.x3,
                    block.l.r.x0, block.l.r.x1, block.l.r.x2, block.l.r.x3,
                    block.r.l.x0, block.r.l.x1, block.r.l.x2, block.r.l.x3,
                    block.r.r.x0, block.r.r.x1, block.r.r.x2, block.r.r.x3);

                return block;
            }

            ARKXMM_API camellia_postwhite(v128& block, const key64& kl, const key64& kr) -> v128&
            {
                const vu8x32 kx = reinterpret<vu8x32>(u64x4(kl, kr));

                byte_slice_16x16(
                    block.r.l.x0, block.r.r.x0, block.l.l.x0, block.l.r.x0,
                    block.r.l.x1, block.r.r.x1, block.l.l.x1, block.l.r.x1,
                    block.r.l.x2, block.r.r.x2, block.l.l.x2, block.l.r.x2,
                    block.r.l.x3, block.r.r.x3, block.l.l.x3, block.l.r.x3);

                block.l.l ^= kx;
                block.l.r ^= kx;
                block.r.l ^= kx;
                block.r.r ^= kx;
                return block;
            }

            ARKXMM_API camellia_thruwhite(v128& block, const key64&, const key64&) -> v128&
            {
                return block;
            }

            ARKXMM_API load_v128(const v128* src) -> v128
            {
                return v128{
                    {
                        {
                            xmm::load_u(&src->l.l.x0),
                            xmm::load_u(&src->l.l.x1),
                            xmm::load_u(&src->l.l.x2),
                            xmm::load_u(&src->l.l.x3)
                        },
                        {
                            xmm::load_u(&src->l.r.x0),
                            xmm::load_u(&src->l.r.x1),
                            xmm::load_u(&src->l.r.x2),
                            xmm::load_u(&src->l.r.x3)
                        },
                    },
                    {
                        {
                            xmm::load_u(&src->r.l.x0),
                            xmm::load_u(&src->r.l.x1),
                            xmm::load_u(&src->r.l.x2),
                            xmm::load_u(&src->r.l.x3)
                        },
                        {
                            xmm::load_u(&src->r.r.x0),
                            xmm::load_u(&src->r.r.x1),
                            xmm::load_u(&src->r.r.x2),
                            xmm::load_u(&src->r.r.x3)
                        },
                    }
                };
            }

            ARKXMM_API store_v128(v128* dst, const v128& reg)
            {
                xmm::store_u(&dst->l.l.x0, reg.l.l.x0);
                xmm::store_u(&dst->l.l.x1, reg.l.l.x1);
                xmm::store_u(&dst->l.l.x2, reg.l.l.x2);
                xmm::store_u(&dst->l.l.x3, reg.l.l.x3);
                xmm::store_u(&dst->l.r.x0, reg.l.r.x0);
                xmm::store_u(&dst->l.r.x1, reg.l.r.x1);
                xmm::store_u(&dst->l.r.x2, reg.l.r.x2);
                xmm::store_u(&dst->l.r.x3, reg.l.r.x3);
                xmm::store_u(&dst->r.l.x0, reg.r.l.x0);
                xmm::store_u(&dst->r.l.x1, reg.r.l.x1);
                xmm::store_u(&dst->r.l.x2, reg.r.l.x2);
                xmm::store_u(&dst->r.l.x3, reg.r.l.x3);
                xmm::store_u(&dst->r.r.x0, reg.r.r.x0);
                xmm::store_u(&dst->r.r.x1, reg.r.r.x1);
                xmm::store_u(&dst->r.r.x2, reg.r.r.x2);
                xmm::store_u(&dst->r.r.x3, reg.r.r.x3);
            }

            ARKXMM_API swap_xor128(v128& v, const v128& k) -> v128&
            {
                v.r.l ^= k.l.l;
                v.r.r ^= k.l.r;
                v.l.l ^= k.r.l;
                v.l.r ^= k.r.r;
                return v;
            }

            ARKXMM_API swap_store_v128(v128* dst, const v128& reg)
            {
                xmm::store_u(&dst->l.l.x0, reg.r.l.x0);
                xmm::store_u(&dst->l.l.x1, reg.r.l.x1);
                xmm::store_u(&dst->l.l.x2, reg.r.l.x2);
                xmm::store_u(&dst->l.l.x3, reg.r.l.x3);
                xmm::store_u(&dst->l.r.x0, reg.r.r.x0);
                xmm::store_u(&dst->l.r.x1, reg.r.r.x1);
                xmm::store_u(&dst->l.r.x2, reg.r.r.x2);
                xmm::store_u(&dst->l.r.x3, reg.r.r.x3);
                xmm::store_u(&dst->r.l.x0, reg.l.l.x0);
                xmm::store_u(&dst->r.l.x1, reg.l.l.x1);
                xmm::store_u(&dst->r.l.x2, reg.l.l.x2);
                xmm::store_u(&dst->r.l.x3, reg.l.l.x3);
                xmm::store_u(&dst->r.r.x0, reg.l.r.x0);
                xmm::store_u(&dst->r.r.x1, reg.l.r.x1);
                xmm::store_u(&dst->r.r.x2, reg.l.r.x2);
                xmm::store_u(&dst->r.r.x3, reg.l.r.x3);
            }
        }

        using ref::key_vector_small_t;
        using ref::key_vector_large_t;

        namespace impl
        {
            using ref::impl::key_vector_small_t;
            using ref::impl::key_vector_large_t;
            using ref::impl::generate_key_vector;

            template <class key_vector_t>
            static inline auto process_blocks_ecb(void* dst, const void* src, size_t length, const key_vector_t& kv)
            -> std::enable_if_t<is_any_of<key_vector_t, key_vector_small_t, key_vector_large_t>::value, void>
            {
                // rfc3713 ecb-mode
                using namespace functions;
                ecb_mode::process_blocks_ecb<
                    v128,
                    load_v128,
                    camellia_prewhite,
                    camellia_f,
                    camellia_fl,
                    camellia_fl_inv,
                    camellia_postwhite,
                    swap_store_v128>(dst, src, length, kv);
            }

            using ref::impl::ctr_iv_t;
            using ref::impl::ctr_nonce_t;

            template <class key_vector_t>
            static inline auto process_bytes_ctr(void* dst, const void* src, size_t position, size_t length, const key_vector_t& kv, const ctr_iv_t& ctr_iv, const ctr_nonce_t& ctr_nonce)
            -> std::enable_if_t<is_any_of<key_vector_t, key_vector_small_t, key_vector_large_t>::value, void>
            {
                // rfc5528 ctr-mode
                struct ctr_t
                {
                    uint32_t n, ivl, ivr, ctr;
                };

                ctr_t ctr0 = arkana::load_u<ctr_t>(&kv);
                ctr0.n ^= arkana::load_u<uint32_t>(ctr_nonce.data() + 0);
                ctr0.ivl ^= arkana::load_u<uint32_t>(ctr_iv.data() + 0);
                ctr0.ivr ^= arkana::load_u<uint32_t>(ctr_iv.data() + 4);

                using namespace functions;
                ctr_mode::process_bytes_ctr<
                    v128,
                    camellia_thruwhite,
                    camellia_f,
                    camellia_fl,
                    camellia_fl_inv,
                    camellia_postwhite,
                    load_v128,
                    swap_xor128,
                    store_v128>(dst, src, position, length, kv, [ctr0](size_t index) -> v128
                {
                    const auto ze = zero<vi8x32>();

                    v32 ctr{
                        u8x32(static_cast<uint8_t>((index * 32) >> 24)),
                        u8x32(static_cast<uint8_t>((index * 32) >> 16)),
                        u8x32(static_cast<uint8_t>((index * 32) >> 8)),
                        u8x32(static_cast<uint8_t>((index * 32) >> 0)),
                    };

                    ctr.x3 += u8x32(
                        1, 9, 17, 25, 3, 11, 19, 27,
                        5, 13, 21, 29, 7, 15, 23, 31,
                        2, 10, 18, 26, 4, 12, 20, 28,
                        6, 14, 22, 30, 8, 16, 24, 32);
                    vu8x32 cf = reinterpret<vu8x32>(abs(reinterpret<vi8x32>(ctr.x3) == ze));
                    ctr.x2 += cf;
                    cf &= reinterpret<vu8x32>(abs(reinterpret<vi8x32>(ctr.x2) == ze));
                    ctr.x1 += cf;
                    cf &= reinterpret<vu8x32>(abs(reinterpret<vi8x32>(ctr.x1) == ze));
                    ctr.x0 += cf;

                    v128 v{};
                    v.l.l.x0 = u8x32(static_cast<uint8_t>(ctr0.n >> 0 * 8));
                    v.l.l.x1 = u8x32(static_cast<uint8_t>(ctr0.n >> 1 * 8));
                    v.l.l.x2 = u8x32(static_cast<uint8_t>(ctr0.n >> 2 * 8));
                    v.l.l.x3 = u8x32(static_cast<uint8_t>(ctr0.n >> 3 * 8));
                    v.l.r.x0 = u8x32(static_cast<uint8_t>(ctr0.ivl >> 0 * 8));
                    v.l.r.x1 = u8x32(static_cast<uint8_t>(ctr0.ivl >> 1 * 8));
                    v.l.r.x2 = u8x32(static_cast<uint8_t>(ctr0.ivl >> 2 * 8));
                    v.l.r.x3 = u8x32(static_cast<uint8_t>(ctr0.ivl >> 3 * 8));
                    v.r.l.x0 = u8x32(static_cast<uint8_t>(ctr0.ivr >> 0 * 8));
                    v.r.l.x1 = u8x32(static_cast<uint8_t>(ctr0.ivr >> 1 * 8));
                    v.r.l.x2 = u8x32(static_cast<uint8_t>(ctr0.ivr >> 2 * 8));
                    v.r.l.x3 = u8x32(static_cast<uint8_t>(ctr0.ivr >> 3 * 8));
                    v.r.r.x0 = u8x32(static_cast<uint8_t>(ctr0.ctr >> 0 * 8)) ^ ctr.x0;
                    v.r.r.x1 = u8x32(static_cast<uint8_t>(ctr0.ctr >> 1 * 8)) ^ ctr.x1;
                    v.r.r.x2 = u8x32(static_cast<uint8_t>(ctr0.ctr >> 2 * 8)) ^ ctr.x2;
                    v.r.r.x3 = u8x32(static_cast<uint8_t>(ctr0.ctr >> 3 * 8)) ^ ctr.x3;
                    return v;
                });
            }

            template <class key_vector_t, class ctr_generator_t>
            static inline auto process_bytes_ctr(void* dst, const void* src, size_t position, size_t length, const key_vector_t& kv, ctr_generator_t&& ctr)
            -> std::enable_if_t<is_any_of<key_vector_t, key_vector_small_t, key_vector_large_t>::value, void>
            {
                // custom ctr-mode
                using ctr_t = std::invoke_result_t<ctr_generator_t, size_t>;
                static_assert(sizeof(ctr_t) == 16);
                static_assert(std::is_trivially_copyable_v<ctr_t>);

                using namespace functions;
                ctr_mode::process_bytes_ctr<
                    v128,
                    camellia_prewhite,
                    camellia_f,
                    camellia_fl,
                    camellia_fl_inv,
                    camellia_postwhite,
                    load_v128,
                    swap_xor128,
                    store_v128>(dst, src, position, length, kv, [ctr = std::forward<decltype(ctr)>(ctr)](size_t index) -> v128
                {
                    array<ctr_t, 32> v = {
                        ctr(index * 32 + 0),
                        ctr(index * 32 + 1),
                        ctr(index * 32 + 2),
                        ctr(index * 32 + 3),
                        ctr(index * 32 + 4),
                        ctr(index * 32 + 5),
                        ctr(index * 32 + 6),
                        ctr(index * 32 + 7),
                        ctr(index * 32 + 8),
                        ctr(index * 32 + 9),
                        ctr(index * 32 + 10),
                        ctr(index * 32 + 11),
                        ctr(index * 32 + 12),
                        ctr(index * 32 + 13),
                        ctr(index * 32 + 14),
                        ctr(index * 32 + 15),
                        ctr(index * 32 + 16),
                        ctr(index * 32 + 17),
                        ctr(index * 32 + 18),
                        ctr(index * 32 + 19),
                        ctr(index * 32 + 20),
                        ctr(index * 32 + 21),
                        ctr(index * 32 + 22),
                        ctr(index * 32 + 23),
                        ctr(index * 32 + 24),
                        ctr(index * 32 + 25),
                        ctr(index * 32 + 26),
                        ctr(index * 32 + 27),
                        ctr(index * 32 + 28),
                        ctr(index * 32 + 29),
                        ctr(index * 32 + 30),
                        ctr(index * 32 + 31),
                    };

                    return load_v128(reinterpret_cast<v128*>(v.data()));
                });
            }
        }

        using impl::key_vector_small_t;
        using impl::key_vector_large_t;
        static inline key_vector_small_t generate_key_vector_encrypt(const key_128bit_t& key) { return impl::generate_key_vector(key, true_t{}); }
        static inline key_vector_large_t generate_key_vector_encrypt(const key_192bit_t& key) { return impl::generate_key_vector(key, true_t{}); }
        static inline key_vector_large_t generate_key_vector_encrypt(const key_256bit_t& key) { return impl::generate_key_vector(key, true_t{}); }
        static inline key_vector_small_t generate_key_vector_decrypt(const key_128bit_t& key) { return impl::generate_key_vector(key, false_t{}); }
        static inline key_vector_large_t generate_key_vector_decrypt(const key_192bit_t& key) { return impl::generate_key_vector(key, false_t{}); }
        static inline key_vector_large_t generate_key_vector_decrypt(const key_256bit_t& key) { return impl::generate_key_vector(key, false_t{}); }

        static inline void process_blocks_ecb(void* dst, const void* src, size_t length, const key_vector_small_t& kv) { return impl::process_blocks_ecb(dst, src, length, kv); }
        static inline void process_blocks_ecb(void* dst, const void* src, size_t length, const key_vector_large_t& kv) { return impl::process_blocks_ecb(dst, src, length, kv); }

        using impl::ctr_iv_t;
        using impl::ctr_nonce_t;
        static inline void process_bytes_ctr(void* dst, const void* src, size_t position, size_t length, const key_vector_small_t& kv, const ctr_iv_t& ctr_iv, const ctr_nonce_t& ctr_nonce) { return impl::process_bytes_ctr(dst, src, position, length, kv, ctr_iv, ctr_nonce); }
        static inline void process_bytes_ctr(void* dst, const void* src, size_t position, size_t length, const key_vector_large_t& kv, const ctr_iv_t& ctr_iv, const ctr_nonce_t& ctr_nonce) { return impl::process_bytes_ctr(dst, src, position, length, kv, ctr_iv, ctr_nonce); }
    }
}
