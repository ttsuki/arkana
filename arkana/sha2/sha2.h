/// @file
/// @brief	arkana::sha2
///			- An implementation of SHA-2
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
    using sha224_digest_t = sha2_digest_t<224>;
    using sha256_digest_t = sha2_digest_t<256>;
    using sha384_digest_t = sha2_digest_t<384>;
    using sha512_digest_t = sha2_digest_t<512>;

    namespace functions
    {
        using bits::rotr;
        using bits::byteswap;

        template <class T> using vector_t = std::array<T, 8>;
        template <class T> using chunk_t = std::array<T, 16>;

        static constexpr inline vector_t<uint32_t> sha224_initial_vector = {0xc1059ed8, 0x367cd507, 0x3070dd17, 0xf70e5939, 0xffc00b31, 0x68581511, 0x64f98fa7, 0xbefa4fa4};
        static constexpr inline vector_t<uint32_t> sha256_initial_vector = {0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a, 0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19};
        static constexpr inline vector_t<uint64_t> sha384_initial_vector = {0xcbbb9d5dc1059ed8, 0x629a292a367cd507, 0x9159015a3070dd17, 0x152fecd8f70e5939, 0x67332667ffc00b31, 0x8eb44a8768581511, 0xdb0c2e0d64f98fa7, 0x47b5481dbefa4fa4};
        static constexpr inline vector_t<uint64_t> sha512_initial_vector = {0x6a09e667f3bcc908, 0xbb67ae8584caa73b, 0x3c6ef372fe94f82b, 0xa54ff53a5f1d36f1, 0x510e527fade682d1, 0x9b05688c2b3e6c1f, 0x1f83d9abfb41bd6b, 0x5be0cd19137e2179};

        static constexpr inline uint32_t round_constants_32[] = {
            0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
            0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
            0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
            0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
            0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
            0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
            0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
            0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2,
        };

        static constexpr inline uint64_t round_constants_64[] = {
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

        template <class T, int S00, int S01, int S02, int S10, int S11, int S12, int i>
        static inline constexpr void compress(vector_t<T>& vec, T kwi) noexcept
        {
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

        template <class T, int s00, int s01, int s02, int s10, int s11, int s12, size_t i>
        static inline constexpr T rotate(chunk_t<T>& ck) noexcept
        {
            using bits::rotr;

            T w15 = ck[(i - 15) & 15];
            T w7 = ck[(i - 7) & 15];
            T w2 = ck[(i - 2) & 15];
            T s0 = rotr(w15, s00) ^ rotr(w15, s01) ^ w15 >> s02;
            T s1 = rotr(w2, s10) ^ rotr(w2, s11) ^ w2 >> s12;
            return ck[i & 15] += s0 + w7 + s1;
        }

        template <
            class T,
            const T* round_constants, size_t rounds,
            int s00, int s01, int s02, int s10, int s11, int s12,
            int S00, int S01, int S02, int S10, int S11, int S12>
        static constexpr void process_chunk_impl(vector_t<T>& vec, chunk_t<T> input) noexcept
        {
            using bits::byteswap;

            for (auto&& i : input) { i = byteswap(i); }

            vector_t<T> x = vec;
            compress<T, S00, S01, S02, S10, S11, S12, 0>(x, round_constants[0] + input[0]);
            compress<T, S00, S01, S02, S10, S11, S12, 1>(x, round_constants[1] + input[1]);
            compress<T, S00, S01, S02, S10, S11, S12, 2>(x, round_constants[2] + input[2]);
            compress<T, S00, S01, S02, S10, S11, S12, 3>(x, round_constants[3] + input[3]);
            compress<T, S00, S01, S02, S10, S11, S12, 4>(x, round_constants[4] + input[4]);
            compress<T, S00, S01, S02, S10, S11, S12, 5>(x, round_constants[5] + input[5]);
            compress<T, S00, S01, S02, S10, S11, S12, 6>(x, round_constants[6] + input[6]);
            compress<T, S00, S01, S02, S10, S11, S12, 7>(x, round_constants[7] + input[7]);
            compress<T, S00, S01, S02, S10, S11, S12, 0>(x, round_constants[8] + input[8]);
            compress<T, S00, S01, S02, S10, S11, S12, 1>(x, round_constants[9] + input[9]);
            compress<T, S00, S01, S02, S10, S11, S12, 2>(x, round_constants[10] + input[10]);
            compress<T, S00, S01, S02, S10, S11, S12, 3>(x, round_constants[11] + input[11]);
            compress<T, S00, S01, S02, S10, S11, S12, 4>(x, round_constants[12] + input[12]);
            compress<T, S00, S01, S02, S10, S11, S12, 5>(x, round_constants[13] + input[13]);
            compress<T, S00, S01, S02, S10, S11, S12, 6>(x, round_constants[14] + input[14]);
            compress<T, S00, S01, S02, S10, S11, S12, 7>(x, round_constants[15] + input[15]);

            for (size_t i = 1; i < (rounds / 16); i++)
            {
                compress<T, S00, S01, S02, S10, S11, S12, 0>(x, round_constants[i * 16 + 0] + rotate<T, s00, s01, s02, s10, s11, s12, 0>(input));
                compress<T, S00, S01, S02, S10, S11, S12, 1>(x, round_constants[i * 16 + 1] + rotate<T, s00, s01, s02, s10, s11, s12, 1>(input));
                compress<T, S00, S01, S02, S10, S11, S12, 2>(x, round_constants[i * 16 + 2] + rotate<T, s00, s01, s02, s10, s11, s12, 2>(input));
                compress<T, S00, S01, S02, S10, S11, S12, 3>(x, round_constants[i * 16 + 3] + rotate<T, s00, s01, s02, s10, s11, s12, 3>(input));
                compress<T, S00, S01, S02, S10, S11, S12, 4>(x, round_constants[i * 16 + 4] + rotate<T, s00, s01, s02, s10, s11, s12, 4>(input));
                compress<T, S00, S01, S02, S10, S11, S12, 5>(x, round_constants[i * 16 + 5] + rotate<T, s00, s01, s02, s10, s11, s12, 5>(input));
                compress<T, S00, S01, S02, S10, S11, S12, 6>(x, round_constants[i * 16 + 6] + rotate<T, s00, s01, s02, s10, s11, s12, 6>(input));
                compress<T, S00, S01, S02, S10, S11, S12, 7>(x, round_constants[i * 16 + 7] + rotate<T, s00, s01, s02, s10, s11, s12, 7>(input));
                compress<T, S00, S01, S02, S10, S11, S12, 0>(x, round_constants[i * 16 + 8] + rotate<T, s00, s01, s02, s10, s11, s12, 8>(input));
                compress<T, S00, S01, S02, S10, S11, S12, 1>(x, round_constants[i * 16 + 9] + rotate<T, s00, s01, s02, s10, s11, s12, 9>(input));
                compress<T, S00, S01, S02, S10, S11, S12, 2>(x, round_constants[i * 16 + 10] + rotate<T, s00, s01, s02, s10, s11, s12, 10>(input));
                compress<T, S00, S01, S02, S10, S11, S12, 3>(x, round_constants[i * 16 + 11] + rotate<T, s00, s01, s02, s10, s11, s12, 11>(input));
                compress<T, S00, S01, S02, S10, S11, S12, 4>(x, round_constants[i * 16 + 12] + rotate<T, s00, s01, s02, s10, s11, s12, 12>(input));
                compress<T, S00, S01, S02, S10, S11, S12, 5>(x, round_constants[i * 16 + 13] + rotate<T, s00, s01, s02, s10, s11, s12, 13>(input));
                compress<T, S00, S01, S02, S10, S11, S12, 6>(x, round_constants[i * 16 + 14] + rotate<T, s00, s01, s02, s10, s11, s12, 14>(input));
                compress<T, S00, S01, S02, S10, S11, S12, 7>(x, round_constants[i * 16 + 15] + rotate<T, s00, s01, s02, s10, s11, s12, 15>(input));
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

        static inline constexpr void process_chunk_256(vector_t<uint32_t>& v, const chunk_t<uint32_t>& input)
        {
            return process_chunk_impl<
                uint32_t,
                round_constants_32, 64,
                7, 18, 3, 17, 19, 10,
                2, 13, 22, 6, 11, 25>(v, input);
        }

        static inline constexpr void process_chunk_512(vector_t<uint64_t>& v, const chunk_t<uint64_t>& input)
        {
            return process_chunk_impl<
                uint64_t,
                round_constants_64, 80,
                1, 8, 7, 19, 61, 6,
                28, 34, 39, 14, 18, 41>(v, input);
        }

        inline namespace context
        {
            using functions::vector_t;
            using functions::chunk_t;

            template <size_t t_digest_bits, class T>
            struct sha2_state_t
            {
                using unit_t = T;
                static inline constexpr size_t digest_bits = t_digest_bits;

                vector_t<T> vec;
                chunk_t<T> input;
                uintmax_t wrote;
            };

            using sha224_state_t = sha2_state_t<224, uint32_t>;
            using sha256_state_t = sha2_state_t<256, uint32_t>;
            using sha384_state_t = sha2_state_t<384, uint64_t>;
            using sha512_state_t = sha2_state_t<512, uint64_t>;

            static inline constexpr sha224_state_t create_sha224_state() { return {functions::sha224_initial_vector, {}, 0}; }
            static inline constexpr sha256_state_t create_sha256_state() { return {functions::sha256_initial_vector, {}, 0}; }
            static inline constexpr sha384_state_t create_sha384_state() { return {functions::sha384_initial_vector, {}, 0}; }
            static inline constexpr sha512_state_t create_sha512_state() { return {functions::sha512_initial_vector, {}, 0}; }

            template <class sha2_state_t = sha256_state_t, auto process_chunk = functions::process_chunk_256>
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

            template <class sha2_state_t = sha256_state_t, auto process_chunk = functions::process_chunk_256>
            static auto finalize_and_get_digest(sha2_state_t& stt) noexcept -> sha2_digest_t<sha2_state_t::digest_bits>
            {
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
                    vector_t<T> vec = stt.vec;
                    for (auto&& i : vec) { i = byteswap(i); }

                    sha2_digest_t<sha2_state_t::digest_bits> digest{};
                    memcpy(&digest, &vec, sizeof(digest));
                    return digest;
                }
            }
        }
    }

    namespace ref
    {
        using functions::sha224_state_t;
        using functions::sha256_state_t;
        using functions::sha384_state_t;
        using functions::sha512_state_t;
        using functions::create_sha224_state;
        using functions::create_sha256_state;
        using functions::create_sha384_state;
        using functions::create_sha512_state;
        static inline void process_bytes(sha224_state_t& stt, const void* data, size_t len) noexcept { return functions::process_bytes<sha224_state_t, functions::process_chunk_256>(stt, data, len); }
        static inline void process_bytes(sha256_state_t& stt, const void* data, size_t len) noexcept { return functions::process_bytes<sha256_state_t, functions::process_chunk_256>(stt, data, len); }
        static inline void process_bytes(sha384_state_t& stt, const void* data, size_t len) noexcept { return functions::process_bytes<sha384_state_t, functions::process_chunk_512>(stt, data, len); }
        static inline void process_bytes(sha512_state_t& stt, const void* data, size_t len) noexcept { return functions::process_bytes<sha512_state_t, functions::process_chunk_512>(stt, data, len); }
        static inline sha224_digest_t finalize_and_get_digest(sha224_state_t& stt) noexcept { return functions::finalize_and_get_digest<sha224_state_t, functions::process_chunk_256>(stt); }
        static inline sha256_digest_t finalize_and_get_digest(sha256_state_t& stt) noexcept { return functions::finalize_and_get_digest<sha256_state_t, functions::process_chunk_256>(stt); }
        static inline sha384_digest_t finalize_and_get_digest(sha384_state_t& stt) noexcept { return functions::finalize_and_get_digest<sha384_state_t, functions::process_chunk_512>(stt); }
        static inline sha512_digest_t finalize_and_get_digest(sha512_state_t& stt) noexcept { return functions::finalize_and_get_digest<sha512_state_t, functions::process_chunk_512>(stt); }
    }

    using namespace ref;
}
