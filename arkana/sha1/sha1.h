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
        using vector_t = std::array<uint32_t, 5>;
        using chunk_t = std::array<uint32_t, 16>;

        static constexpr inline vector_t sha1_initial_vector = {0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476, 0xC3D2E1F0};

        template <int i>
        static inline constexpr void compress(vector_t& vec, uint32_t wi) noexcept
        {
            using bits::rotl;

            uint32_t a = vec[(80 - i) % 5];
            uint32_t b = vec[(81 - i) % 5];
            uint32_t c = vec[(82 - i) % 5];
            uint32_t d = vec[(83 - i) % 5];
            uint32_t e = vec[(84 - i) % 5];

            uint32_t fk = 0;
            if constexpr (i < 20) fk = ((b & c) | (~b & d)) + 0x5A827999;
            else if constexpr (i < 40) fk = (b ^ c ^ d) + 0x6ED9EBA1;
            else if constexpr (i < 60) fk = ((b & c) | (b & d) | (c & d)) + 0x8F1BBCDC;
            else fk = (b ^ c ^ d) + 0xCA62C1D6;

            vec[(81 - i) % 5] = rotl(b, 30);
            vec[(84 - i) % 5] = rotl(a, 5) + fk + e + wi;
        }

        template <size_t i>
        static inline constexpr uint32_t rotate(chunk_t& ck) noexcept
        {
            using bits::rotl;
            uint32_t w16 = ck[(i - 16) & 15];
            uint32_t w14 = ck[(i - 14) & 15];
            uint32_t w8 = ck[(i - 8) & 15];
            uint32_t w3 = ck[(i - 3) & 15];
            return ck[i & 15] = rotl(w3 ^ w8 ^ w14 ^ w16, 1);
        }

        template <size_t i>
        static constexpr void round16x(vector_t& x, chunk_t& input) noexcept
        {
            if constexpr (i == 0)
            {
                compress<i * 16 + 0>(x, input[0]);
                compress<i * 16 + 1>(x, input[1]);
                compress<i * 16 + 2>(x, input[2]);
                compress<i * 16 + 3>(x, input[3]);
                compress<i * 16 + 4>(x, input[4]);
                compress<i * 16 + 5>(x, input[5]);
                compress<i * 16 + 6>(x, input[6]);
                compress<i * 16 + 7>(x, input[7]);
                compress<i * 16 + 8>(x, input[8]);
                compress<i * 16 + 9>(x, input[9]);
                compress<i * 16 + 10>(x, input[10]);
                compress<i * 16 + 11>(x, input[11]);
                compress<i * 16 + 12>(x, input[12]);
                compress<i * 16 + 13>(x, input[13]);
                compress<i * 16 + 14>(x, input[14]);
                compress<i * 16 + 15>(x, input[15]);
            }
            else
            {
                compress<i * 16 + 0>(x, rotate<0>(input));
                compress<i * 16 + 1>(x, rotate<1>(input));
                compress<i * 16 + 2>(x, rotate<2>(input));
                compress<i * 16 + 3>(x, rotate<3>(input));
                compress<i * 16 + 4>(x, rotate<4>(input));
                compress<i * 16 + 5>(x, rotate<5>(input));
                compress<i * 16 + 6>(x, rotate<6>(input));
                compress<i * 16 + 7>(x, rotate<7>(input));
                compress<i * 16 + 8>(x, rotate<8>(input));
                compress<i * 16 + 9>(x, rotate<9>(input));
                compress<i * 16 + 10>(x, rotate<10>(input));
                compress<i * 16 + 11>(x, rotate<11>(input));
                compress<i * 16 + 12>(x, rotate<12>(input));
                compress<i * 16 + 13>(x, rotate<13>(input));
                compress<i * 16 + 14>(x, rotate<14>(input));
                compress<i * 16 + 15>(x, rotate<15>(input));
            }
        }

        static constexpr void process_chunk(vector_t& vec, chunk_t input) noexcept
        {
            using bits::byteswap;

            for (auto&& i : input) { i = byteswap(i); }

            vector_t x = vec;

            round16x<0>(x, input);
            round16x<1>(x, input);
            round16x<2>(x, input);
            round16x<3>(x, input);
            round16x<4>(x, input);

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

            struct sha1_state_t
            {
                static inline constexpr size_t digest_bits = 160;
                vector_t vec;
                chunk_t input;
                uintmax_t wrote;
            };

            static inline constexpr sha1_state_t create_sha1_state()
            {
                return {functions::sha1_initial_vector, {}, 0};
            }

            template <auto process_chunk = functions::process_chunk>
            static void process_bytes(sha1_state_t& stt, const void* data, size_t len) noexcept
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

            template <auto process_chunk = functions::process_chunk>
            static auto finalize_and_get_digest(sha1_state_t& stt) noexcept -> sha1_digest_t
            {
                using bits::byteswap;

                using T = typename decltype(stt.vec)::value_type;
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
                    vector_t vec = stt.vec;
                    for (auto&& i : vec) { i = byteswap(i); }

                    sha1_digest_t digest{};
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
        static inline void process_bytes(sha1_state_t& stt, const void* data, size_t len) noexcept { return functions::process_bytes<functions::process_chunk>(stt, data, len); }
        static inline sha1_digest_t finalize_and_get_digest(sha1_state_t& stt) noexcept { return functions::finalize_and_get_digest<functions::process_chunk>(stt); }
    }

    using namespace ref;
}
