/// @file
/// @brief	arkana::sha2
///			- An implementation of SHA-1, SHA-2
/// @author Copyright(c) 2021 ttsuki
/// 
/// This software is released under the MIT License.
/// https://opensource.org/licenses/MIT

#pragma once

#include "../bits/types.h"
#include "../bits/bit-manip.h"
#include "../bits/memory.h"

namespace arkana::sha2
{
    template <size_t bits>
    using sha2_digest_t = byte_array<bits / 8>;

    using md5_digest_t = sha2_digest_t<128>;
    using sha1_digest_t = sha2_digest_t<160>;
    using sha224_digest_t = sha2_digest_t<224>;
    using sha256_digest_t = sha2_digest_t<256>;
    using sha384_digest_t = sha2_digest_t<384>;
    using sha512_digest_t = sha2_digest_t<512>;
    using sha512_224_digest_t = sha2_digest_t<224>;
    using sha512_256_digest_t = sha2_digest_t<256>;

    namespace functions
    {
        template <class T> using vector_t = std::array<T, 8>;
        template <class T> using chunk_t = std::array<T, 16>;

        static constexpr inline vector_t<uint32_t> md5_initial_vector = {0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476};
        static constexpr inline vector_t<uint32_t> sha1_initial_vector = {0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476, 0xC3D2E1F0};
        static constexpr inline vector_t<uint32_t> sha224_initial_vector = {0xc1059ed8, 0x367cd507, 0x3070dd17, 0xf70e5939, 0xffc00b31, 0x68581511, 0x64f98fa7, 0xbefa4fa4};
        static constexpr inline vector_t<uint32_t> sha256_initial_vector = {0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a, 0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19};
        static constexpr inline vector_t<uint64_t> sha384_initial_vector = {0xcbbb9d5dc1059ed8, 0x629a292a367cd507, 0x9159015a3070dd17, 0x152fecd8f70e5939, 0x67332667ffc00b31, 0x8eb44a8768581511, 0xdb0c2e0d64f98fa7, 0x47b5481dbefa4fa4};
        static constexpr inline vector_t<uint64_t> sha512_initial_vector = {0x6a09e667f3bcc908, 0xbb67ae8584caa73b, 0x3c6ef372fe94f82b, 0xa54ff53a5f1d36f1, 0x510e527fade682d1, 0x9b05688c2b3e6c1f, 0x1f83d9abfb41bd6b, 0x5be0cd19137e2179};
        static constexpr inline vector_t<uint64_t> sha512_224_initial_vector = {0x8c3d37c819544da2, 0x73e1996689dcd4d6, 0x1dfab7ae32ff9c82, 0x679dd514582f9fcf, 0x0f6d2b697bd44da8, 0x77e36f7304c48942, 0x3f9d85a86a1d36c8, 0x1112e6ad91d692a1};
        static constexpr inline vector_t<uint64_t> sha512_256_initial_vector = {0x22312194fc2bf72c, 0x9f555fa3c84c64c2, 0x2393b86b6f53b151, 0x963877195940eabd, 0x96283ee2a88effe3, 0xbe5e1e2553863992, 0x2b0199fc2c85b8aa, 0x0eb72ddc81c52ca2};

        struct round_constants_md5
        {
            static inline constexpr uint32_t g[64] = {
                0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, // i * 1 + 0
                1, 6, 11, 0, 5, 10, 15, 4, 9, 14, 3, 8, 13, 2, 7, 12, // i * 5 + 1
                5, 8, 11, 14, 1, 4, 7, 10, 13, 0, 3, 6, 9, 12, 15, 2, // i * 3 + 5
                0, 7, 14, 5, 12, 3, 10, 1, 8, 15, 6, 13, 4, 11, 2, 9, // i * 7 + 0
            };

            static inline constexpr uint32_t s[64] = {
                7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
                5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20,
                4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
                6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21,
            };

            static inline constexpr uint32_t k[64] = {
                0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee, 0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
                0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be, 0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
                0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa, 0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
                0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed, 0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
                0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c, 0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
                0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05, 0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
                0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039, 0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
                0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1, 0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391,
            };
        };

        template <size_t i>
        static constexpr inline void round_md5(std::array<uint32_t, 4>& x, const std::array<uint32_t, 16>& ck) noexcept
        {
            using bits::rotl;

            uint32_t a = x[(0 - i) & 3];
            uint32_t b = x[(1 - i) & 3];
            uint32_t c = x[(2 - i) & 3];
            uint32_t d = x[(3 - i) & 3];

            constexpr uint32_t g = round_constants_md5::g[i];
            constexpr uint32_t s = round_constants_md5::s[i];
            constexpr uint32_t k = round_constants_md5::k[i];

            if constexpr (!(i < 64)) static_assert(i < 64);
            else if constexpr (i < 16) a = b + rotl(a + ck[g] + k + ((b & c) | (~b & d)), s);
            else if constexpr (i < 32) a = b + rotl(a + ck[g] + k + ((b & d) | (c & ~d)), s);
            else if constexpr (i < 48) a = b + rotl(a + ck[g] + k + (b ^ c ^ d), s);
            else if constexpr (i < 64) a = b + rotl(a + ck[g] + k + (c ^ (b | ~d)), s);

            x[(0 - i) & 3] = a;
        }


        static void process_chunk_md5(vector_t<uint32_t>& vec, chunk_t<uint32_t> ck) noexcept
        {
            std::array<uint32_t, 4> x{vec[0], vec[1], vec[2], vec[3]};

            round_md5<0x00>(x, ck);
            round_md5<0x01>(x, ck);
            round_md5<0x02>(x, ck);
            round_md5<0x03>(x, ck);
            round_md5<0x04>(x, ck);
            round_md5<0x05>(x, ck);
            round_md5<0x06>(x, ck);
            round_md5<0x07>(x, ck);
            round_md5<0x08>(x, ck);
            round_md5<0x09>(x, ck);
            round_md5<0x0A>(x, ck);
            round_md5<0x0B>(x, ck);
            round_md5<0x0C>(x, ck);
            round_md5<0x0D>(x, ck);
            round_md5<0x0E>(x, ck);
            round_md5<0x0F>(x, ck);
            round_md5<0x10>(x, ck);
            round_md5<0x11>(x, ck);
            round_md5<0x12>(x, ck);
            round_md5<0x13>(x, ck);
            round_md5<0x14>(x, ck);
            round_md5<0x15>(x, ck);
            round_md5<0x16>(x, ck);
            round_md5<0x17>(x, ck);
            round_md5<0x18>(x, ck);
            round_md5<0x19>(x, ck);
            round_md5<0x1A>(x, ck);
            round_md5<0x1B>(x, ck);
            round_md5<0x1C>(x, ck);
            round_md5<0x1D>(x, ck);
            round_md5<0x1E>(x, ck);
            round_md5<0x1F>(x, ck);
            round_md5<0x20>(x, ck);
            round_md5<0x21>(x, ck);
            round_md5<0x22>(x, ck);
            round_md5<0x23>(x, ck);
            round_md5<0x24>(x, ck);
            round_md5<0x25>(x, ck);
            round_md5<0x26>(x, ck);
            round_md5<0x27>(x, ck);
            round_md5<0x28>(x, ck);
            round_md5<0x29>(x, ck);
            round_md5<0x2A>(x, ck);
            round_md5<0x2B>(x, ck);
            round_md5<0x2C>(x, ck);
            round_md5<0x2D>(x, ck);
            round_md5<0x2E>(x, ck);
            round_md5<0x2F>(x, ck);
            round_md5<0x30>(x, ck);
            round_md5<0x31>(x, ck);
            round_md5<0x32>(x, ck);
            round_md5<0x33>(x, ck);
            round_md5<0x34>(x, ck);
            round_md5<0x35>(x, ck);
            round_md5<0x36>(x, ck);
            round_md5<0x37>(x, ck);
            round_md5<0x38>(x, ck);
            round_md5<0x39>(x, ck);
            round_md5<0x3A>(x, ck);
            round_md5<0x3B>(x, ck);
            round_md5<0x3C>(x, ck);
            round_md5<0x3D>(x, ck);
            round_md5<0x3E>(x, ck);
            round_md5<0x3F>(x, ck);

            vec[0] += x[0];
            vec[1] += x[1];
            vec[2] += x[2];
            vec[3] += x[3];
        }

        struct round_constants_sha1
        {
            static inline constexpr uint32_t k[4] = {
                0x5A827999, 0x6ED9EBA1, 0x8F1BBCDC, 0xCA62C1D6
            };
        };

        template <size_t i, class T>
        static inline constexpr void round_sha1(vector_t<T>& vec, T kwi) noexcept
        {
            using bits::rotl;
            static_assert(i < 80);

            T a = vec[(80 - i) % 5];
            T b = vec[(81 - i) % 5];
            T c = vec[(82 - i) % 5];
            T d = vec[(83 - i) % 5];
            T e = vec[(84 - i) % 5];

            T f = 0;
            if constexpr (i < 20) f = (b & c) | (~b & d);
            else if constexpr (i < 40) f = b ^ c ^ d;
            else if constexpr (i < 60) f = (b & c) | (b & d) | (c & d);
            else f = b ^ c ^ d;

            vec[(81 - i) % 5] = rotl(b, 30);
            vec[(84 - i) % 5] = rotl(a, 5) + f + e + kwi;
        }

        template <size_t i, class T>
        static inline constexpr uint32_t rotate_sha1(std::array<T, 20>& ck) noexcept
        {
            using bits::rotl;
            static_assert(i < 20);
            T w16 = ck[(i - 16 + 20) % 20];
            T w14 = ck[(i - 14 + 20) % 20];
            T w8 = ck[(i - 8 + 20) % 20];
            T w3 = ck[(i - 3 + 20) % 20];
            return ck[i % 20] = rotl(w3 ^ w8 ^ w14 ^ w16, 1);
        }

        static constexpr void process_chunk_sha1(vector_t<uint32_t>& vec, const chunk_t<uint32_t>& input) noexcept
        {
            using bits::byteswap;

            vector_t<uint32_t> x = vec;
            std::array<uint32_t, 20> ck{};

            round_sha1<00 + 0>(x, round_constants_sha1::k[0] + (ck[0x0] = byteswap(input[0x0])));
            round_sha1<00 + 1>(x, round_constants_sha1::k[0] + (ck[0x1] = byteswap(input[0x1])));
            round_sha1<00 + 2>(x, round_constants_sha1::k[0] + (ck[0x2] = byteswap(input[0x2])));
            round_sha1<00 + 3>(x, round_constants_sha1::k[0] + (ck[0x3] = byteswap(input[0x3])));
            round_sha1<00 + 4>(x, round_constants_sha1::k[0] + (ck[0x4] = byteswap(input[0x4])));
            round_sha1<00 + 0>(x, round_constants_sha1::k[0] + (ck[0x5] = byteswap(input[0x5])));
            round_sha1<00 + 1>(x, round_constants_sha1::k[0] + (ck[0x6] = byteswap(input[0x6])));
            round_sha1<00 + 2>(x, round_constants_sha1::k[0] + (ck[0x7] = byteswap(input[0x7])));
            round_sha1<00 + 3>(x, round_constants_sha1::k[0] + (ck[0x8] = byteswap(input[0x8])));
            round_sha1<00 + 4>(x, round_constants_sha1::k[0] + (ck[0x9] = byteswap(input[0x9])));
            round_sha1<00 + 0>(x, round_constants_sha1::k[0] + (ck[0xA] = byteswap(input[0xA])));
            round_sha1<00 + 1>(x, round_constants_sha1::k[0] + (ck[0xB] = byteswap(input[0xB])));
            round_sha1<00 + 2>(x, round_constants_sha1::k[0] + (ck[0xC] = byteswap(input[0xC])));
            round_sha1<00 + 3>(x, round_constants_sha1::k[0] + (ck[0xD] = byteswap(input[0xD])));
            round_sha1<00 + 4>(x, round_constants_sha1::k[0] + (ck[0xE] = byteswap(input[0xE])));
            round_sha1<00 + 0>(x, round_constants_sha1::k[0] + (ck[0xF] = byteswap(input[0xF])));
            round_sha1<00 + 1>(x, round_constants_sha1::k[0] + rotate_sha1<16>(ck));
            round_sha1<00 + 2>(x, round_constants_sha1::k[0] + rotate_sha1<17>(ck));
            round_sha1<00 + 3>(x, round_constants_sha1::k[0] + rotate_sha1<18>(ck));
            round_sha1<00 + 4>(x, round_constants_sha1::k[0] + rotate_sha1<19>(ck));

            round_sha1<20 + 0>(x, round_constants_sha1::k[1] + rotate_sha1<0>(ck));
            round_sha1<20 + 1>(x, round_constants_sha1::k[1] + rotate_sha1<1>(ck));
            round_sha1<20 + 2>(x, round_constants_sha1::k[1] + rotate_sha1<2>(ck));
            round_sha1<20 + 3>(x, round_constants_sha1::k[1] + rotate_sha1<3>(ck));
            round_sha1<20 + 4>(x, round_constants_sha1::k[1] + rotate_sha1<4>(ck));
            round_sha1<20 + 0>(x, round_constants_sha1::k[1] + rotate_sha1<5>(ck));
            round_sha1<20 + 1>(x, round_constants_sha1::k[1] + rotate_sha1<6>(ck));
            round_sha1<20 + 2>(x, round_constants_sha1::k[1] + rotate_sha1<7>(ck));
            round_sha1<20 + 3>(x, round_constants_sha1::k[1] + rotate_sha1<8>(ck));
            round_sha1<20 + 4>(x, round_constants_sha1::k[1] + rotate_sha1<9>(ck));
            round_sha1<20 + 0>(x, round_constants_sha1::k[1] + rotate_sha1<10>(ck));
            round_sha1<20 + 1>(x, round_constants_sha1::k[1] + rotate_sha1<11>(ck));
            round_sha1<20 + 2>(x, round_constants_sha1::k[1] + rotate_sha1<12>(ck));
            round_sha1<20 + 3>(x, round_constants_sha1::k[1] + rotate_sha1<13>(ck));
            round_sha1<20 + 4>(x, round_constants_sha1::k[1] + rotate_sha1<14>(ck));
            round_sha1<20 + 0>(x, round_constants_sha1::k[1] + rotate_sha1<15>(ck));
            round_sha1<20 + 1>(x, round_constants_sha1::k[1] + rotate_sha1<16>(ck));
            round_sha1<20 + 2>(x, round_constants_sha1::k[1] + rotate_sha1<17>(ck));
            round_sha1<20 + 3>(x, round_constants_sha1::k[1] + rotate_sha1<18>(ck));
            round_sha1<20 + 4>(x, round_constants_sha1::k[1] + rotate_sha1<19>(ck));

            round_sha1<40 + 0>(x, round_constants_sha1::k[2] + rotate_sha1<0>(ck));
            round_sha1<40 + 1>(x, round_constants_sha1::k[2] + rotate_sha1<1>(ck));
            round_sha1<40 + 2>(x, round_constants_sha1::k[2] + rotate_sha1<2>(ck));
            round_sha1<40 + 3>(x, round_constants_sha1::k[2] + rotate_sha1<3>(ck));
            round_sha1<40 + 4>(x, round_constants_sha1::k[2] + rotate_sha1<4>(ck));
            round_sha1<40 + 0>(x, round_constants_sha1::k[2] + rotate_sha1<5>(ck));
            round_sha1<40 + 1>(x, round_constants_sha1::k[2] + rotate_sha1<6>(ck));
            round_sha1<40 + 2>(x, round_constants_sha1::k[2] + rotate_sha1<7>(ck));
            round_sha1<40 + 3>(x, round_constants_sha1::k[2] + rotate_sha1<8>(ck));
            round_sha1<40 + 4>(x, round_constants_sha1::k[2] + rotate_sha1<9>(ck));
            round_sha1<40 + 0>(x, round_constants_sha1::k[2] + rotate_sha1<10>(ck));
            round_sha1<40 + 1>(x, round_constants_sha1::k[2] + rotate_sha1<11>(ck));
            round_sha1<40 + 2>(x, round_constants_sha1::k[2] + rotate_sha1<12>(ck));
            round_sha1<40 + 3>(x, round_constants_sha1::k[2] + rotate_sha1<13>(ck));
            round_sha1<40 + 4>(x, round_constants_sha1::k[2] + rotate_sha1<14>(ck));
            round_sha1<40 + 0>(x, round_constants_sha1::k[2] + rotate_sha1<15>(ck));
            round_sha1<40 + 1>(x, round_constants_sha1::k[2] + rotate_sha1<16>(ck));
            round_sha1<40 + 2>(x, round_constants_sha1::k[2] + rotate_sha1<17>(ck));
            round_sha1<40 + 3>(x, round_constants_sha1::k[2] + rotate_sha1<18>(ck));
            round_sha1<40 + 4>(x, round_constants_sha1::k[2] + rotate_sha1<19>(ck));

            round_sha1<60 + 0>(x, round_constants_sha1::k[3] + rotate_sha1<0>(ck));
            round_sha1<60 + 1>(x, round_constants_sha1::k[3] + rotate_sha1<1>(ck));
            round_sha1<60 + 2>(x, round_constants_sha1::k[3] + rotate_sha1<2>(ck));
            round_sha1<60 + 3>(x, round_constants_sha1::k[3] + rotate_sha1<3>(ck));
            round_sha1<60 + 4>(x, round_constants_sha1::k[3] + rotate_sha1<4>(ck));
            round_sha1<60 + 0>(x, round_constants_sha1::k[3] + rotate_sha1<5>(ck));
            round_sha1<60 + 1>(x, round_constants_sha1::k[3] + rotate_sha1<6>(ck));
            round_sha1<60 + 2>(x, round_constants_sha1::k[3] + rotate_sha1<7>(ck));
            round_sha1<60 + 3>(x, round_constants_sha1::k[3] + rotate_sha1<8>(ck));
            round_sha1<60 + 4>(x, round_constants_sha1::k[3] + rotate_sha1<9>(ck));
            round_sha1<60 + 0>(x, round_constants_sha1::k[3] + rotate_sha1<10>(ck));
            round_sha1<60 + 1>(x, round_constants_sha1::k[3] + rotate_sha1<11>(ck));
            round_sha1<60 + 2>(x, round_constants_sha1::k[3] + rotate_sha1<12>(ck));
            round_sha1<60 + 3>(x, round_constants_sha1::k[3] + rotate_sha1<13>(ck));
            round_sha1<60 + 4>(x, round_constants_sha1::k[3] + rotate_sha1<14>(ck));
            round_sha1<60 + 0>(x, round_constants_sha1::k[3] + rotate_sha1<15>(ck));
            round_sha1<60 + 1>(x, round_constants_sha1::k[3] + rotate_sha1<16>(ck));
            round_sha1<60 + 2>(x, round_constants_sha1::k[3] + rotate_sha1<17>(ck));
            round_sha1<60 + 3>(x, round_constants_sha1::k[3] + rotate_sha1<18>(ck));
            round_sha1<60 + 4>(x, round_constants_sha1::k[3] + rotate_sha1<19>(ck));

            vec[0] += x[0];
            vec[1] += x[1];
            vec[2] += x[2];
            vec[3] += x[3];
            vec[4] += x[4];
        }

        struct round_constants_sha256
        {
            static constexpr inline size_t rounds = 64;
            static constexpr inline uint32_t constants[rounds] = {
                0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
                0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
                0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
                0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
                0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
                0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
                0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
                0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2,
            };
        };

        struct round_constants_sha512
        {
            static constexpr inline size_t rounds = 80;
            static constexpr inline uint64_t constants[rounds] = {
                0x428a2f98d728ae22, 0x7137449123ef65cd, 0xb5c0fbcfec4d3b2f, 0xe9b5dba58189dbbc,
                0x3956c25bf348b538, 0x59f111f1b605d019, 0x923f82a4af194f9b, 0xab1c5ed5da6d8118,
                0xd807aa98a3030242, 0x12835b0145706fbe, 0x243185be4ee4b28c, 0x550c7dc3d5ffb4e2,
                0x72be5d74f27b896f, 0x80deb1fe3b1696b1, 0x9bdc06a725c71235, 0xc19bf174cf692694,
                0xe49b69c19ef14ad2, 0xefbe4786384f25e3, 0x0fc19dc68b8cd5b5, 0x240ca1cc77ac9c65,
                0x2de92c6f592b0275, 0x4a7484aa6ea6e483, 0x5cb0a9dcbd41fbd4, 0x76f988da831153b5,
                0x983e5152ee66dfab, 0xa831c66d2db43210, 0xb00327c898fb213f, 0xbf597fc7beef0ee4,
                0xc6e00bf33da88fc2, 0xd5a79147930aa725, 0x06ca6351e003826f, 0x142929670a0e6e70,
                0x27b70a8546d22ffc, 0x2e1b21385c26c926, 0x4d2c6dfc5ac42aed, 0x53380d139d95b3df,
                0x650a73548baf63de, 0x766a0abb3c77b2a8, 0x81c2c92e47edaee6, 0x92722c851482353b,
                0xa2bfe8a14cf10364, 0xa81a664bbc423001, 0xc24b8b70d0f89791, 0xc76c51a30654be30,
                0xd192e819d6ef5218, 0xd69906245565a910, 0xf40e35855771202a, 0x106aa07032bbd1b8,
                0x19a4c116b8d2d0c8, 0x1e376c085141ab53, 0x2748774cdf8eeb99, 0x34b0bcb5e19b48a8,
                0x391c0cb3c5c95a63, 0x4ed8aa4ae3418acb, 0x5b9cca4f7763e373, 0x682e6ff3d6b2b8a3,
                0x748f82ee5defb2fc, 0x78a5636f43172f60, 0x84c87814a1f0ab72, 0x8cc702081a6439ec,
                0x90befffa23631e28, 0xa4506cebde82bde9, 0xbef9a3f7b2c67915, 0xc67178f2e372532b,
                0xca273eceea26619c, 0xd186b8c721c0c207, 0xeada7dd6cde0eb1e, 0xf57d4f7fee6ed178,
                0x06f067aa72176fba, 0x0a637dc5a2c898a6, 0x113f9804bef90dae, 0x1b710b35131c471b,
                0x28db77f523047d84, 0x32caab7b40c72493, 0x3c9ebe0a15c9bebc, 0x431d67c49c100d4c,
                0x4cc5d4becb3e42b6, 0x597f299cfc657e2a, 0x5fcb6fab3ad6faec, 0x6c44198c4a475817,
            };
        };

        template <int S00, int S01, int S02, int S10, int S11, int S12, size_t i, class T>
        static inline constexpr void round_sha2(vector_t<T>& vec, T kwi) noexcept
        {
            using bits::rotr;

            T a = vec[(0 - i) & 7];
            T b = vec[(1 - i) & 7];
            T c = vec[(2 - i) & 7];
            T d = vec[(3 - i) & 7];
            T e = vec[(4 - i) & 7];
            T f = vec[(5 - i) & 7];
            T g = vec[(6 - i) & 7];
            T h = vec[(7 - i) & 7];

            T S0 = rotr(a, S00) ^ rotr(a, S01) ^ rotr(a, S02);
            T S1 = rotr(e, S10) ^ rotr(e, S11) ^ rotr(e, S12);
            T ch = (e & f) ^ (~e & g);
            T maj = (a & b) ^ (a & c) ^ (b & c);

            h += kwi + ch + S1;
            d += h;
            h += maj + S0;

            vec[(3 - i) & 7] = d;
            vec[(7 - i) & 7] = h;
        }

        template <int s00, int s01, int s02, int s10, int s11, int s12, size_t i, class T>
        static inline constexpr T rotate_sha2(chunk_t<T>& ck) noexcept
        {
            using bits::rotr;

            T w15 = ck[(i - 15) & 15];
            T w7 = ck[(i - 7) & 15];
            T w2 = ck[(i - 2) & 15];
            T s0 = rotr(w15, s00) ^ rotr(w15, s01) ^ w15 >> s02;
            T s1 = rotr(w2, s10) ^ rotr(w2, s11) ^ w2 >> s12;
            return ck[i & 15] += s0 + w7 + s1;
        }

        template <class T, class round_constants,
                  int s00, int s01, int s02, int s10, int s11, int s12,
                  int S00, int S01, int S02, int S10, int S11, int S12>
        static constexpr void process_chunk_sha2(vector_t<T>& vec, const chunk_t<T>& input) noexcept
        {
            using bits::byteswap;

            vector_t<T> x = vec;
            chunk_t<T> ck{};

            {
                auto k = round_constants::constants;
                round_sha2<S00, S01, S02, S10, S11, S12, 0>(x, k[0x0] + (ck[0x0] = byteswap(input[0x0])));
                round_sha2<S00, S01, S02, S10, S11, S12, 1>(x, k[0x1] + (ck[0x1] = byteswap(input[0x1])));
                round_sha2<S00, S01, S02, S10, S11, S12, 2>(x, k[0x2] + (ck[0x2] = byteswap(input[0x2])));
                round_sha2<S00, S01, S02, S10, S11, S12, 3>(x, k[0x3] + (ck[0x3] = byteswap(input[0x3])));
                round_sha2<S00, S01, S02, S10, S11, S12, 4>(x, k[0x4] + (ck[0x4] = byteswap(input[0x4])));
                round_sha2<S00, S01, S02, S10, S11, S12, 5>(x, k[0x5] + (ck[0x5] = byteswap(input[0x5])));
                round_sha2<S00, S01, S02, S10, S11, S12, 6>(x, k[0x6] + (ck[0x6] = byteswap(input[0x6])));
                round_sha2<S00, S01, S02, S10, S11, S12, 7>(x, k[0x7] + (ck[0x7] = byteswap(input[0x7])));
                round_sha2<S00, S01, S02, S10, S11, S12, 0>(x, k[0x8] + (ck[0x8] = byteswap(input[0x8])));
                round_sha2<S00, S01, S02, S10, S11, S12, 1>(x, k[0x9] + (ck[0x9] = byteswap(input[0x9])));
                round_sha2<S00, S01, S02, S10, S11, S12, 2>(x, k[0xA] + (ck[0xA] = byteswap(input[0xA])));
                round_sha2<S00, S01, S02, S10, S11, S12, 3>(x, k[0xB] + (ck[0xB] = byteswap(input[0xB])));
                round_sha2<S00, S01, S02, S10, S11, S12, 4>(x, k[0xC] + (ck[0xC] = byteswap(input[0xC])));
                round_sha2<S00, S01, S02, S10, S11, S12, 5>(x, k[0xD] + (ck[0xD] = byteswap(input[0xD])));
                round_sha2<S00, S01, S02, S10, S11, S12, 6>(x, k[0xE] + (ck[0xE] = byteswap(input[0xE])));
                round_sha2<S00, S01, S02, S10, S11, S12, 7>(x, k[0xF] + (ck[0xF] = byteswap(input[0xF])));
            }

            for (size_t i = 1; i < round_constants::rounds / 16; i++)
            {
                auto k = round_constants::constants + i * 0x10;
                round_sha2<S00, S01, S02, S10, S11, S12, 0>(x, k[0x0] + rotate_sha2<s00, s01, s02, s10, s11, s12, 0x0>(ck));
                round_sha2<S00, S01, S02, S10, S11, S12, 1>(x, k[0x1] + rotate_sha2<s00, s01, s02, s10, s11, s12, 0x1>(ck));
                round_sha2<S00, S01, S02, S10, S11, S12, 2>(x, k[0x2] + rotate_sha2<s00, s01, s02, s10, s11, s12, 0x2>(ck));
                round_sha2<S00, S01, S02, S10, S11, S12, 3>(x, k[0x3] + rotate_sha2<s00, s01, s02, s10, s11, s12, 0x3>(ck));
                round_sha2<S00, S01, S02, S10, S11, S12, 4>(x, k[0x4] + rotate_sha2<s00, s01, s02, s10, s11, s12, 0x4>(ck));
                round_sha2<S00, S01, S02, S10, S11, S12, 5>(x, k[0x5] + rotate_sha2<s00, s01, s02, s10, s11, s12, 0x5>(ck));
                round_sha2<S00, S01, S02, S10, S11, S12, 6>(x, k[0x6] + rotate_sha2<s00, s01, s02, s10, s11, s12, 0x6>(ck));
                round_sha2<S00, S01, S02, S10, S11, S12, 7>(x, k[0x7] + rotate_sha2<s00, s01, s02, s10, s11, s12, 0x7>(ck));
                round_sha2<S00, S01, S02, S10, S11, S12, 0>(x, k[0x8] + rotate_sha2<s00, s01, s02, s10, s11, s12, 0x8>(ck));
                round_sha2<S00, S01, S02, S10, S11, S12, 1>(x, k[0x9] + rotate_sha2<s00, s01, s02, s10, s11, s12, 0x9>(ck));
                round_sha2<S00, S01, S02, S10, S11, S12, 2>(x, k[0xA] + rotate_sha2<s00, s01, s02, s10, s11, s12, 0xA>(ck));
                round_sha2<S00, S01, S02, S10, S11, S12, 3>(x, k[0xB] + rotate_sha2<s00, s01, s02, s10, s11, s12, 0xB>(ck));
                round_sha2<S00, S01, S02, S10, S11, S12, 4>(x, k[0xC] + rotate_sha2<s00, s01, s02, s10, s11, s12, 0xC>(ck));
                round_sha2<S00, S01, S02, S10, S11, S12, 5>(x, k[0xD] + rotate_sha2<s00, s01, s02, s10, s11, s12, 0xD>(ck));
                round_sha2<S00, S01, S02, S10, S11, S12, 6>(x, k[0xE] + rotate_sha2<s00, s01, s02, s10, s11, s12, 0xE>(ck));
                round_sha2<S00, S01, S02, S10, S11, S12, 7>(x, k[0xF] + rotate_sha2<s00, s01, s02, s10, s11, s12, 0xF>(ck));
            }

            vec[0] += x[0];
            vec[1] += x[1];
            vec[2] += x[2];
            vec[3] += x[3];
            vec[4] += x[4];
            vec[5] += x[5];
            vec[6] += x[6];
            vec[7] += x[7];
        }

        static inline constexpr void process_chunk_sha256(vector_t<uint32_t>& v, const chunk_t<uint32_t>& input)
        {
            return process_chunk_sha2<uint32_t, round_constants_sha256, 7, 18, 3, 17, 19, 10, 2, 13, 22, 6, 11, 25>(v, input);
        }

        static inline constexpr void process_chunk_sha512(vector_t<uint64_t>& v, const chunk_t<uint64_t>& input)
        {
            return process_chunk_sha2<uint64_t, round_constants_sha512, 1, 8, 7, 19, 61, 6, 28, 34, 39, 14, 18, 41>(v, input);
        }

        inline namespace context
        {
            template <class T, class TDigest>
            struct sha2_state_t
            {
                using unit_t = T;
                using digest_t = TDigest;

                vector_t<T> vec;
                chunk_t<T> input;
                uintmax_t wrote;
            };

            using md5_state_t = sha2_state_t<uint32_t, md5_digest_t>;
            using sha1_state_t = sha2_state_t<uint32_t, sha1_digest_t>;
            using sha224_state_t = sha2_state_t<uint32_t, sha224_digest_t>;
            using sha256_state_t = sha2_state_t<uint32_t, sha256_digest_t>;
            using sha384_state_t = sha2_state_t<uint64_t, sha384_digest_t>;
            using sha512_state_t = sha2_state_t<uint64_t, sha512_digest_t>;
            using sha512_224_state_t = sha2_state_t<uint64_t, sha512_224_digest_t>;
            using sha512_256_state_t = sha2_state_t<uint64_t, sha512_256_digest_t>;

            static inline constexpr md5_state_t create_md5_state() { return {md5_initial_vector, {}, 0}; }
            static inline constexpr sha1_state_t create_sha1_state() { return {sha1_initial_vector, {}, 0}; }
            static inline constexpr sha224_state_t create_sha224_state() { return {sha224_initial_vector, {}, 0}; }
            static inline constexpr sha256_state_t create_sha256_state() { return {sha256_initial_vector, {}, 0}; }
            static inline constexpr sha384_state_t create_sha384_state() { return {sha384_initial_vector, {}, 0}; }
            static inline constexpr sha512_state_t create_sha512_state() { return {sha512_initial_vector, {}, 0}; }
            static inline constexpr sha512_224_state_t create_sha512_224_state() { return {sha512_224_initial_vector, {}, 0}; }
            static inline constexpr sha512_256_state_t create_sha512_256_state() { return {sha512_256_initial_vector, {}, 0}; }

            template <class sha2_state_t = sha256_state_t, auto process_chunk = functions::process_chunk_sha256>
            static void process_bytes(sha2_state_t& stt, const void* data, size_t len) noexcept
            {
                const byte_t* src = static_cast<const byte_t*>(data);

                while (true)
                {
                    byte_t* const buf = reinterpret_cast<byte_t*>(stt.input.data());
                    size_t constexpr buf_sz = sizeof(stt.input);
                    size_t used = stt.wrote % buf_sz;
                    size_t sz = std::min(buf_sz - used, len);

                    memcpy(buf + used, src, sz);

                    len -= sz;
                    src += sz;
                    used += sz;
                    stt.wrote += sz;

                    if (used != buf_sz) break;

                    process_chunk(stt.vec, stt.input);
                }
            }

            template <class sha2_state_t = sha256_state_t, auto process_chunk = functions::process_chunk_sha256, bool big_endian = true>
            static auto finalize_and_get_digest(sha2_state_t& stt) noexcept -> typename sha2_state_t::digest_t
            {
                using T = typename sha2_state_t::unit_t;
                using digest_t = typename sha2_state_t::digest_t;
                using bits::byteswap;

                byte_t* const buf = reinterpret_cast<byte_t*>(stt.input.data());
                size_t constexpr buf_sz = sizeof(stt.input);
                size_t used = stt.wrote % sizeof(stt.input);

                // appends '1' and pads buffer
                buf[used++] = static_cast<byte_t>(0x80);
                memset(buf + used, 0, buf_sz - used);

                // if there is no buffer for message length, processes current chunk
                if (sizeof(stt.input) - used < sizeof(T) * 2)
                {
                    process_chunk(stt.vec, stt.input);
                    stt.input = {};
                }

                // processes last chunk
                {
                    // puts message length
                    if constexpr (big_endian)
                    {
                        stt.input[14] = byteswap(static_cast<T>(stt.wrote >> (sizeof(T) * 8 - 3))); // higher
                        stt.input[15] = byteswap(static_cast<T>(stt.wrote << 3));                   // lower
                    }
                    else
                    {
                        stt.input[14] = (static_cast<T>(stt.wrote << 3));                   // lower
                        stt.input[15] = (static_cast<T>(stt.wrote >> (sizeof(T) * 8 - 3))); // higher
                    }
                    process_chunk(stt.vec, stt.input);
                }

                // to digest value
                {
                    vector_t<T> vec = stt.vec;
                    if constexpr (big_endian)
                    {
                        for (auto&& i : vec) { i = byteswap(i); }
                    }

                    digest_t digest{};
                    memcpy(&digest, &vec, sizeof(digest));
                    return digest;
                }
            }
        }
    }

    namespace ref
    {
        using functions::md5_state_t;
        using functions::sha1_state_t;
        using functions::sha224_state_t;
        using functions::sha256_state_t;
        using functions::sha384_state_t;
        using functions::sha512_state_t;
        using functions::sha512_224_state_t;
        using functions::sha512_256_state_t;
        using functions::create_md5_state;
        using functions::create_sha1_state;
        using functions::create_sha224_state;
        using functions::create_sha256_state;
        using functions::create_sha384_state;
        using functions::create_sha512_state;
        using functions::create_sha512_224_state;
        using functions::create_sha512_256_state;
        static inline void process_bytes(md5_state_t& stt, const void* data, size_t len) noexcept { return functions::process_bytes<md5_state_t, functions::process_chunk_md5>(stt, data, len); }
        static inline void process_bytes(sha1_state_t& stt, const void* data, size_t len) noexcept { return functions::process_bytes<sha1_state_t, functions::process_chunk_sha1>(stt, data, len); }
        static inline void process_bytes(sha224_state_t& stt, const void* data, size_t len) noexcept { return functions::process_bytes<sha224_state_t, functions::process_chunk_sha256>(stt, data, len); }
        static inline void process_bytes(sha256_state_t& stt, const void* data, size_t len) noexcept { return functions::process_bytes<sha256_state_t, functions::process_chunk_sha256>(stt, data, len); }
        static inline void process_bytes(sha384_state_t& stt, const void* data, size_t len) noexcept { return functions::process_bytes<sha384_state_t, functions::process_chunk_sha512>(stt, data, len); }
        static inline void process_bytes(sha512_state_t& stt, const void* data, size_t len) noexcept { return functions::process_bytes<sha512_state_t, functions::process_chunk_sha512>(stt, data, len); }
        static inline void process_bytes(sha512_224_state_t& stt, const void* data, size_t len) noexcept { return functions::process_bytes<sha512_224_state_t, functions::process_chunk_sha512>(stt, data, len); }
        static inline void process_bytes(sha512_256_state_t& stt, const void* data, size_t len) noexcept { return functions::process_bytes<sha512_256_state_t, functions::process_chunk_sha512>(stt, data, len); }
        static inline md5_digest_t finalize_and_get_digest(md5_state_t& stt) noexcept { return functions::finalize_and_get_digest<md5_state_t, functions::process_chunk_md5, false>(stt); }
        static inline sha1_digest_t finalize_and_get_digest(sha1_state_t& stt) noexcept { return functions::finalize_and_get_digest<sha1_state_t, functions::process_chunk_sha1>(stt); }
        static inline sha224_digest_t finalize_and_get_digest(sha224_state_t& stt) noexcept { return functions::finalize_and_get_digest<sha224_state_t, functions::process_chunk_sha256>(stt); }
        static inline sha256_digest_t finalize_and_get_digest(sha256_state_t& stt) noexcept { return functions::finalize_and_get_digest<sha256_state_t, functions::process_chunk_sha256>(stt); }
        static inline sha384_digest_t finalize_and_get_digest(sha384_state_t& stt) noexcept { return functions::finalize_and_get_digest<sha384_state_t, functions::process_chunk_sha512>(stt); }
        static inline sha512_digest_t finalize_and_get_digest(sha512_state_t& stt) noexcept { return functions::finalize_and_get_digest<sha512_state_t, functions::process_chunk_sha512>(stt); }
        static inline sha512_224_digest_t finalize_and_get_digest(sha512_224_state_t& stt) noexcept { return functions::finalize_and_get_digest<sha512_224_state_t, functions::process_chunk_sha512>(stt); }
        static inline sha512_256_digest_t finalize_and_get_digest(sha512_256_state_t& stt) noexcept { return functions::finalize_and_get_digest<sha512_256_state_t, functions::process_chunk_sha512>(stt); }
    }

    using namespace ref;
}
