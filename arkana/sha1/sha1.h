/// @file
/// @brief	arkana::sha1
///			- An implementation of SHA-1
/// @author Copyright(c) 2021 ttsuki
/// 
/// This software is released under the MIT License.
/// https://opensource.org/licenses/MIT

#pragma once

#include "../bits/types.h"
#include "../bits/bit-manip.h"
#include "../bits/memory.h"

namespace arkana::sha1
{
    using sha1_digest_t = byte_array<160 / 8>;

    namespace functions
    {
        template <class T> using vector_t = std::array<T, 8>;
        template <class T> using chunk_t = std::array<T, 16>;

        static constexpr inline vector_t<uint32_t> sha1_initial_vector = {0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476, 0xC3D2E1F0};

        template <size_t i, class T>
        static inline constexpr void compress_sha1(vector_t<T>& vec, T kwi) noexcept
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

        static constexpr void process_chunk_sha1(vector_t<uint32_t>& vec, chunk_t<uint32_t> input) noexcept
        {
            using bits::byteswap;

            vector_t<uint32_t> x = vec;
            std::array<uint32_t, 20> ck{};

            compress_sha1<00 + 0>(x, 0x5A827999 + (ck[0x0] = byteswap(input[0x0])));
            compress_sha1<00 + 1>(x, 0x5A827999 + (ck[0x1] = byteswap(input[0x1])));
            compress_sha1<00 + 2>(x, 0x5A827999 + (ck[0x2] = byteswap(input[0x2])));
            compress_sha1<00 + 3>(x, 0x5A827999 + (ck[0x3] = byteswap(input[0x3])));
            compress_sha1<00 + 4>(x, 0x5A827999 + (ck[0x4] = byteswap(input[0x4])));
            compress_sha1<00 + 0>(x, 0x5A827999 + (ck[0x5] = byteswap(input[0x5])));
            compress_sha1<00 + 1>(x, 0x5A827999 + (ck[0x6] = byteswap(input[0x6])));
            compress_sha1<00 + 2>(x, 0x5A827999 + (ck[0x7] = byteswap(input[0x7])));
            compress_sha1<00 + 3>(x, 0x5A827999 + (ck[0x8] = byteswap(input[0x8])));
            compress_sha1<00 + 4>(x, 0x5A827999 + (ck[0x9] = byteswap(input[0x9])));
            compress_sha1<00 + 0>(x, 0x5A827999 + (ck[0xA] = byteswap(input[0xA])));
            compress_sha1<00 + 1>(x, 0x5A827999 + (ck[0xB] = byteswap(input[0xB])));
            compress_sha1<00 + 2>(x, 0x5A827999 + (ck[0xC] = byteswap(input[0xC])));
            compress_sha1<00 + 3>(x, 0x5A827999 + (ck[0xD] = byteswap(input[0xD])));
            compress_sha1<00 + 4>(x, 0x5A827999 + (ck[0xE] = byteswap(input[0xE])));
            compress_sha1<00 + 0>(x, 0x5A827999 + (ck[0xF] = byteswap(input[0xF])));
            compress_sha1<00 + 1>(x, 0x5A827999 + rotate_sha1<16>(ck));
            compress_sha1<00 + 2>(x, 0x5A827999 + rotate_sha1<17>(ck));
            compress_sha1<00 + 3>(x, 0x5A827999 + rotate_sha1<18>(ck));
            compress_sha1<00 + 4>(x, 0x5A827999 + rotate_sha1<19>(ck));

            compress_sha1<20 + 0>(x, 0x6ED9EBA1 + rotate_sha1<0>(ck));
            compress_sha1<20 + 1>(x, 0x6ED9EBA1 + rotate_sha1<1>(ck));
            compress_sha1<20 + 2>(x, 0x6ED9EBA1 + rotate_sha1<2>(ck));
            compress_sha1<20 + 3>(x, 0x6ED9EBA1 + rotate_sha1<3>(ck));
            compress_sha1<20 + 4>(x, 0x6ED9EBA1 + rotate_sha1<4>(ck));
            compress_sha1<20 + 0>(x, 0x6ED9EBA1 + rotate_sha1<5>(ck));
            compress_sha1<20 + 1>(x, 0x6ED9EBA1 + rotate_sha1<6>(ck));
            compress_sha1<20 + 2>(x, 0x6ED9EBA1 + rotate_sha1<7>(ck));
            compress_sha1<20 + 3>(x, 0x6ED9EBA1 + rotate_sha1<8>(ck));
            compress_sha1<20 + 4>(x, 0x6ED9EBA1 + rotate_sha1<9>(ck));
            compress_sha1<20 + 0>(x, 0x6ED9EBA1 + rotate_sha1<10>(ck));
            compress_sha1<20 + 1>(x, 0x6ED9EBA1 + rotate_sha1<11>(ck));
            compress_sha1<20 + 2>(x, 0x6ED9EBA1 + rotate_sha1<12>(ck));
            compress_sha1<20 + 3>(x, 0x6ED9EBA1 + rotate_sha1<13>(ck));
            compress_sha1<20 + 4>(x, 0x6ED9EBA1 + rotate_sha1<14>(ck));
            compress_sha1<20 + 0>(x, 0x6ED9EBA1 + rotate_sha1<15>(ck));
            compress_sha1<20 + 1>(x, 0x6ED9EBA1 + rotate_sha1<16>(ck));
            compress_sha1<20 + 2>(x, 0x6ED9EBA1 + rotate_sha1<17>(ck));
            compress_sha1<20 + 3>(x, 0x6ED9EBA1 + rotate_sha1<18>(ck));
            compress_sha1<20 + 4>(x, 0x6ED9EBA1 + rotate_sha1<19>(ck));

            compress_sha1<40 + 0>(x, 0x8F1BBCDC + rotate_sha1<0>(ck));
            compress_sha1<40 + 1>(x, 0x8F1BBCDC + rotate_sha1<1>(ck));
            compress_sha1<40 + 2>(x, 0x8F1BBCDC + rotate_sha1<2>(ck));
            compress_sha1<40 + 3>(x, 0x8F1BBCDC + rotate_sha1<3>(ck));
            compress_sha1<40 + 4>(x, 0x8F1BBCDC + rotate_sha1<4>(ck));
            compress_sha1<40 + 0>(x, 0x8F1BBCDC + rotate_sha1<5>(ck));
            compress_sha1<40 + 1>(x, 0x8F1BBCDC + rotate_sha1<6>(ck));
            compress_sha1<40 + 2>(x, 0x8F1BBCDC + rotate_sha1<7>(ck));
            compress_sha1<40 + 3>(x, 0x8F1BBCDC + rotate_sha1<8>(ck));
            compress_sha1<40 + 4>(x, 0x8F1BBCDC + rotate_sha1<9>(ck));
            compress_sha1<40 + 0>(x, 0x8F1BBCDC + rotate_sha1<10>(ck));
            compress_sha1<40 + 1>(x, 0x8F1BBCDC + rotate_sha1<11>(ck));
            compress_sha1<40 + 2>(x, 0x8F1BBCDC + rotate_sha1<12>(ck));
            compress_sha1<40 + 3>(x, 0x8F1BBCDC + rotate_sha1<13>(ck));
            compress_sha1<40 + 4>(x, 0x8F1BBCDC + rotate_sha1<14>(ck));
            compress_sha1<40 + 0>(x, 0x8F1BBCDC + rotate_sha1<15>(ck));
            compress_sha1<40 + 1>(x, 0x8F1BBCDC + rotate_sha1<16>(ck));
            compress_sha1<40 + 2>(x, 0x8F1BBCDC + rotate_sha1<17>(ck));
            compress_sha1<40 + 3>(x, 0x8F1BBCDC + rotate_sha1<18>(ck));
            compress_sha1<40 + 4>(x, 0x8F1BBCDC + rotate_sha1<19>(ck));

            compress_sha1<60 + 0>(x, 0xCA62C1D6 + rotate_sha1<0>(ck));
            compress_sha1<60 + 1>(x, 0xCA62C1D6 + rotate_sha1<1>(ck));
            compress_sha1<60 + 2>(x, 0xCA62C1D6 + rotate_sha1<2>(ck));
            compress_sha1<60 + 3>(x, 0xCA62C1D6 + rotate_sha1<3>(ck));
            compress_sha1<60 + 4>(x, 0xCA62C1D6 + rotate_sha1<4>(ck));
            compress_sha1<60 + 0>(x, 0xCA62C1D6 + rotate_sha1<5>(ck));
            compress_sha1<60 + 1>(x, 0xCA62C1D6 + rotate_sha1<6>(ck));
            compress_sha1<60 + 2>(x, 0xCA62C1D6 + rotate_sha1<7>(ck));
            compress_sha1<60 + 3>(x, 0xCA62C1D6 + rotate_sha1<8>(ck));
            compress_sha1<60 + 4>(x, 0xCA62C1D6 + rotate_sha1<9>(ck));
            compress_sha1<60 + 0>(x, 0xCA62C1D6 + rotate_sha1<10>(ck));
            compress_sha1<60 + 1>(x, 0xCA62C1D6 + rotate_sha1<11>(ck));
            compress_sha1<60 + 2>(x, 0xCA62C1D6 + rotate_sha1<12>(ck));
            compress_sha1<60 + 3>(x, 0xCA62C1D6 + rotate_sha1<13>(ck));
            compress_sha1<60 + 4>(x, 0xCA62C1D6 + rotate_sha1<14>(ck));
            compress_sha1<60 + 0>(x, 0xCA62C1D6 + rotate_sha1<15>(ck));
            compress_sha1<60 + 1>(x, 0xCA62C1D6 + rotate_sha1<16>(ck));
            compress_sha1<60 + 2>(x, 0xCA62C1D6 + rotate_sha1<17>(ck));
            compress_sha1<60 + 3>(x, 0xCA62C1D6 + rotate_sha1<18>(ck));
            compress_sha1<60 + 4>(x, 0xCA62C1D6 + rotate_sha1<19>(ck));

            vec[0] += x[0];
            vec[1] += x[1];
            vec[2] += x[2];
            vec[3] += x[3];
            vec[4] += x[4];
        }

        inline namespace context
        {
            using functions::vector_t;
            using functions::chunk_t;

            template <class T, class TDigest>
            struct sha2_state_t
            {
                using unit_t = T;
                using digest_t = TDigest;

                vector_t<T> vec;
                chunk_t<T> input;
                uintmax_t wrote;
            };

            using sha1_state_t = sha2_state_t<uint32_t, sha1_digest_t>;

            static inline constexpr sha1_state_t create_sha1_state() { return {sha1_initial_vector, {}, 0}; }

            template <class sha2_state_t = sha1_state_t, auto process_chunk = functions::process_chunk_sha1>
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

            template <class sha2_state_t = sha1_state_t, auto process_chunk = functions::process_chunk_sha1>
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
                    stt.input[14] = byteswap(static_cast<T>(stt.wrote >> (sizeof(T) * 8 - 3))); // higher
                    stt.input[15] = byteswap(static_cast<T>(stt.wrote << 3));                   // lower
                    process_chunk(stt.vec, stt.input);
                }

                // to digest value
                {
                    vector_t<T> vec = stt.vec;
                    for (auto&& i : vec) { i = byteswap(i); }

                    digest_t digest{};
                    memcpy(&digest, &vec, sizeof(digest));
                    return digest;
                }
            }
        }
    }

    namespace ref
    {
        using functions::sha1_state_t;
        using functions::create_sha1_state;
        static inline void process_bytes(sha1_state_t& stt, const void* data, size_t len) noexcept { return functions::process_bytes<sha1_state_t, functions::process_chunk_sha1>(stt, data, len); }
        static inline sha1_digest_t finalize_and_get_digest(sha1_state_t& stt) noexcept { return functions::finalize_and_get_digest<sha1_state_t, functions::process_chunk_sha1>(stt); }
    }

    using namespace ref;
}
