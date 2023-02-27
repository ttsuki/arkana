/// @file
/// @brief	arkana::crc32
///			- An implementation of CRC32
/// @author Copyright(c) 2020 ttsuki
/// 
/// This software is released under the MIT License.
/// https://opensource.org/licenses/MIT

#pragma once

#include "crc32-ia32.h"
#include "../ark/xmm.h"

namespace arkana::crc32
{
    namespace avx2
    {
        template <uint32_t polynomial>
        static inline crc32_value_t calculate_crc32(const void* data, size_t length, crc32_value_t current = 0)
        {
#if defined(_MSC_VER) && _MSC_VER < 1920 // explicit instantiate tables (for workaround VS2017 bug?)
            [[maybe_unused]] constexpr auto &dummy0 = tables::crc32_table_n<polynomial, 0>;
            [[maybe_unused]] constexpr auto &dummy1 = tables::crc32_table_n<polynomial, 1>;
            [[maybe_unused]] constexpr auto &dummy2 = tables::crc32_table_n<polynomial, 2>;
            [[maybe_unused]] constexpr auto &dummy3 = tables::crc32_table_n<polynomial, 3>;
            [[maybe_unused]] constexpr auto &dummy4 = tables::crc32_table_n<polynomial, 4>;
            [[maybe_unused]] constexpr auto &dummy5 = tables::crc32_table_n<polynomial, 5>;
            [[maybe_unused]] constexpr auto &dummy6 = tables::crc32_table_n<polynomial, 6>;
            [[maybe_unused]] constexpr auto &dummy7 = tables::crc32_table_n<polynomial, 7>;
            [[maybe_unused]] constexpr auto &dummy8 = tables::crc32_table_n<polynomial, 8>;
            [[maybe_unused]] constexpr auto &dummy9 = tables::crc32_table_n<polynomial, 9>;
            [[maybe_unused]] constexpr auto &dummy10 = tables::crc32_table_n<polynomial, 10>;
            [[maybe_unused]] constexpr auto &dummy11 = tables::crc32_table_n<polynomial, 11>;
            [[maybe_unused]] constexpr auto &dummy12 = tables::crc32_table_n<polynomial, 12>;
            [[maybe_unused]] constexpr auto &dummy13 = tables::crc32_table_n<polynomial, 13>;
            [[maybe_unused]] constexpr auto &dummy14 = tables::crc32_table_n<polynomial, 14>;
            [[maybe_unused]] constexpr auto &dummy15 = tables::crc32_table_n<polynomial, 15>;
            [[maybe_unused]] constexpr auto &dummy16 = tables::crc32_table_n<polynomial, 16>;
            [[maybe_unused]] constexpr auto &dummy17 = tables::crc32_table_n<polynomial, 17>;
            [[maybe_unused]] constexpr auto &dummy18 = tables::crc32_table_n<polynomial, 18>;
            [[maybe_unused]] constexpr auto &dummy19 = tables::crc32_table_n<polynomial, 19>;
            [[maybe_unused]] constexpr auto &dummy20 = tables::crc32_table_n<polynomial, 20>;
            [[maybe_unused]] constexpr auto &dummy21 = tables::crc32_table_n<polynomial, 21>;
            [[maybe_unused]] constexpr auto &dummy22 = tables::crc32_table_n<polynomial, 22>;
            [[maybe_unused]] constexpr auto &dummy23 = tables::crc32_table_n<polynomial, 23>;
            [[maybe_unused]] constexpr auto &dummy24 = tables::crc32_table_n<polynomial, 24>;
            [[maybe_unused]] constexpr auto &dummy25 = tables::crc32_table_n<polynomial, 25>;
            [[maybe_unused]] constexpr auto &dummy26 = tables::crc32_table_n<polynomial, 26>;
            [[maybe_unused]] constexpr auto &dummy27 = tables::crc32_table_n<polynomial, 27>;
            [[maybe_unused]] constexpr auto &dummy28 = tables::crc32_table_n<polynomial, 28>;
            [[maybe_unused]] constexpr auto &dummy29 = tables::crc32_table_n<polynomial, 29>;
            [[maybe_unused]] constexpr auto &dummy30 = tables::crc32_table_n<polynomial, 30>;
            [[maybe_unused]] constexpr auto &dummy31 = tables::crc32_table_n<polynomial, 31>;
            [[maybe_unused]] constexpr auto &dummy32 = tables::crc32_table_n<polynomial, 32>;
            [[maybe_unused]] constexpr auto &dummy33 = tables::crc32_table_n<polynomial, 33>;
            [[maybe_unused]] constexpr auto &dummy34 = tables::crc32_table_n<polynomial, 34>;
            [[maybe_unused]] constexpr auto &dummy35 = tables::crc32_table_n<polynomial, 35>;
            [[maybe_unused]] constexpr auto &dummy36 = tables::crc32_table_n<polynomial, 36>;
            [[maybe_unused]] constexpr auto &dummy37 = tables::crc32_table_n<polynomial, 37>;
            [[maybe_unused]] constexpr auto &dummy38 = tables::crc32_table_n<polynomial, 38>;
            [[maybe_unused]] constexpr auto &dummy39 = tables::crc32_table_n<polynomial, 39>;
            [[maybe_unused]] constexpr auto &dummy40 = tables::crc32_table_n<polynomial, 40>;
            [[maybe_unused]] constexpr auto &dummy41 = tables::crc32_table_n<polynomial, 41>;
            [[maybe_unused]] constexpr auto &dummy42 = tables::crc32_table_n<polynomial, 42>;
            [[maybe_unused]] constexpr auto &dummy43 = tables::crc32_table_n<polynomial, 43>;
            [[maybe_unused]] constexpr auto &dummy44 = tables::crc32_table_n<polynomial, 44>;
            [[maybe_unused]] constexpr auto &dummy45 = tables::crc32_table_n<polynomial, 45>;
            [[maybe_unused]] constexpr auto &dummy46 = tables::crc32_table_n<polynomial, 46>;
            [[maybe_unused]] constexpr auto &dummy47 = tables::crc32_table_n<polynomial, 47>;
            [[maybe_unused]] constexpr auto &dummy48 = tables::crc32_table_n<polynomial, 48>;
            [[maybe_unused]] constexpr auto &dummy49 = tables::crc32_table_n<polynomial, 49>;
            [[maybe_unused]] constexpr auto &dummy50 = tables::crc32_table_n<polynomial, 50>;
            [[maybe_unused]] constexpr auto &dummy51 = tables::crc32_table_n<polynomial, 51>;
            [[maybe_unused]] constexpr auto &dummy52 = tables::crc32_table_n<polynomial, 52>;
            [[maybe_unused]] constexpr auto &dummy53 = tables::crc32_table_n<polynomial, 53>;
            [[maybe_unused]] constexpr auto &dummy54 = tables::crc32_table_n<polynomial, 54>;
            [[maybe_unused]] constexpr auto &dummy55 = tables::crc32_table_n<polynomial, 55>;
            [[maybe_unused]] constexpr auto &dummy56 = tables::crc32_table_n<polynomial, 56>;
            [[maybe_unused]] constexpr auto &dummy57 = tables::crc32_table_n<polynomial, 57>;
            [[maybe_unused]] constexpr auto &dummy58 = tables::crc32_table_n<polynomial, 58>;
            [[maybe_unused]] constexpr auto &dummy59 = tables::crc32_table_n<polynomial, 59>;
            [[maybe_unused]] constexpr auto &dummy60 = tables::crc32_table_n<polynomial, 60>;
            [[maybe_unused]] constexpr auto &dummy61 = tables::crc32_table_n<polynomial, 61>;
            [[maybe_unused]] constexpr auto &dummy62 = tables::crc32_table_n<polynomial, 62>;
            [[maybe_unused]] constexpr auto &dummy63 = tables::crc32_table_n<polynomial, 63>;
            [[maybe_unused]] constexpr auto &dummy64 = tables::crc32_table_n<polynomial, 64>;
            [[maybe_unused]] constexpr auto &dummy65 = tables::crc32_table_n<polynomial, 65>;
            [[maybe_unused]] constexpr auto &dummy66 = tables::crc32_table_n<polynomial, 66>;
            [[maybe_unused]] constexpr auto &dummy67 = tables::crc32_table_n<polynomial, 67>;
            [[maybe_unused]] constexpr auto &dummy68 = tables::crc32_table_n<polynomial, 68>;
            [[maybe_unused]] constexpr auto &dummy69 = tables::crc32_table_n<polynomial, 69>;
            [[maybe_unused]] constexpr auto &dummy70 = tables::crc32_table_n<polynomial, 70>;
            [[maybe_unused]] constexpr auto &dummy71 = tables::crc32_table_n<polynomial, 71>;
            [[maybe_unused]] constexpr auto &dummy72 = tables::crc32_table_n<polynomial, 72>;
            [[maybe_unused]] constexpr auto &dummy73 = tables::crc32_table_n<polynomial, 73>;
            [[maybe_unused]] constexpr auto &dummy74 = tables::crc32_table_n<polynomial, 74>;
            [[maybe_unused]] constexpr auto &dummy75 = tables::crc32_table_n<polynomial, 75>;
            [[maybe_unused]] constexpr auto &dummy76 = tables::crc32_table_n<polynomial, 76>;
            [[maybe_unused]] constexpr auto &dummy77 = tables::crc32_table_n<polynomial, 77>;
            [[maybe_unused]] constexpr auto &dummy78 = tables::crc32_table_n<polynomial, 78>;
            [[maybe_unused]] constexpr auto &dummy79 = tables::crc32_table_n<polynomial, 79>;
            [[maybe_unused]] constexpr auto &dummy80 = tables::crc32_table_n<polynomial, 80>;
            [[maybe_unused]] constexpr auto &dummy81 = tables::crc32_table_n<polynomial, 81>;
            [[maybe_unused]] constexpr auto &dummy82 = tables::crc32_table_n<polynomial, 82>;
            [[maybe_unused]] constexpr auto &dummy83 = tables::crc32_table_n<polynomial, 83>;
            [[maybe_unused]] constexpr auto &dummy84 = tables::crc32_table_n<polynomial, 84>;
            [[maybe_unused]] constexpr auto &dummy85 = tables::crc32_table_n<polynomial, 85>;
            [[maybe_unused]] constexpr auto &dummy86 = tables::crc32_table_n<polynomial, 86>;
            [[maybe_unused]] constexpr auto &dummy87 = tables::crc32_table_n<polynomial, 87>;
            [[maybe_unused]] constexpr auto &dummy88 = tables::crc32_table_n<polynomial, 88>;
            [[maybe_unused]] constexpr auto &dummy89 = tables::crc32_table_n<polynomial, 89>;
            [[maybe_unused]] constexpr auto &dummy90 = tables::crc32_table_n<polynomial, 90>;
            [[maybe_unused]] constexpr auto &dummy91 = tables::crc32_table_n<polynomial, 91>;
            [[maybe_unused]] constexpr auto &dummy92 = tables::crc32_table_n<polynomial, 92>;
            [[maybe_unused]] constexpr auto &dummy93 = tables::crc32_table_n<polynomial, 93>;
            [[maybe_unused]] constexpr auto &dummy94 = tables::crc32_table_n<polynomial, 94>;
            [[maybe_unused]] constexpr auto &dummy95 = tables::crc32_table_n<polynomial, 95>;
            [[maybe_unused]] constexpr auto &dummy96 = tables::crc32_table_n<polynomial, 96>;
            [[maybe_unused]] constexpr auto &dummy97 = tables::crc32_table_n<polynomial, 97>;
            [[maybe_unused]] constexpr auto &dummy98 = tables::crc32_table_n<polynomial, 98>;
            [[maybe_unused]] constexpr auto &dummy99 = tables::crc32_table_n<polynomial, 99>;
            [[maybe_unused]] constexpr auto &dummy100 = tables::crc32_table_n<polynomial, 100>;
            [[maybe_unused]] constexpr auto &dummy101 = tables::crc32_table_n<polynomial, 101>;
            [[maybe_unused]] constexpr auto &dummy102 = tables::crc32_table_n<polynomial, 102>;
            [[maybe_unused]] constexpr auto &dummy103 = tables::crc32_table_n<polynomial, 103>;
            [[maybe_unused]] constexpr auto &dummy104 = tables::crc32_table_n<polynomial, 104>;
            [[maybe_unused]] constexpr auto &dummy105 = tables::crc32_table_n<polynomial, 105>;
            [[maybe_unused]] constexpr auto &dummy106 = tables::crc32_table_n<polynomial, 106>;
            [[maybe_unused]] constexpr auto &dummy107 = tables::crc32_table_n<polynomial, 107>;
            [[maybe_unused]] constexpr auto &dummy108 = tables::crc32_table_n<polynomial, 108>;
            [[maybe_unused]] constexpr auto &dummy109 = tables::crc32_table_n<polynomial, 109>;
            [[maybe_unused]] constexpr auto &dummy110 = tables::crc32_table_n<polynomial, 110>;
            [[maybe_unused]] constexpr auto &dummy111 = tables::crc32_table_n<polynomial, 111>;
#endif

            using namespace arkana::xmm;

            const byte_t* p = static_cast<const byte_t*>(data);

            // process unaligned bytes
            constexpr size_t align_to = alignof(vu32x8);
            if (size_t l = std::min<size_t>(length, static_cast<size_t>(-reinterpret_cast<intptr_t>(p) & (align_to - 1))))
            {
                current = ref::calculate_crc32<polynomial>(p, l, current);
                length -= l;
                p += l;
            }

            // process aligned 128-byte blocks
            constexpr size_t block_size = sizeof(uint32_t) * 8 * 4;
            if (std::size_t block_count = length / block_size)
            {
                vu32x8 v = from_values<vu32x8>(~current, 0, 0, 0, 0, 0, 0, 0);

                while (block_count > 1)
                {
                    vu32x8 s0 = load_s<vu32x8>(reinterpret_cast<const vu32x8*>(p) + 0);
                    vu32x8 s1 = load_s<vu32x8>(reinterpret_cast<const vu32x8*>(p) + 1);
                    vu32x8 s2 = load_s<vu32x8>(reinterpret_cast<const vu32x8*>(p) + 2);
                    vu32x8 s3 = load_s<vu32x8>(reinterpret_cast<const vu32x8*>(p) + 3);
                    p += block_size;
                    prefetch_nta(p); // prefetch next next block

                    // transpose_32x4x4
                    {
                        vu64x4 t0 = reinterpret<vu64x4>(unpack_lo(s0, s1)); // t0 = | 00010203 10111213|04050607 14151617 |
                        vu64x4 t1 = reinterpret<vu64x4>(unpack_hi(s0, s1)); // t1 = | 08090A0B 18191A1B|0C0D0E0F 1C1D1E1F |
                        vu64x4 t2 = reinterpret<vu64x4>(unpack_lo(s2, s3)); // t2 = | 20210223 30313233|24252627 34353637 |
                        vu64x4 t3 = reinterpret<vu64x4>(unpack_hi(s2, s3)); // t3 = | 28290A2B 38393A3B|2C2D2E2F 3C3D3E3F |
                        s0 = reinterpret<vu32x8>(unpack_lo(t0, t2));        // s0 = | 00010203 10111213 20212223 30313233 |
                        s1 = reinterpret<vu32x8>(unpack_hi(t0, t2));        // s1 = | 04050607 14151617 24252627 34353637 |
                        s2 = reinterpret<vu32x8>(unpack_lo(t1, t3));        // s2 = | 08090A0B 18191A1B 28292A2B 38393A3B |
                        s3 = reinterpret<vu32x8>(unpack_hi(t1, t3));        // s3 = | 0C0D0E0F 1C1D1E1F 2C2D2E2F 3C3D3E3F |
                    }

                    s0 ^= v;

                    v =
                        xmm::gather<vu32x8>(tables::crc32_table_n<polynomial, 112 + 0>.data(), extract_byte<3>(s3)) ^
                        xmm::gather<vu32x8>(tables::crc32_table_n<polynomial, 112 + 1>.data(), extract_byte<2>(s3)) ^
                        xmm::gather<vu32x8>(tables::crc32_table_n<polynomial, 112 + 2>.data(), extract_byte<1>(s3)) ^
                        xmm::gather<vu32x8>(tables::crc32_table_n<polynomial, 112 + 3>.data(), extract_byte<0>(s3)) ^
                        xmm::gather<vu32x8>(tables::crc32_table_n<polynomial, 112 + 4>.data(), extract_byte<3>(s2)) ^
                        xmm::gather<vu32x8>(tables::crc32_table_n<polynomial, 112 + 5>.data(), extract_byte<2>(s2)) ^
                        xmm::gather<vu32x8>(tables::crc32_table_n<polynomial, 112 + 6>.data(), extract_byte<1>(s2)) ^
                        xmm::gather<vu32x8>(tables::crc32_table_n<polynomial, 112 + 7>.data(), extract_byte<0>(s2)) ^
                        xmm::gather<vu32x8>(tables::crc32_table_n<polynomial, 112 + 8>.data(), extract_byte<3>(s1)) ^
                        xmm::gather<vu32x8>(tables::crc32_table_n<polynomial, 112 + 9>.data(), extract_byte<2>(s1)) ^
                        xmm::gather<vu32x8>(tables::crc32_table_n<polynomial, 112 + 10>.data(), extract_byte<1>(s1)) ^
                        xmm::gather<vu32x8>(tables::crc32_table_n<polynomial, 112 + 11>.data(), extract_byte<0>(s1)) ^
                        xmm::gather<vu32x8>(tables::crc32_table_n<polynomial, 112 + 12>.data(), extract_byte<3>(s0)) ^
                        xmm::gather<vu32x8>(tables::crc32_table_n<polynomial, 112 + 13>.data(), extract_byte<2>(s0)) ^
                        xmm::gather<vu32x8>(tables::crc32_table_n<polynomial, 112 + 14>.data(), extract_byte<1>(s0)) ^
                        xmm::gather<vu32x8>(tables::crc32_table_n<polynomial, 112 + 15>.data(), extract_byte<0>(s0));

                    --block_count;
                }

                // process last 128-byte block
                {
                    auto block = bit::load_u<std::array<uint32_t, 32>>(p);
                    p += block_size;

                    auto varr = to_array(permute32<0, 4, 1, 5, 2, 6, 3, 7>(v));
                    block[0] ^= varr[0];
                    block[4] ^= varr[1];
                    block[8] ^= varr[2];
                    block[12] ^= varr[3];
                    block[16] ^= varr[4];
                    block[20] ^= varr[5];
                    block[24] ^= varr[6];
                    block[28] ^= varr[7];

                    current = ia32::calculate_crc32<polynomial>(block.data(), sizeof(block), ~0u);
                }

                length %= block_size;
            }

            // process remain bytes
            return ref::calculate_crc32<polynomial>(p, length, current);
        }
    }
}
