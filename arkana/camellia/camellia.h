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

#include "../bits/types.h"
#include "../bits/memory.h"
#include "../bits/bit-manip.h"
#include "../bits/table.h"

namespace arkana::camellia
{
    namespace functions
    {
        template <class T> using sbox = table<T, 256>;

        template <class T>
        struct key_vector_128_t
        {
            T kw_1, kw_2;
            T k_1, k_2;
            T k_3, k_4;
            T k_5, k_6;
            T kl_1, kl_2;
            T k_7, k_8;
            T k_9, k_10;
            T k_11, k_12;
            T kl_3, kl_4;
            T k_13, k_14;
            T k_15, k_16;
            T k_17, k_18;
            T kw_3, kw_4;
        };

        template <class T>
        struct key_vector_256_t
        {
            T kw_1, kw_2;
            T k_1, k_2;
            T k_3, k_4;
            T k_5, k_6;
            T kl_1, kl_2;
            T k_7, k_8;
            T k_9, k_10;
            T k_11, k_12;
            T kl_3, kl_4;
            T k_13, k_14;
            T k_15, k_16;
            T k_17, k_18;
            T kl_5, kl_6;
            T k_19, k_20;
            T k_21, k_22;
            T k_23, k_24;
            T kw_3, kw_4;
        };

        template <class T> static inline constexpr auto decompose(T x) noexcept;
        template <class T, class U> static inline constexpr auto recompose(U l, U r) noexcept -> T;
        template <class T> static inline constexpr T lookup_sbox(const sbox<T>& sbox, T index, int shift) noexcept;

        template <> inline constexpr auto decompose<uint64_t>(uint64_t x) noexcept { return array<uint32_t, 2>{static_cast<uint32_t>(x), static_cast<uint32_t>(x >> 32)}; }
        template <> inline constexpr uint64_t recompose<uint64_t, uint32_t>(uint32_t l, uint32_t r) noexcept { return static_cast<uint64_t>(r) << 32 | l; }
        template <> inline constexpr uint32_t lookup_sbox<uint32_t>(const sbox<uint32_t>& sbox, uint32_t index, int shift) noexcept { return sbox[index >> 8 * shift & 0xFF]; }
        template <> inline constexpr uint64_t lookup_sbox<uint64_t>(const sbox<uint64_t>& sbox, uint64_t index, int shift) noexcept { return sbox[index >> 8 * shift & 0xFF]; }

        static inline constexpr table<uint8_t, 256> sbox_0 = {
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

        static inline constexpr auto sbox_1 = generate_table<uint8_t, 256>([](auto i) { return static_cast<uint8_t>(sbox_0[i] << 1 | sbox_0[i] >> 7); });
        static inline constexpr auto sbox_2 = generate_table<uint8_t, 256>([](auto i) { return static_cast<uint8_t>(sbox_0[i] << 7 | sbox_0[i] >> 1); });
        static inline constexpr auto sbox_3 = generate_table<uint8_t, 256>([](auto i) { return sbox_0[static_cast<uint8_t>(i << 1 | i >> 7)]; });

        static inline constexpr auto sbox_32_0 = generate_table<uint32_t, 256>([](auto i) { return 0x00010101u * sbox_0[i]; });
        static inline constexpr auto sbox_32_1 = generate_table<uint32_t, 256>([](auto i) { return 0x01010100u * sbox_1[i]; });
        static inline constexpr auto sbox_32_2 = generate_table<uint32_t, 256>([](auto i) { return 0x01010001u * sbox_2[i]; });
        static inline constexpr auto sbox_32_3 = generate_table<uint32_t, 256>([](auto i) { return 0x01000101u * sbox_3[i]; });

        static inline constexpr auto sbox_64_0 = generate_table<uint64_t, 256>([](auto i) { return 0x0100000100010101u * sbox_0[i]; });
        static inline constexpr auto sbox_64_1 = generate_table<uint64_t, 256>([](auto i) { return 0x0000010101010100u * sbox_1[i]; });
        static inline constexpr auto sbox_64_2 = generate_table<uint64_t, 256>([](auto i) { return 0x0001010001010001u * sbox_2[i]; });
        static inline constexpr auto sbox_64_3 = generate_table<uint64_t, 256>([](auto i) { return 0x0101000001000101u * sbox_3[i]; });
        static inline constexpr auto sbox_64_4 = generate_table<uint64_t, 256>([](auto i) { return 0x0101010001010100u * sbox_1[i]; });
        static inline constexpr auto sbox_64_5 = generate_table<uint64_t, 256>([](auto i) { return 0x0101000101010001u * sbox_2[i]; });
        static inline constexpr auto sbox_64_6 = generate_table<uint64_t, 256>([](auto i) { return 0x0100010101000101u * sbox_3[i]; });
        static inline constexpr auto sbox_64_7 = generate_table<uint64_t, 256>([](auto i) { return 0x0001010100010101u * sbox_0[i]; });


        template <class uint64_t>
        static inline constexpr uint64_t camellia_f_table_lookup_32(uint64_t l, uint64_t r, uint64_t k) noexcept
        {
            auto [rl, rr] = decompose(r);
            auto [kl, kr] = decompose(k);
            using uint32_t = decltype(rl);

            uint32_t d = 0;
            uint32_t xr = rr ^ kr;
            d ^= lookup_sbox(sbox_32_1, xr, 0);
            d ^= lookup_sbox(sbox_32_2, xr, 1);
            d ^= lookup_sbox(sbox_32_3, xr, 2);
            d ^= lookup_sbox(sbox_32_0, xr, 3);

            uint32_t u = 0;
            uint32_t xl = rl ^ kl;
            u ^= lookup_sbox(sbox_32_0, xl, 0);
            u ^= lookup_sbox(sbox_32_1, xl, 1);
            u ^= lookup_sbox(sbox_32_2, xl, 2);
            u ^= lookup_sbox(sbox_32_3, xl, 3);

            uint32_t dd = d ^ u;
            uint32_t uu = dd ^ (u << 8 | u >> 24);

            return l ^ recompose<uint64_t>(dd, uu);
        }

        template <class uint64_t>
        static inline constexpr uint64_t camellia_f_table_lookup_64(uint64_t l, uint64_t r, uint64_t k) noexcept
        {
            uint64_t xl = r ^ k;
            return l
                ^ lookup_sbox(sbox_64_0, xl, 0)
                ^ lookup_sbox(sbox_64_1, xl, 1)
                ^ lookup_sbox(sbox_64_2, xl, 2)
                ^ lookup_sbox(sbox_64_3, xl, 3)
                ^ lookup_sbox(sbox_64_4, xl, 4)
                ^ lookup_sbox(sbox_64_5, xl, 5)
                ^ lookup_sbox(sbox_64_6, xl, 6)
                ^ lookup_sbox(sbox_64_7, xl, 7);
        }

        template <class uint64_t>
        static inline constexpr uint64_t camellia_f_table_lookup(uint64_t l, uint64_t r, uint64_t k) noexcept
        {
            if constexpr (sizeof(void*) == 8)
                return camellia_f_table_lookup_64(l, r, k);
            else
                return camellia_f_table_lookup_32(l, r, k);
        }

        template <class uint32_t>
        static inline constexpr uint32_t rotl_be1(uint32_t v) noexcept
        {
            /* Big endian rotl(v, 1) */     //  BE: 01234567 89abcdef ghijklmn opqrstuv
            /* on little endian      */     //  ->: 12345678 9abcdefg hijklmno pqrstuv0
            uint32_t x = v & 0x80808080u;   //x   = o_______ g_______ 8_______ 0_______ (& 0x80808080)
            uint32_t y = v & ~0x80808080u;  //  y = _pqrstuv _hijklmn _9abcdef _1234567 (& ~0x80808080)
            uint32_t a = x << 17 | x >> 15; //a   = _______0 _______o _______g _______8 (& 0x80808080 <<< 17)
            uint32_t b = y << 1;            //  b = pqrstuv_ hijklmn_ 9abcdef_ 1234567_ (& ~0x80808080 << 1)
            return a | b;                   //a|b = pqrstuv0 hijklmno 9abcdefg 12345678
        }

        template <class uint64_t>
        static inline constexpr uint64_t camellia_fl(uint64_t l, uint64_t k) noexcept
        {
            auto [ll, lr] = decompose(l);
            auto [kl, kr] = decompose(k);
            lr ^= rotl_be1(ll & kl);
            ll ^= (lr | kr);
            return recompose<uint64_t>(ll, lr);
        }

        template <class uint64_t>
        static inline constexpr uint64_t camellia_fl_inv(uint64_t r, uint64_t k) noexcept
        {
            auto [rl, rr] = decompose(r);
            auto [kl, kr] = decompose(k);
            rl ^= (rr | kr);
            rr ^= rotl_be1(rl & kl);
            return recompose<uint64_t>(rl, rr);
        }

        template <size_t key_length, bool for_encryption>
        static auto generate_key_vector(byte_array<key_length / 8> key)
        -> std::enable_if_t<key_length == 128 || key_length == 192 || key_length == 256, std::conditional_t<key_length == 128, key_vector_128_t<uint64_t>, key_vector_256_t<uint64_t>>>
        {
            struct uint128_t
            {
                uint64_t l, r;
                uint128_t operator ^(uint128_t rhs) const noexcept { return uint128_t{l ^ rhs.l, r ^ rhs.r}; }
                uint128_t byteswap() const noexcept { return uint128_t{bits::byteswap(r), bits::byteswap(l)}; }

                uint128_t rotl(int i) const noexcept
                {
                    auto xl = (i & 63) ? l << (i & 63) | r >> (-i & 63) : l;
                    auto xr = (i & 63) ? r << (i & 63) | l >> (-i & 63) : r;
                    return i & 64 ? uint128_t{xr, xl} : uint128_t{xl, xr};
                }
            };

            auto [kl, kr, ka, kb] = [key]
            {
                uint128_t kl{};
                uint128_t kr{};

                if constexpr (key_length == 128)
                {
                    kl = load_u<uint128_t>(key.data() + 0);
                    kr = {};
                }
                else if constexpr (key_length == 192)
                {
                    kl = load_u<uint128_t>(key.data() + 0);
                    kr.l = load_u<uint64_t>(key.data() + 16);
                    kr.r = ~kr.l;
                }
                else if constexpr (key_length == 256)
                {
                    kl = load_u<uint128_t>(key.data() + 0);
                    kr = load_u<uint128_t>(key.data() + 16);
                }

                constexpr auto camellia_2xf = [](uint128_t x, uint128_t key_pair) noexcept -> uint128_t
                {
                    x.r = camellia_f_table_lookup(x.r, x.l, key_pair.l);
                    x.l = camellia_f_table_lookup(x.l, x.r, key_pair.r);
                    return x;
                };

                uint128_t kt = camellia_2xf(kl ^ kr, {bits::byteswap(0xA09E667F3BCC908Bu), bits::byteswap(0xB67AE8584CAA73B2u)});
                uint128_t ka = camellia_2xf(kt ^ kl, {bits::byteswap(0xC6EF372FE94F82BEu), bits::byteswap(0x54FF53A5F1D36F1Cu)});
                uint128_t kb = camellia_2xf(kr ^ ka, {bits::byteswap(0x10E527FADE682D1Du), bits::byteswap(0xB05688C2B3E6C1FDu)});

                return array<uint128_t, 4>{kl, kr, ka, kb};
            }();

            if constexpr (key_length == 128)
            {
                key_vector_128_t<uint64_t> r{};
                (for_encryption ? r.kw_1 : r.kw_3) = kl.byteswap().rotl(0).byteswap().l;
                (for_encryption ? r.kw_2 : r.kw_4) = kl.byteswap().rotl(0).byteswap().r;
                (for_encryption ? r.k_1 : r.k_18) = ka.byteswap().rotl(0).byteswap().l;
                (for_encryption ? r.k_2 : r.k_17) = ka.byteswap().rotl(0).byteswap().r;
                (for_encryption ? r.k_3 : r.k_16) = kl.byteswap().rotl(15).byteswap().l;
                (for_encryption ? r.k_4 : r.k_15) = kl.byteswap().rotl(15).byteswap().r;
                (for_encryption ? r.k_5 : r.k_14) = ka.byteswap().rotl(15).byteswap().l;
                (for_encryption ? r.k_6 : r.k_13) = ka.byteswap().rotl(15).byteswap().r;
                (for_encryption ? r.kl_1 : r.kl_4) = ka.byteswap().rotl(30).byteswap().l;
                (for_encryption ? r.kl_2 : r.kl_3) = ka.byteswap().rotl(30).byteswap().r;
                (for_encryption ? r.k_7 : r.k_12) = kl.byteswap().rotl(45).byteswap().l;
                (for_encryption ? r.k_8 : r.k_11) = kl.byteswap().rotl(45).byteswap().r;
                (for_encryption ? r.k_9 : r.k_10) = ka.byteswap().rotl(45).byteswap().l;
                (for_encryption ? r.k_10 : r.k_9) = kl.byteswap().rotl(60).byteswap().r;
                (for_encryption ? r.k_11 : r.k_8) = ka.byteswap().rotl(60).byteswap().l;
                (for_encryption ? r.k_12 : r.k_7) = ka.byteswap().rotl(60).byteswap().r;
                (for_encryption ? r.kl_3 : r.kl_2) = kl.byteswap().rotl(77).byteswap().l;
                (for_encryption ? r.kl_4 : r.kl_1) = kl.byteswap().rotl(77).byteswap().r;
                (for_encryption ? r.k_13 : r.k_6) = kl.byteswap().rotl(94).byteswap().l;
                (for_encryption ? r.k_14 : r.k_5) = kl.byteswap().rotl(94).byteswap().r;
                (for_encryption ? r.k_15 : r.k_4) = ka.byteswap().rotl(94).byteswap().l;
                (for_encryption ? r.k_16 : r.k_3) = ka.byteswap().rotl(94).byteswap().r;
                (for_encryption ? r.k_17 : r.k_2) = kl.byteswap().rotl(111).byteswap().l;
                (for_encryption ? r.k_18 : r.k_1) = kl.byteswap().rotl(111).byteswap().r;
                (for_encryption ? r.kw_3 : r.kw_1) = ka.byteswap().rotl(111).byteswap().l;
                (for_encryption ? r.kw_4 : r.kw_2) = ka.byteswap().rotl(111).byteswap().r;
                return r;
            }
            else
            {
                key_vector_256_t<uint64_t> r{};
                (for_encryption ? r.kw_1 : r.kw_3) = kl.byteswap().rotl(0).byteswap().l;
                (for_encryption ? r.kw_2 : r.kw_4) = kl.byteswap().rotl(0).byteswap().r;
                (for_encryption ? r.k_1 : r.k_24) = kb.byteswap().rotl(0).byteswap().l;
                (for_encryption ? r.k_2 : r.k_23) = kb.byteswap().rotl(0).byteswap().r;
                (for_encryption ? r.k_3 : r.k_22) = kr.byteswap().rotl(15).byteswap().l;
                (for_encryption ? r.k_4 : r.k_21) = kr.byteswap().rotl(15).byteswap().r;
                (for_encryption ? r.k_5 : r.k_20) = ka.byteswap().rotl(15).byteswap().l;
                (for_encryption ? r.k_6 : r.k_19) = ka.byteswap().rotl(15).byteswap().r;
                (for_encryption ? r.kl_1 : r.kl_6) = kr.byteswap().rotl(30).byteswap().l;
                (for_encryption ? r.kl_2 : r.kl_5) = kr.byteswap().rotl(30).byteswap().r;
                (for_encryption ? r.k_7 : r.k_18) = kb.byteswap().rotl(30).byteswap().l;
                (for_encryption ? r.k_8 : r.k_17) = kb.byteswap().rotl(30).byteswap().r;
                (for_encryption ? r.k_9 : r.k_16) = kl.byteswap().rotl(45).byteswap().l;
                (for_encryption ? r.k_10 : r.k_15) = kl.byteswap().rotl(45).byteswap().r;
                (for_encryption ? r.k_11 : r.k_14) = ka.byteswap().rotl(45).byteswap().l;
                (for_encryption ? r.k_12 : r.k_13) = ka.byteswap().rotl(45).byteswap().r;
                (for_encryption ? r.kl_3 : r.kl_4) = kl.byteswap().rotl(60).byteswap().l;
                (for_encryption ? r.kl_4 : r.kl_3) = kl.byteswap().rotl(60).byteswap().r;
                (for_encryption ? r.k_13 : r.k_12) = kr.byteswap().rotl(60).byteswap().l;
                (for_encryption ? r.k_14 : r.k_11) = kr.byteswap().rotl(60).byteswap().r;
                (for_encryption ? r.k_15 : r.k_10) = kb.byteswap().rotl(60).byteswap().l;
                (for_encryption ? r.k_16 : r.k_9) = kb.byteswap().rotl(60).byteswap().r;
                (for_encryption ? r.k_17 : r.k_8) = kl.byteswap().rotl(77).byteswap().l;
                (for_encryption ? r.k_18 : r.k_7) = kl.byteswap().rotl(77).byteswap().r;
                (for_encryption ? r.kl_5 : r.kl_2) = ka.byteswap().rotl(77).byteswap().l;
                (for_encryption ? r.kl_6 : r.kl_1) = ka.byteswap().rotl(77).byteswap().r;
                (for_encryption ? r.k_19 : r.k_6) = kr.byteswap().rotl(94).byteswap().l;
                (for_encryption ? r.k_20 : r.k_5) = kr.byteswap().rotl(94).byteswap().r;
                (for_encryption ? r.k_21 : r.k_4) = ka.byteswap().rotl(94).byteswap().l;
                (for_encryption ? r.k_22 : r.k_3) = ka.byteswap().rotl(94).byteswap().r;
                (for_encryption ? r.k_23 : r.k_2) = kl.byteswap().rotl(111).byteswap().l;
                (for_encryption ? r.k_24 : r.k_1) = kl.byteswap().rotl(111).byteswap().r;
                (for_encryption ? r.kw_3 : r.kw_1) = kb.byteswap().rotl(111).byteswap().l;
                (for_encryption ? r.kw_4 : r.kw_2) = kb.byteswap().rotl(111).byteswap().r;
                return r;
            }
        }

        template <size_t key_length>
        static auto prepare_key_vector(const byte_array<key_length>& key, bool for_encryption)
        {
            return for_encryption
                       ? generate_key_vector<key_length * 8, true>(key)
                       : generate_key_vector<key_length * 8, false>(key);
        }

        template <class block_t, class key_element_t, auto camellia_prewhite, auto camellia_f, auto camellia_fl, auto camellia_fl_inv, auto camellia_postwhite>
        static block_t process_block(block_t input, const key_vector_128_t<key_element_t>& kev)
        {
            auto&& [l, r] = camellia_prewhite(input, kev.kw_1, kev.kw_2);
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
            return camellia_postwhite(l, r, kev.kw_3, kev.kw_4);
        }

        template <class block_t, class key_element_t, auto camellia_prewhite, auto camellia_f, auto camellia_fl, auto camellia_fl_inv, auto camellia_postwhite>
        static block_t process_block(block_t input, const key_vector_256_t<key_element_t>& kev)
        {
            auto&& [l, r] = camellia_prewhite(input, kev.kw_1, kev.kw_2);
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
            return camellia_postwhite(l, r, kev.kw_3, kev.kw_4);
        }
    }

    using key_vector_128_t = functions::key_vector_128_t<uint64_t>;
    using key_vector_256_t = functions::key_vector_256_t<uint64_t>;
    using functions::prepare_key_vector;

    struct block_t
    {
        uint64_t l, r;
    };

    template <class key_vector_t>
    static block_t process_block_ecb(block_t input, const key_vector_t& kev)
    {
        struct impl
        {
            static inline block_t camellia_prewhite(block_t b, uint64_t kl, uint64_t kr) { return {b.l ^ kl, b.r ^ kr}; }
            static inline block_t camellia_postwhite(uint64_t l, uint64_t r, uint64_t kl, uint64_t kr) { return {r ^ kl, l ^ kr}; }
        };

        return functions::process_block<
            block_t,
            uint64_t,
            impl::camellia_prewhite,
            functions::camellia_f_table_lookup<uint64_t>,
            functions::camellia_fl<uint64_t>,
            functions::camellia_fl_inv<uint64_t>,
            impl::camellia_postwhite
        >(input, kev);
    }
}
