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

namespace arkana::sha2
{
    template <size_t bits>
    using sha2_digest = byte_array<bits / 8>;

    template <class impl>
    class sha2_context
    {
    public:
        static inline constexpr size_t digest_bits = impl::digest_bits;
        using digest_t = sha2_digest<digest_bits>;

    private:
        typename impl::context_t ctx = impl::create_context();

    public:
        sha2_context() = default;
        sha2_context(const sha2_context& other) = default;
        sha2_context(sha2_context&& other) noexcept = default;
        sha2_context& operator=(const sha2_context& other) = default;
        sha2_context& operator=(sha2_context&& other) noexcept = default;
        ~sha2_context() = default;

        sha2_context& process_bytes(const void* data, size_t len) noexcept
        {
            impl::process_bytes(ctx, data, len);
            return *this;
        }

        sha2_context& process_string(std::string_view text) noexcept
        {
            return process_bytes(text.data(), text.length());
        }

        digest_t finalize() noexcept
        {
            return impl::finalize(ctx);
        }
    };

    namespace functions
    {
        using bits::rotr;
        using bits::byteswap;

        template <
            size_t bits,
            class T,
            const T* round_constants, size_t rounds,
            int s0_0, int s0_1, int s0_2, int s1_0, int s1_1, int s1_2,
            int S0_0, int S0_1, int S0_2, int S1_0, int S1_1, int S1_2,
            T iv0, T iv1, T iv2, T iv3, T iv4, T iv5, T iv6, T iv7>
        struct sha2_impl
        {
            static inline constexpr size_t digest_bits = bits;
            using digest_t = sha2_digest<bits>;

            using unit_t = T;
            using vector_t = array<T, 8>;
            using chunk_t = array<T, 16>;

            static inline constexpr size_t chunk_size = sizeof(chunk_t);
            static inline constexpr vector_t iv = {iv0, iv1, iv2, iv3, iv4, iv5, iv6, iv7};

            struct context_t
            {
                vector_t vec;
                chunk_t buf;
                uintmax_t wrote;
            };

            static context_t create_context() noexcept
            {
                return context_t{iv, {}, {}};
            }

            static void process_bytes(context_t& ctx, const void* data, size_t len) noexcept
            {
                const byte_t* src = static_cast<const byte_t*>(data);

                while (true)
                {
                    byte_t* buf = reinterpret_cast<byte_t*>(ctx.buf.data());
                    size_t used = ctx.wrote % chunk_size;
                    size_t sz = std::min(chunk_size - used, len);

                    memcpy(buf + used, src, sz);

                    len -= sz;
                    src += sz;
                    used += sz;
                    ctx.wrote += sz;

                    if (used != chunk_size) break;

                    process_chunk(ctx.vec, ctx.buf);
                }
            }

            static digest_t finalize(context_t& ctx) noexcept
            {
                byte_t* buf = reinterpret_cast<byte_t*>(ctx.buf.data());
                size_t used = ctx.wrote % chunk_size;

                // appends '1' and pads buffer
                buf[used++] = static_cast<byte_t>(0x80);
                memset(buf + used, 0, chunk_size - used);

                // if there is no buffer for message length, processes current chunk
                if (chunk_size - used < sizeof(T) * 2)
                {
                    process_chunk(ctx.vec, ctx.buf);
                    ctx.buf = {};
                }

                // processes last chunk
                {
                    // puts message length
                    ctx.buf[14] = byteswap(static_cast<T>(ctx.wrote >> (sizeof(T) * 8 - 3))); // higher
                    ctx.buf[15] = byteswap(static_cast<T>(ctx.wrote << 3)); // lower
                    process_chunk(ctx.vec, ctx.buf);
                }

                return vector_to_digest(ctx.vec);
            }

            static void process_chunk(vector_t& vec, chunk_t ck) noexcept
            {
                for (auto&& i : ck) { i = byteswap(i); }

                vector_t x = vec;
                compress<0>(x, round_constants[0], ck[0]);
                compress<1>(x, round_constants[1], ck[1]);
                compress<2>(x, round_constants[2], ck[2]);
                compress<3>(x, round_constants[3], ck[3]);
                compress<4>(x, round_constants[4], ck[4]);
                compress<5>(x, round_constants[5], ck[5]);
                compress<6>(x, round_constants[6], ck[6]);
                compress<7>(x, round_constants[7], ck[7]);
                compress<0>(x, round_constants[8], ck[8]);
                compress<1>(x, round_constants[9], ck[9]);
                compress<2>(x, round_constants[10], ck[10]);
                compress<3>(x, round_constants[11], ck[11]);
                compress<4>(x, round_constants[12], ck[12]);
                compress<5>(x, round_constants[13], ck[13]);
                compress<6>(x, round_constants[14], ck[14]);
                compress<7>(x, round_constants[15], ck[15]);

                for (size_t i = 1; i < (rounds / 16); i++)
                {
                    compress<0>(x, round_constants[i * 16 + 0], rotate<0>(ck));
                    compress<1>(x, round_constants[i * 16 + 1], rotate<1>(ck));
                    compress<2>(x, round_constants[i * 16 + 2], rotate<2>(ck));
                    compress<3>(x, round_constants[i * 16 + 3], rotate<3>(ck));
                    compress<4>(x, round_constants[i * 16 + 4], rotate<4>(ck));
                    compress<5>(x, round_constants[i * 16 + 5], rotate<5>(ck));
                    compress<6>(x, round_constants[i * 16 + 6], rotate<6>(ck));
                    compress<7>(x, round_constants[i * 16 + 7], rotate<7>(ck));
                    compress<0>(x, round_constants[i * 16 + 8], rotate<8>(ck));
                    compress<1>(x, round_constants[i * 16 + 9], rotate<9>(ck));
                    compress<2>(x, round_constants[i * 16 + 10], rotate<10>(ck));
                    compress<3>(x, round_constants[i * 16 + 11], rotate<11>(ck));
                    compress<4>(x, round_constants[i * 16 + 12], rotate<12>(ck));
                    compress<5>(x, round_constants[i * 16 + 13], rotate<13>(ck));
                    compress<6>(x, round_constants[i * 16 + 14], rotate<14>(ck));
                    compress<7>(x, round_constants[i * 16 + 15], rotate<15>(ck));
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

            template <size_t i>
            static inline T rotate(chunk_t& ck) noexcept
            {
                T w15 = ck[(i - 15) & 15];
                T w7 = ck[(i - 7) & 15];
                T w2 = ck[(i - 2) & 15];
                T s0 = rotr(w15, s0_0) ^ rotr(w15, s0_1) ^ w15 >> s0_2;
                T s1 = rotr(w2, s1_0) ^ rotr(w2, s1_1) ^ w2 >> s1_2;
                return ck[i & 15] += s0 + w7 + s1;
            }

            template <int i>
            static inline void compress(vector_t& vec, T ki, T wi) noexcept
            {
                T a = vec[(0 - i) & 7];
                T b = vec[(1 - i) & 7];
                T c = vec[(2 - i) & 7];
                T d = vec[(3 - i) & 7];
                T e = vec[(4 - i) & 7];
                T f = vec[(5 - i) & 7];
                T g = vec[(6 - i) & 7];
                T h = vec[(7 - i) & 7];

                T S0 = rotr(a, S0_0) ^ rotr(a, S0_1) ^ rotr(a, S0_2);
                T S1 = rotr(e, S1_0) ^ rotr(e, S1_1) ^ rotr(e, S1_2);
                T ch = (e & f) ^ (~e & g);
                T maj = (a & b) ^ (a & c) ^ (b & c);

                h += wi + ki + ch + S1;
                d += h;
                h += maj + S0;

                vec[(3 - i) & 7] = d;
                vec[(7 - i) & 7] = h;
            }

            static inline digest_t vector_to_digest(vector_t vec) noexcept
            {
                for (auto&& i : vec) { i = byteswap(i); }
                digest_t t;
                memcpy(&t, &vec, sizeof(t));
                return t;
            }
        };

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

        using sha224_impl = sha2_impl<
            224, uint32_t, round_constants_32, 64,
            7, 18, 3, 17, 19, 10, 2, 13, 22, 6, 11, 25,
            0xc1059ed8, 0x367cd507, 0x3070dd17, 0xf70e5939, 0xffc00b31, 0x68581511, 0x64f98fa7, 0xbefa4fa4>;

        using sha256_impl = sha2_impl<
            256, uint32_t, round_constants_32, 64,
            7, 18, 3, 17, 19, 10, 2, 13, 22, 6, 11, 25,
            0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a, 0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19>;

        using sha384_impl = sha2_impl<
            384, uint64_t, round_constants_64, 80,
            1, 8, 7, 19, 61, 6, 28, 34, 39, 14, 18, 41,
            0xcbbb9d5dc1059ed8, 0x629a292a367cd507, 0x9159015a3070dd17, 0x152fecd8f70e5939,
            0x67332667ffc00b31, 0x8eb44a8768581511, 0xdb0c2e0d64f98fa7, 0x47b5481dbefa4fa4>;

        using sha512_impl = sha2_impl<
            512, uint64_t, round_constants_64, 80,
            1, 8, 7, 19, 61, 6, 28, 34, 39, 14, 18, 41,
            0x6a09e667f3bcc908, 0xbb67ae8584caa73b, 0x3c6ef372fe94f82b, 0xa54ff53a5f1d36f1,
            0x510e527fade682d1, 0x9b05688c2b3e6c1f, 0x1f83d9abfb41bd6b, 0x5be0cd19137e2179>;
    }

    using sha224_context = sha2_context<functions::sha224_impl>;
    using sha256_context = sha2_context<functions::sha256_impl>;
    using sha384_context = sha2_context<functions::sha384_impl>;
    using sha512_context = sha2_context<functions::sha512_impl>;
}
