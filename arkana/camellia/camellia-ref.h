/// @file
/// @brief	arkana::camellia
///			- An implementation of Camellia cipher
/// @author Copyright(c) 2021 ttsuki
/// 
/// This software is released under the MIT License.
/// https://opensource.org/licenses/MIT
///
/// - camellia https://info.isl.ntt.co.jp/crypt/camellia/ 

#pragma once

#include "../ark/types.h"
#include "../ark/memory.h"
#include "../ark/bitmanip.h"
#include "../ark/lutgen.h"
#include "../ark/macros.h"

#include <stdexcept>

namespace arkana::camellia
{
    using block_t = byte_array<16>;

    template <size_t key_length> using key_t = byte_array<key_length>;
    using key_128bit_t = key_t<16>;
    using key_192bit_t = key_t<24>;
    using key_256bit_t = key_t<32>;

    namespace functions
    {
        template <class T>
        struct value_pair_t
        {
            T l, r;
        };

        template <class value_pair_t>
        static ARKANA_FORCEINLINE constexpr auto decompose(value_pair_t&& v) noexcept -> /* TODO sfinae */ value_pair_t
        {
            return v;
        }

        template <class value_pair_t, class value_t>
        static ARKANA_FORCEINLINE constexpr auto recompose(value_pair_t&& v, value_t&& l, value_t&& r) noexcept -> /* TODO sfinae */ value_pair_t
        {
            auto& [vl, vr] = v;
            vl = std::forward<value_t>(l);
            vr = std::forward<value_t>(r);
            return v;
        }

        static ARKANA_FORCEINLINE constexpr auto decompose(uint64_t x) noexcept -> value_pair_t<uint32_t>
        {
            return {static_cast<uint32_t>(x), static_cast<uint32_t>(x >> 32)};
        }

        static ARKANA_FORCEINLINE constexpr auto recompose(uint64_t& t, uint32_t l, uint32_t r) noexcept -> uint64_t&
        {
            return t = static_cast<std::uint64_t>(r) << 32 | l;
        }

        static ARKANA_FORCEINLINE constexpr uint32_t rotl_be1(uint32_t v) noexcept
        {
            /* Big endian rotl(v, 1) */     //  BE: 01234567 89abcdef ghijklmn opqrstuv
            /* on little endian      */     //  ->: 12345678 9abcdefg hijklmno pqrstuv0
            uint32_t x = v & 0x80808080u;   //x   = o_______ g_______ 8_______ 0_______ (& 0x80808080)
            uint32_t y = v & ~0x80808080u;  //  y = _pqrstuv _hijklmn _9abcdef _1234567 (& ~0x80808080)
            uint32_t a = x << 17 | x >> 15; //a   = _______0 _______o _______g _______8 (& 0x80808080 <<< 17)
            uint32_t b = y << 1;            //  b = pqrstuv_ hijklmn_ 9abcdef_ 1234567_ (& ~0x80808080 << 1)
            return a | b;                   //a|b = pqrstuv0 hijklmno 9abcdefg 12345678
        }

        inline namespace sbox
        {
            template <class T> using sbox_t = lutgen::lookup_table<T, 256>;

            static inline constexpr sbox_t<uint8_t> sbox_0 = {
                112, 130, 44, 236, 179, 39, 192, 229, 228, 133, 87, 53, 234, 12, 174, 65,
                35, 239, 107, 147, 69, 25, 165, 33, 237, 14, 79, 78, 29, 101, 146, 189,
                134, 184, 175, 143, 124, 235, 31, 206, 62, 48, 220, 95, 94, 197, 11, 26,
                166, 225, 57, 202, 213, 71, 93, 61, 217, 1, 90, 214, 81, 86, 108, 77,
                139, 13, 154, 102, 251, 204, 176, 45, 116, 18, 43, 32, 240, 177, 132, 153,
                223, 76, 203, 194, 52, 126, 118, 5, 109, 183, 169, 49, 209, 23, 4, 215,
                20, 88, 58, 97, 222, 27, 17, 28, 50, 15, 156, 22, 83, 24, 242, 34,
                254, 68, 207, 178, 195, 181, 122, 145, 36, 8, 232, 168, 96, 252, 105, 80,
                170, 208, 160, 125, 161, 137, 98, 151, 84, 91, 30, 149, 224, 255, 100, 210,
                16, 196, 0, 72, 163, 247, 117, 219, 138, 3, 230, 218, 9, 63, 221, 148,
                135, 92, 131, 2, 205, 74, 144, 51, 115, 103, 246, 243, 157, 127, 191, 226,
                82, 155, 216, 38, 200, 55, 198, 59, 129, 150, 111, 75, 19, 190, 99, 46,
                233, 121, 167, 140, 159, 110, 188, 142, 41, 245, 249, 182, 47, 253, 180, 89,
                120, 152, 6, 106, 231, 70, 113, 186, 212, 37, 171, 66, 136, 162, 141, 250,
                114, 7, 185, 85, 248, 238, 172, 10, 54, 73, 42, 104, 60, 56, 241, 164,
                64, 40, 211, 123, 187, 201, 67, 193, 21, 227, 173, 244, 119, 199, 128, 158,
            };

            static inline constexpr sbox_t<uint8_t> sbox_1 = lutgen::generate_table<uint8_t, 256>([](auto i) { return static_cast<uint8_t>(sbox_0[i] << 1 | sbox_0[i] >> 7); });
            static inline constexpr sbox_t<uint8_t> sbox_2 = lutgen::generate_table<uint8_t, 256>([](auto i) { return static_cast<uint8_t>(sbox_0[i] << 7 | sbox_0[i] >> 1); });
            static inline constexpr sbox_t<uint8_t> sbox_3 = lutgen::generate_table<uint8_t, 256>([](auto i) { return sbox_0[static_cast<uint8_t>(i << 1 | i >> 7)]; });

            static inline constexpr sbox_t<uint32_t> sbox_32_0 = lutgen::generate_table<uint32_t, 256>([](auto i) { return 0x00010101u * sbox_0[i]; });
            static inline constexpr sbox_t<uint32_t> sbox_32_1 = lutgen::generate_table<uint32_t, 256>([](auto i) { return 0x01010100u * sbox_1[i]; });
            static inline constexpr sbox_t<uint32_t> sbox_32_2 = lutgen::generate_table<uint32_t, 256>([](auto i) { return 0x01010001u * sbox_2[i]; });
            static inline constexpr sbox_t<uint32_t> sbox_32_3 = lutgen::generate_table<uint32_t, 256>([](auto i) { return 0x01000101u * sbox_3[i]; });

            static inline constexpr sbox_t<uint64_t> sbox_64_0 = lutgen::generate_table<uint64_t, 256>([](auto i) { return 0x0100000100010101u * sbox_0[i]; });
            static inline constexpr sbox_t<uint64_t> sbox_64_1 = lutgen::generate_table<uint64_t, 256>([](auto i) { return 0x0000010101010100u * sbox_1[i]; });
            static inline constexpr sbox_t<uint64_t> sbox_64_2 = lutgen::generate_table<uint64_t, 256>([](auto i) { return 0x0001010001010001u * sbox_2[i]; });
            static inline constexpr sbox_t<uint64_t> sbox_64_3 = lutgen::generate_table<uint64_t, 256>([](auto i) { return 0x0101000001000101u * sbox_3[i]; });
            static inline constexpr sbox_t<uint64_t> sbox_64_4 = lutgen::generate_table<uint64_t, 256>([](auto i) { return 0x0101010001010100u * sbox_1[i]; });
            static inline constexpr sbox_t<uint64_t> sbox_64_5 = lutgen::generate_table<uint64_t, 256>([](auto i) { return 0x0101000101010001u * sbox_2[i]; });
            static inline constexpr sbox_t<uint64_t> sbox_64_6 = lutgen::generate_table<uint64_t, 256>([](auto i) { return 0x0100010101000101u * sbox_3[i]; });
            static inline constexpr sbox_t<uint64_t> sbox_64_7 = lutgen::generate_table<uint64_t, 256>([](auto i) { return 0x0001010100010101u * sbox_0[i]; });

            static ARKANA_FORCEINLINE constexpr uint32_t lookup_sbox32(const sbox_t<uint32_t>& sbox, uint32_t index, int shift) noexcept
            {
                return sbox[static_cast<uint8_t>(index >> 8 * shift)];
            }

            static ARKANA_FORCEINLINE constexpr uint64_t lookup_sbox64(const sbox_t<uint64_t>& sbox, uint64_t index, int shift) noexcept
            {
                return sbox[static_cast<uint8_t>(index >> 8 * shift)];
            }
        }

        using v32 = uint32_t;
        using v64 = uint64_t;
        using v128 = value_pair_t<uint64_t>;
        using key64 = uint64_t;

        template <class v64 = v64, auto lookup_sbox32 = lookup_sbox32, class key64 = key64>
        static ARKANA_FORCEINLINE constexpr auto camellia_f_table_lookup_32(v64 r, v64 l, const key64& k) noexcept -> v64
        {
            auto t = l; // copy
            auto&& [tl, tr] = decompose(t);
            auto&& [kl, kr] = decompose(k);

            tr ^= kr;
            tl ^= kl;

            auto d = lookup_sbox32(sbox::sbox_32_1, tr, 0) ^
                lookup_sbox32(sbox::sbox_32_2, tr, 1) ^
                lookup_sbox32(sbox::sbox_32_3, tr, 2) ^
                lookup_sbox32(sbox::sbox_32_0, tr, 3);

            auto u = lookup_sbox32(sbox::sbox_32_0, tl, 0) ^
                lookup_sbox32(sbox::sbox_32_1, tl, 1) ^
                lookup_sbox32(sbox::sbox_32_2, tl, 2) ^
                lookup_sbox32(sbox::sbox_32_3, tl, 3);

            tl = d ^ u;
            tr = tl ^ (u << 8 | u >> 24);

            return r ^= recompose(t, tl, tr);
        }

        template <class v64 = v64, auto lookup_sbox64 = lookup_sbox64, class key64 = key64>
        static ARKANA_FORCEINLINE constexpr auto camellia_f_table_lookup_64(v64 r, v64 l, const key64& k) noexcept -> v64
        {
            auto t = l; // copy
            t ^= k;
            t = lookup_sbox64(sbox::sbox_64_0, t, 0) ^
                lookup_sbox64(sbox::sbox_64_1, t, 1) ^
                lookup_sbox64(sbox::sbox_64_2, t, 2) ^
                lookup_sbox64(sbox::sbox_64_3, t, 3) ^
                lookup_sbox64(sbox::sbox_64_4, t, 4) ^
                lookup_sbox64(sbox::sbox_64_5, t, 5) ^
                lookup_sbox64(sbox::sbox_64_6, t, 6) ^
                lookup_sbox64(sbox::sbox_64_7, t, 7);
            return r ^= t;
        }

        template <class v64 = v64, auto lookup_sbox32 = lookup_sbox32, auto lookup_sbox64 = lookup_sbox64, class key64 = key64>
        static ARKANA_FORCEINLINE constexpr auto camellia_f_table_lookup(v64 r, v64 l, const key64& k) noexcept -> v64
        {
            if constexpr (sizeof(void*) == 8)
                return camellia_f_table_lookup_64<v64, lookup_sbox64, key64>(std::forward<v64>(r), std::forward<v64>(l), k);
            else
                return camellia_f_table_lookup_32<v64, lookup_sbox32, key64>(std::forward<v64>(r), std::forward<v64>(l), k);
        }

        template <class v64 = v64, auto rotl_be1 = rotl_be1, class key64 = key64>
        static ARKANA_FORCEINLINE constexpr auto camellia_fl(v64 l, const key64& k) noexcept -> v64
        {
            auto&& [ll, lr] = decompose(l);
            auto&& [kl, kr] = decompose(k);

            lr ^= rotl_be1(ll & kl);
            ll ^= (lr | kr);

            return recompose(l, ll, lr);
        }

        template <class v64 = v64, auto rotl_be1 = rotl_be1, class key64 = key64>
        static ARKANA_FORCEINLINE constexpr auto camellia_fl_inv(v64 r, const key64& k) noexcept -> v64
        {
            auto&& [rl, rr] = decompose(r);
            auto&& [kl, kr] = decompose(k);

            rl ^= (rr | kr);
            rr ^= rotl_be1(rl & kl);

            return recompose(r, rl, rr);
        }

        template <class v128 = v128, class key64 = key64>
        static ARKANA_FORCEINLINE constexpr auto camellia_prewhite(v128 v, const key64& kl, const key64& kr) noexcept -> v128
        {
            auto&& [vl, vr] = decompose(v);
            vl ^= kl;
            vr ^= kr;
            return recompose(v, vl, vr);
        }

        template <class v128 = v128, class key64 = key64>
        static ARKANA_FORCEINLINE constexpr auto camellia_postwhite(v128 v, const key64& kl, const key64& kr) noexcept -> v128
        {
            auto [vr, vl] = decompose(v); // copy
            vl ^= kl;
            vr ^= kr;
            return recompose(v, vl, vr);
        }

        template <class v128 = v128, class key64 = key64>
        static ARKANA_FORCEINLINE constexpr auto camellia_thruwhite(v128 v, const key64&, const key64&) noexcept -> v128
        {
            return v;
        }

        template <class v128 = v128>
        static ARKANA_FORCEINLINE constexpr auto xor_block(v128 v, const v128& k) noexcept -> v128
        {
            auto&& [vl, vr] = decompose(v);
            auto&& [kl, kr] = decompose(k);
            vl ^= kl;
            vr ^= kr;
            return recompose(v, vl, vr);
        }

        inline namespace key_scheduling
        {
            template <class key64>
            struct key_vector_small_t
            {
                key64 kw_1, kw_2;
                key64 k_1, k_2;
                key64 k_3, k_4;
                key64 k_5, k_6;
                key64 kl_1, kl_2;
                key64 k_7, k_8;
                key64 k_9, k_10;
                key64 k_11, k_12;
                key64 kl_3, kl_4;
                key64 k_13, k_14;
                key64 k_15, k_16;
                key64 k_17, k_18;
                key64 kw_3, kw_4;
            };

            template <class key64>
            struct key_vector_large_t
            {
                key64 kw_1, kw_2;
                key64 k_1, k_2;
                key64 k_3, k_4;
                key64 k_5, k_6;
                key64 kl_1, kl_2;
                key64 k_7, k_8;
                key64 k_9, k_10;
                key64 k_11, k_12;
                key64 kl_3, kl_4;
                key64 k_13, k_14;
                key64 k_15, k_16;
                key64 k_17, k_18;
                key64 kl_5, kl_6;
                key64 k_19, k_20;
                key64 k_21, k_22;
                key64 k_23, k_24;
                key64 kw_3, kw_4;
            };

            template <class key_t, class key64>
            using key_vector_for_t = std::enable_if_t<
                is_any_of_v<key_t, key_128bit_t, key_192bit_t, key_256bit_t>,
                std::conditional_t<std::is_same_v<key_t, key_128bit_t>, key_vector_small_t<key64>, key_vector_large_t<key64>>>;

            template <size_t key_length, bool encrypting, auto camellia_f = camellia_f_table_lookup<v64, lookup_sbox32, lookup_sbox64, key64>>
            static constexpr auto generate_key_vector(const key_t<key_length>& key, bool_constant_t<encrypting>  = {}) noexcept
            -> key_vector_for_t<key_t<key_length>, key64>
            {
                struct uint128_t
                {
                    uint64_t l, r;

                    ARKANA_FORCEINLINE uint128_t operator ^(uint128_t rhs) const noexcept { return uint128_t{l ^ rhs.l, r ^ rhs.r}; }
                    ARKANA_FORCEINLINE uint128_t& operator ^=(uint128_t rhs) noexcept { return l ^= rhs.l, r ^= rhs.r, *this; }

                    ARKANA_FORCEINLINE auto rotl(int i) const noexcept
                    {
                        auto xl = (i & 63) ? l << (i & 63) | r >> (-i & 63) : l;
                        auto xr = (i & 63) ? r << (i & 63) | l >> (-i & 63) : r;
                        return i & 64 ? uint128_t{xr, xl} : uint128_t{xl, xr};
                    }

                    ARKANA_FORCEINLINE auto byteswap() const noexcept
                    {
                        return uint128_t{ bit::byteswap(r), bit::byteswap(l) };
                    }
                };

                // little endian 128bit sub-key {KL,KR,KA,KB}
                auto [kl, kr, ka, kb] = [](const byte_t* key)
                {
                    uint128_t kl{};
                    uint128_t kr{};

                    if constexpr (key_length * 8 == 128)
                    {
                        kl.l = load_u<uint64_t>(key + 0);
                        kl.r = load_u<uint64_t>(key + 8);
                        kr = {};
                    }
                    else if constexpr (key_length * 8 == 192)
                    {
                        kl.l = load_u<uint64_t>(key + 0);
                        kl.r = load_u<uint64_t>(key + 8);
                        kr.l = load_u<uint64_t>(key + 16);
                        kr.r = ~kr.l;
                    }
                    else if constexpr (key_length * 8 == 256)
                    {
                        kl.l = load_u<uint64_t>(key + 0);
                        kl.r = load_u<uint64_t>(key + 8);
                        kr.l = load_u<uint64_t>(key + 16);
                        kr.r = load_u<uint64_t>(key + 24);
                    }

                    uint128_t t = kl;
                    t.l ^= kr.l;
                    t.r ^= kr.r;
                    t.r = camellia_f(t.r, t.l, bit::byteswap(0xA09E667F3BCC908Bu));
                    t.l = camellia_f(t.l, t.r, bit::byteswap(0xB67AE8584CAA73B2u));
                    t.l ^= kl.l;
                    t.r ^= kl.r;
                    t.r = camellia_f(t.r, t.l, bit::byteswap(0xC6EF372FE94F82BEu));
                    t.l = camellia_f(t.l, t.r, bit::byteswap(0x54FF53A5F1D36F1Cu));
                    uint128_t ka = t;
                    t.l ^= kr.l;
                    t.r ^= kr.r;
                    t.r = camellia_f(t.r, t.l, bit::byteswap(0x10E527FADE682D1Du));
                    t.l = camellia_f(t.l, t.r, bit::byteswap(0xB05688C2B3E6C1FDu));
                    uint128_t kb = t;

                    return std::make_tuple(kl.byteswap(), kr.byteswap(), ka.byteswap(), kb.byteswap());
                }(static_cast<const byte_t*>(key.data()));

                // result
                key_vector_for_t<key_t<key_length>, key64> r{};
                if constexpr (std::is_same_v<decltype(r), key_vector_small_t<key64>>)
                {
                    // 128 bit key
                    (encrypting ? r.kw_1 : r.kw_3) = kl.rotl(0).byteswap().l;
                    (encrypting ? r.kw_2 : r.kw_4) = kl.rotl(0).byteswap().r;
                    (encrypting ? r.k_1 : r.k_18) = ka.rotl(0).byteswap().l;
                    (encrypting ? r.k_2 : r.k_17) = ka.rotl(0).byteswap().r;
                    (encrypting ? r.k_3 : r.k_16) = kl.rotl(15).byteswap().l;
                    (encrypting ? r.k_4 : r.k_15) = kl.rotl(15).byteswap().r;
                    (encrypting ? r.k_5 : r.k_14) = ka.rotl(15).byteswap().l;
                    (encrypting ? r.k_6 : r.k_13) = ka.rotl(15).byteswap().r;
                    (encrypting ? r.kl_1 : r.kl_4) = ka.rotl(30).byteswap().l;
                    (encrypting ? r.kl_2 : r.kl_3) = ka.rotl(30).byteswap().r;
                    (encrypting ? r.k_7 : r.k_12) = kl.rotl(45).byteswap().l;
                    (encrypting ? r.k_8 : r.k_11) = kl.rotl(45).byteswap().r;
                    (encrypting ? r.k_9 : r.k_10) = ka.rotl(45).byteswap().l;
                    (encrypting ? r.k_10 : r.k_9) = kl.rotl(60).byteswap().r;
                    (encrypting ? r.k_11 : r.k_8) = ka.rotl(60).byteswap().l;
                    (encrypting ? r.k_12 : r.k_7) = ka.rotl(60).byteswap().r;
                    (encrypting ? r.kl_3 : r.kl_2) = kl.rotl(77).byteswap().l;
                    (encrypting ? r.kl_4 : r.kl_1) = kl.rotl(77).byteswap().r;
                    (encrypting ? r.k_13 : r.k_6) = kl.rotl(94).byteswap().l;
                    (encrypting ? r.k_14 : r.k_5) = kl.rotl(94).byteswap().r;
                    (encrypting ? r.k_15 : r.k_4) = ka.rotl(94).byteswap().l;
                    (encrypting ? r.k_16 : r.k_3) = ka.rotl(94).byteswap().r;
                    (encrypting ? r.k_17 : r.k_2) = kl.rotl(111).byteswap().l;
                    (encrypting ? r.k_18 : r.k_1) = kl.rotl(111).byteswap().r;
                    (encrypting ? r.kw_3 : r.kw_1) = ka.rotl(111).byteswap().l;
                    (encrypting ? r.kw_4 : r.kw_2) = ka.rotl(111).byteswap().r;
                }
                else
                {
                    // 192 or 256 bit key
                    (encrypting ? r.kw_1 : r.kw_3) = kl.rotl(0).byteswap().l;
                    (encrypting ? r.kw_2 : r.kw_4) = kl.rotl(0).byteswap().r;
                    (encrypting ? r.k_1 : r.k_24) = kb.rotl(0).byteswap().l;
                    (encrypting ? r.k_2 : r.k_23) = kb.rotl(0).byteswap().r;
                    (encrypting ? r.k_3 : r.k_22) = kr.rotl(15).byteswap().l;
                    (encrypting ? r.k_4 : r.k_21) = kr.rotl(15).byteswap().r;
                    (encrypting ? r.k_5 : r.k_20) = ka.rotl(15).byteswap().l;
                    (encrypting ? r.k_6 : r.k_19) = ka.rotl(15).byteswap().r;
                    (encrypting ? r.kl_1 : r.kl_6) = kr.rotl(30).byteswap().l;
                    (encrypting ? r.kl_2 : r.kl_5) = kr.rotl(30).byteswap().r;
                    (encrypting ? r.k_7 : r.k_18) = kb.rotl(30).byteswap().l;
                    (encrypting ? r.k_8 : r.k_17) = kb.rotl(30).byteswap().r;
                    (encrypting ? r.k_9 : r.k_16) = kl.rotl(45).byteswap().l;
                    (encrypting ? r.k_10 : r.k_15) = kl.rotl(45).byteswap().r;
                    (encrypting ? r.k_11 : r.k_14) = ka.rotl(45).byteswap().l;
                    (encrypting ? r.k_12 : r.k_13) = ka.rotl(45).byteswap().r;
                    (encrypting ? r.kl_3 : r.kl_4) = kl.rotl(60).byteswap().l;
                    (encrypting ? r.kl_4 : r.kl_3) = kl.rotl(60).byteswap().r;
                    (encrypting ? r.k_13 : r.k_12) = kr.rotl(60).byteswap().l;
                    (encrypting ? r.k_14 : r.k_11) = kr.rotl(60).byteswap().r;
                    (encrypting ? r.k_15 : r.k_10) = kb.rotl(60).byteswap().l;
                    (encrypting ? r.k_16 : r.k_9) = kb.rotl(60).byteswap().r;
                    (encrypting ? r.k_17 : r.k_8) = kl.rotl(77).byteswap().l;
                    (encrypting ? r.k_18 : r.k_7) = kl.rotl(77).byteswap().r;
                    (encrypting ? r.kl_5 : r.kl_2) = ka.rotl(77).byteswap().l;
                    (encrypting ? r.kl_6 : r.kl_1) = ka.rotl(77).byteswap().r;
                    (encrypting ? r.k_19 : r.k_6) = kr.rotl(94).byteswap().l;
                    (encrypting ? r.k_20 : r.k_5) = kr.rotl(94).byteswap().r;
                    (encrypting ? r.k_21 : r.k_4) = ka.rotl(94).byteswap().l;
                    (encrypting ? r.k_22 : r.k_3) = ka.rotl(94).byteswap().r;
                    (encrypting ? r.k_23 : r.k_2) = kl.rotl(111).byteswap().l;
                    (encrypting ? r.k_24 : r.k_1) = kl.rotl(111).byteswap().r;
                    (encrypting ? r.kw_3 : r.kw_1) = kb.rotl(111).byteswap().l;
                    (encrypting ? r.kw_4 : r.kw_2) = kb.rotl(111).byteswap().r;
                }
                return r;
            }

            template <
                class block_t = v128&,
                auto camellia_prewhite = functions::camellia_prewhite<v128&, key64>,
                auto camellia_f = functions::camellia_f_table_lookup<v64&, lookup_sbox32, lookup_sbox64, key64>,
                auto camellia_fl = functions::camellia_fl<v64&, rotl_be1, key64>,
                auto camellia_fl_inv = functions::camellia_fl_inv<v64&, rotl_be1, key64>,
                auto camellia_postwhite = functions::camellia_postwhite<v128&, key64>,
                class key64 = key64>
            static ARKANA_FORCEINLINE auto process_block_inlined(block_t block, const key_vector_small_t<key64>& kev) -> block_t
            {
                block = camellia_prewhite(block, kev.kw_1, kev.kw_2);
                auto&& [l, r] = decompose(block);
                r = camellia_f(r, l, kev.k_1);
                l = camellia_f(l, r, kev.k_2);
                r = camellia_f(r, l, kev.k_3);
                l = camellia_f(l, r, kev.k_4);
                r = camellia_f(r, l, kev.k_5);
                l = camellia_f(l, r, kev.k_6);
                l = camellia_fl(l, kev.kl_1);
                r = camellia_fl_inv(r, kev.kl_2);
                r = camellia_f(r, l, kev.k_7);
                l = camellia_f(l, r, kev.k_8);
                r = camellia_f(r, l, kev.k_9);
                l = camellia_f(l, r, kev.k_10);
                r = camellia_f(r, l, kev.k_11);
                l = camellia_f(l, r, kev.k_12);
                l = camellia_fl(l, kev.kl_3);
                r = camellia_fl_inv(r, kev.kl_4);
                r = camellia_f(r, l, kev.k_13);
                l = camellia_f(l, r, kev.k_14);
                r = camellia_f(r, l, kev.k_15);
                l = camellia_f(l, r, kev.k_16);
                r = camellia_f(r, l, kev.k_17);
                l = camellia_f(l, r, kev.k_18);
                block = recompose(block, l, r);
                return block = camellia_postwhite(block, kev.kw_3, kev.kw_4);
            }

            template <
                class block_t = v128&,
                auto camellia_prewhite = functions::camellia_prewhite<v128&, key64>,
                auto camellia_f = functions::camellia_f_table_lookup<v64&, lookup_sbox32, lookup_sbox64, key64>,
                auto camellia_fl = functions::camellia_fl<v64&, rotl_be1, key64>,
                auto camellia_fl_inv = functions::camellia_fl_inv<v64&, rotl_be1, key64>,
                auto camellia_postwhite = functions::camellia_postwhite<v128&, key64>,
                class key64 = key64>
            static ARKANA_FORCEINLINE auto process_block_inlined(block_t block, const key_vector_large_t<key64>& kev) -> block_t
            {
                block = camellia_prewhite(block, kev.kw_1, kev.kw_2);
                auto&& [l, r] = decompose(block);
                r = camellia_f(r, l, kev.k_1);
                l = camellia_f(l, r, kev.k_2);
                r = camellia_f(r, l, kev.k_3);
                l = camellia_f(l, r, kev.k_4);
                r = camellia_f(r, l, kev.k_5);
                l = camellia_f(l, r, kev.k_6);
                l = camellia_fl(l, kev.kl_1);
                r = camellia_fl_inv(r, kev.kl_2);
                r = camellia_f(r, l, kev.k_7);
                l = camellia_f(l, r, kev.k_8);
                r = camellia_f(r, l, kev.k_9);
                l = camellia_f(l, r, kev.k_10);
                r = camellia_f(r, l, kev.k_11);
                l = camellia_f(l, r, kev.k_12);
                l = camellia_fl(l, kev.kl_3);
                r = camellia_fl_inv(r, kev.kl_4);
                r = camellia_f(r, l, kev.k_13);
                l = camellia_f(l, r, kev.k_14);
                r = camellia_f(r, l, kev.k_15);
                l = camellia_f(l, r, kev.k_16);
                r = camellia_f(r, l, kev.k_17);
                l = camellia_f(l, r, kev.k_18);
                l = camellia_fl(l, kev.kl_5);
                r = camellia_fl_inv(r, kev.kl_6);
                r = camellia_f(r, l, kev.k_19);
                l = camellia_f(l, r, kev.k_20);
                r = camellia_f(r, l, kev.k_21);
                l = camellia_f(l, r, kev.k_22);
                r = camellia_f(r, l, kev.k_23);
                l = camellia_f(l, r, kev.k_24);
                block = recompose(block, l, r);
                return block = camellia_postwhite(block, kev.kw_3, kev.kw_4);
            }
        }

        inline namespace ecb_mode
        {
            // process_blocks_ecb
            template <
                class block_t = v128,
                auto load_block = load_u<v128>,
                auto camellia_prewhite = camellia_prewhite<v128&, key64>,
                auto camellia_f = camellia_f_table_lookup<v64&, lookup_sbox32, lookup_sbox64, key64>,
                auto camellia_fl = camellia_fl<v64&, rotl_be1, key64>,
                auto camellia_fl_inv = camellia_fl_inv<v64&, rotl_be1, key64>,
                auto camellia_postwhite = camellia_postwhite<v128&, key64>,
                auto store_block = store_u<v128>,
                class key_vector_t>
            static void process_blocks_ecb(void* dst, const void* src, size_t length, const key_vector_t& kv)
            {
                static_assert(std::is_trivial_v<block_t>);

                // check camellia block size.
                if (length % 16 != 0)
                    throw std::invalid_argument("invalid length. length must be multiple of 16.");

                constexpr auto f = [](block_t* dst, const block_t* src, size_t count, const auto& kv)
                {
                    for (size_t i = 0; i < count; i++)
                    {
                        block_t b = load_block(src + i);
                        b = process_block_inlined<block_t&, camellia_prewhite, camellia_f, camellia_fl, camellia_fl_inv, camellia_postwhite>(b, kv);
                        store_block(dst + i, b);
                    }
                };

                const size_t unit_count = length / sizeof(block_t);
                const size_t remain_bytes = length % sizeof(block_t);

                f(static_cast<block_t*>(dst), static_cast<const block_t*>(src), unit_count, kv);

                if (remain_bytes)
                {
                    block_t buf{};
                    memcpy(&buf, static_cast<const std::byte*>(src) + unit_count * sizeof(block_t), remain_bytes);
                    f(&buf, &buf, 1, kv);
                    memcpy(static_cast<std::byte*>(dst) + unit_count * sizeof(block_t), &buf, remain_bytes);
                }
            }
        }

        inline namespace ctr_mode
        {
            // type_traits

            template <class ctr_generator_t, class block_t = block_t, class = void> struct is_ctr_generator : std::false_type { };

            template <class ctr_generator_t, class block_t>
            struct is_ctr_generator<ctr_generator_t, block_t, std::enable_if_t<
                                        std::is_invocable_v<std::decay_t<ctr_generator_t>, size_t> &&
                                        std::is_trivially_copyable_v<std::invoke_result_t<std::decay_t<ctr_generator_t>, size_t>> &&
                                        sizeof(std::invoke_result_t<std::decay_t<ctr_generator_t>, size_t>) == sizeof(block_t)>> : std::true_type { };

            template <class ctr_generator_t, class block_t = block_t>
            static constexpr bool is_ctr_generator_v = is_ctr_generator<ctr_generator_t, block_t>::value;

            // process_bytes_ctr
            template <
                class block_t = v128,
                auto camellia_prewhite = camellia_prewhite<v128&, key64>,
                auto camellia_f = camellia_f_table_lookup<v64&, lookup_sbox32, lookup_sbox64, key64>,
                auto camellia_fl = camellia_fl<v64&, rotl_be1, key64>,
                auto camellia_fl_inv = camellia_fl_inv<v64&, rotl_be1, key64>,
                auto camellia_postwhite = camellia_postwhite<v128&, key64>,
                auto load_block = load_u<v128>,
                auto xor_block = xor_block<v128>,
                auto store_block = store_u<v128>,
                class key_vector_t,
                class ctr_generator_t, std::enable_if_t<is_ctr_generator_v<ctr_generator_t, block_t>>* = nullptr>
            static void process_bytes_ctr(
                void* dst,
                const void* src,
                size_t pos,
                size_t length,
                const key_vector_t& kv,
                ctr_generator_t&& load_ctr)
            {
                static_assert(std::is_trivial_v<block_t>);
                constexpr size_t block_size = sizeof(block_t);

                auto f = [&load_ctr](block_t* dst, const block_t* src, size_t start, size_t count, const auto& kv)
                {
                    for (size_t i = 0; i < count; i++)
                    {
                        block_t b = load_ctr(start + i);
                        b = process_block_inlined<block_t&, camellia_prewhite, camellia_f, camellia_fl, camellia_fl_inv, camellia_postwhite>(b, kv);
                        block_t d = load_block(src + i);
                        d = xor_block(d, b);
                        store_block(dst + i, d);
                    }
                };

                auto* src_ptr = static_cast<const byte_t*>(src);
                auto* dst_ptr = static_cast<byte_t*>(dst);

                size_t i = pos / block_size;

                // Processes first partial block if exists
                if (const size_t s = pos % block_size)
                {
                    block_t buf{};
                    const size_t sz = std::min(s + length, block_size) - s;
                    auto* buf_ptr = reinterpret_cast<byte_t*>(&buf) + s;

                    memcpy(buf_ptr, src_ptr, sz);
                    f(&buf, &buf, i, 1, kv);
                    memcpy(dst_ptr, buf_ptr, sz);

                    i += 1;
                    src_ptr += sz;
                    dst_ptr += sz;
                    length -= sz;
                }

                // Processes blocks
                if (const size_t unit_count = length / block_size)
                {
                    f(reinterpret_cast<block_t*>(dst_ptr), reinterpret_cast<const block_t*>(src_ptr), i, unit_count, kv);

                    i += unit_count;
                    src_ptr += unit_count * block_size;
                    dst_ptr += unit_count * block_size;
                    length -= unit_count * block_size;
                }

                // Processes last partial block if exists
                if (const size_t remain_bytes = length)
                {
                    block_t buf{};
                    auto* buf_ptr = reinterpret_cast<byte_t*>(&buf);

                    memcpy(buf_ptr, src_ptr, remain_bytes);
                    f(&buf, &buf, i, 1, kv);
                    memcpy(dst_ptr, buf_ptr, remain_bytes);

                    i += 1;
                    src_ptr += remain_bytes;
                    dst_ptr += remain_bytes;
                    length -= remain_bytes;
                }
            }

            // rfc5528 ctr-mode

            using ctr_iv_t = byte_array<8>;
            using ctr_nonce_t = byte_array<4>;

            struct ctr_vector_t
            {
                uint32_t n, ivl, ivr, ctr;
            };

            static inline ctr_vector_t generate_rfc5528_ctr_vector(const ctr_iv_t& iv, const ctr_nonce_t& nonce)
            {
                return {
                    load_u<uint32_t>(nonce.data() + 0),
                    load_u<uint32_t>(iv.data() + 0),
                    load_u<uint32_t>(iv.data() + 4),
                    0,
                };
            }

            static inline auto make_rfc5528_ctr_provider(const ctr_iv_t& iv, const ctr_nonce_t& nonce)
            {
                return [ctr0 = generate_rfc5528_ctr_vector(iv, nonce)](size_t index) -> v128
                {
                    auto v = ctr0;
                    v.ctr = bit::byteswap(static_cast<uint32_t>(index + 1));
                    return load_u<v128>(&v);
                };
            }
        }
    }

    namespace ref
    {
        namespace impl
        {
            using key_vector_small_t = functions::key_vector_small_t<functions::key64>;
            using key_vector_large_t = functions::key_vector_large_t<functions::key64>;
            using functions::generate_key_vector;

            // rfc3713 ecb-mode
            template <
                class key_vector_t, std::enable_if_t<is_any_of_v<key_vector_t, key_vector_small_t, key_vector_large_t>>* = nullptr
            >
            static inline auto process_blocks_ecb(void* dst, const void* src, size_t length, const key_vector_t& kv)
            {
                using namespace functions;
                ecb_mode::process_blocks_ecb<
                    v128,
                    load_u<v128>,
                    camellia_prewhite<v128&, key64>,
                    camellia_f_table_lookup<v64&, lookup_sbox32, lookup_sbox64, key64>,
                    camellia_fl<v64&, rotl_be1, key64>,
                    camellia_fl_inv<v64&, rotl_be1, key64>,
                    camellia_postwhite<v128&, key64>,
                    store_u<v128>>(dst, src, length, kv);
            }

            using functions::ctr_iv_t;
            using functions::ctr_nonce_t;
            using functions::ctr_vector_t;
            using functions::generate_rfc5528_ctr_vector;
            using functions::is_ctr_generator_v;

            // rfc5528 ctr-mode
            template <
                class key_vector_t, std::enable_if_t<is_any_of_v<key_vector_t, key_vector_small_t, key_vector_large_t>>* = nullptr
            >
            static inline void process_bytes_ctr(void* dst, const void* src, size_t position, size_t length, const key_vector_t& kv, const ctr_vector_t& cv)
            {
                // pre-prewhitening
                ctr_vector_t ctr0 = arkana::load_u<ctr_vector_t>(&kv);
                ctr0.n ^= cv.n;
                ctr0.ivl ^= cv.ivl;
                ctr0.ivr ^= cv.ivr;

                using namespace functions;
                ctr_mode::process_bytes_ctr<
                    v128,
                    camellia_thruwhite<v128&, key64>,
                    camellia_f_table_lookup<v64&, lookup_sbox32, lookup_sbox64, key64>,
                    camellia_fl<v64&, rotl_be1, key64>,
                    camellia_fl_inv<v64&, rotl_be1, key64>,
                    camellia_postwhite<v128&, key64>,
                    load_u<v128>,
                    xor_block<v128>,
                    store_u<v128>>
                (
                    dst, src, position, length, kv,
                    [ctr0](size_t index) -> v128
                    {
                        auto v = ctr0;
                        v.ctr ^= bit::byteswap(static_cast<uint32_t>(index + 1)); // prewhitening
                        return load_u<v128>(&v);
                    });
            }

            // custom ctr-mode
            template <
                class key_vector_t, std::enable_if_t<is_any_of_v<key_vector_t, key_vector_small_t, key_vector_large_t>>* = nullptr,
                class ctr_generator_t, std::enable_if_t<is_ctr_generator_v<ctr_generator_t>>* = nullptr>
            static inline void process_bytes_ctr(void* dst, const void* src, size_t position, size_t length, const key_vector_t& kv, ctr_generator_t&& ctr)
            {
                using namespace functions;
                ctr_mode::process_bytes_ctr<
                    v128,
                    camellia_prewhite<v128&, key64>,
                    camellia_f_table_lookup<v64&, lookup_sbox32, lookup_sbox64, key64>,
                    camellia_fl<v64&, rotl_be1, key64>,
                    camellia_fl_inv<v64&, rotl_be1, key64>,
                    camellia_postwhite<v128&, key64>,
                    load_u<v128>,
                    xor_block<v128>,
                    store_u<v128>>
                (
                    dst, src, position, length, kv,
                    [ctr = std::forward<decltype(ctr)>(ctr)](size_t index)
                    {
                        return bit_cast<v128>(ctr(index));
                    });
            }
        }

        // public API

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
        using impl::ctr_vector_t;

        static inline ctr_vector_t generate_ctr_vector(const ctr_iv_t& ctr_iv, const ctr_nonce_t& ctr_nonce) { return impl::generate_rfc5528_ctr_vector(ctr_iv, ctr_nonce); }
        static inline void process_bytes_ctr(void* dst, const void* src, size_t position, size_t length, const key_vector_small_t& kv, const ctr_vector_t& ctr) { return impl::process_bytes_ctr(dst, src, position, length, kv, ctr); }
        static inline void process_bytes_ctr(void* dst, const void* src, size_t position, size_t length, const key_vector_large_t& kv, const ctr_vector_t& ctr) { return impl::process_bytes_ctr(dst, src, position, length, kv, ctr); }
        template <class custom_ctr_generator_t, std::enable_if_t<impl::is_ctr_generator_v<custom_ctr_generator_t>> * = nullptr> static inline void process_bytes_ctr(void* dst, const void* src, size_t position, size_t length, const key_vector_small_t& kv, custom_ctr_generator_t&& ctr) { return impl::process_bytes_ctr(dst, src, position, length, kv, std::forward<custom_ctr_generator_t>(ctr)); }
        template <class custom_ctr_generator_t, std::enable_if_t<impl::is_ctr_generator_v<custom_ctr_generator_t>>* = nullptr> static inline void process_bytes_ctr(void* dst, const void* src, size_t position, size_t length, const key_vector_large_t& kv, custom_ctr_generator_t&& ctr) { return impl::process_bytes_ctr(dst, src, position, length, kv, std::forward<custom_ctr_generator_t>(ctr)); }
    }
}
