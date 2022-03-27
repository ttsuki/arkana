/// @file
/// @brief	arkana::crc32
///			- An implementation of CRC32
/// @author Copyright(c) 2020 ttsuki
/// 
/// This software is released under the MIT License.
/// https://opensource.org/licenses/MIT
///
/// This implementation based on
/// "Fast CRC Computation for Generic Polynomials Using PCLMULQDQ Instruction"
/// -- V. Gopal, E. Ozturk, J. Guilford, et al., 2009,
/// https://www.intel.com/content/dam/www/public/us/en/documents/white-papers/fast-crc-computation-generic-polynomials-pclmulqdq-paper.pdf

#pragma once

#include "../bits/xmm.h"
#include "crc32.h"

namespace arkana::crc32
{
    namespace avx2clmul
    {
        template <uint32_t polynomial>
        struct constants;

        template <>
        struct constants<0xEDB88320>
        {
            static inline constexpr uint64_t kP = 0x1'DB710641;
            static inline constexpr uint64_t k1 = 0x1'54442bd4;
            static inline constexpr uint64_t k2 = 0x1'c6e41596;
            static inline constexpr uint64_t k3 = 0x1'751997d0;
            static inline constexpr uint64_t k4 = 0x0'ccaa009e;
            static inline constexpr uint64_t k5 = 0x1'63cd6124;
            static inline constexpr uint64_t k6 = 0x1'db710640;
            static inline constexpr uint64_t kM = 0x1'f7011641;
        };

        template <uint32_t polynomial>
        static inline crc32_value_t calculate_crc32(const void* data, size_t length, crc32_value_t current = 0)
        {
            using namespace xmm;
            using state128_t = vx128x1;
            using state512_t = PAIR<PAIR<vx128x1>>;
            using constants = constants<polynomial>;

            const byte_t* p = static_cast<const byte_t*>(data);

            // process unaligned bytes
            constexpr size_t align_to = alignof(state128_t);
            if (size_t l = std::min<size_t>(length, static_cast<size_t>(-reinterpret_cast<intptr_t>(p) & (align_to - 1))))
            {
                current = ref::calculate_crc32<polynomial>(p, l, current);
                length -= l;
                p += l;
            }

            // process aligned 128-byte blocks
            if (length >= 16)
            {
                struct f
                {
                    static ARKXMM_INLINE state128_t ARKXMM_VECTORCALL fold_128(state128_t current, vu64x2 k)
                    {
                        return clmul<0, 0>(reinterpret<vu64x2>(current), k)
                            ^ clmul<1, 1>(reinterpret<vu64x2>(current), k);
                    }

                    static ARKXMM_INLINE state128_t ARKXMM_VECTORCALL read_128(const byte_t*& p, size_t& length)
                    {
                        state128_t t = load_s(reinterpret_cast<const state128_t*>(p));
                        p += sizeof(state128_t);
                        length -= sizeof(state128_t);
                        return t;
                    }

                    static ARKXMM_INLINE state512_t ARKXMM_VECTORCALL read_512(const byte_t*& p, size_t& length)
                    {
                        state512_t t = load_s(reinterpret_cast<const state512_t*>(p));
                        p += sizeof(state512_t);
                        length -= sizeof(state512_t);
                        return t;
                    }
                };

                state128_t state128 = reinterpret<state128_t>(u32x4(~current, 0, 0, 0));

                // fold by 4 loop
                if (length >= 64)
                {
                    state512_t state512 = {};
                    state512.l.l = state128;
                    state512.l.r = zero<state128_t>();
                    state512.r.l = zero<state128_t>();
                    state512.r.r = zero<state128_t>();

                    // read first block
                    state512 ^= f::read_512(p, length);

                    // fold by 4 loop
                    while (length >= 64)
                    {
                        const vu64x2 k1_k2 = u64x2(constants::k1, constants::k2);
                        prefetch_nta(p);

                        // calculates next state
                        state512.l.l = f::fold_128(state512.l.l, k1_k2);
                        state512.l.r = f::fold_128(state512.l.r, k1_k2);
                        state512.r.l = f::fold_128(state512.r.l, k1_k2);
                        state512.r.r = f::fold_128(state512.r.r, k1_k2);
                        state512 ^= f::read_512(p, length);
                    }

                    // fold 4x128 state into 1x128 state
                    {
                        const vu64x2 k3_k4 = u64x2(constants::k3, constants::k4);
                        state128 = state512.l.l;
                        state128 = f::fold_128(state128, k3_k4);
                        state128 ^= state512.l.r;
                        state128 = f::fold_128(state128, k3_k4);
                        state128 ^= state512.r.l;
                        state128 = f::fold_128(state128, k3_k4);
                        state128 ^= state512.r.r;
                    }
                }
                else
                {
                    // read first block
                    state128 ^= f::read_128(p, length);
                }

                // fold by 1 loop
                while (length >= 16)
                {
                    const vu64x2 k3_k4 = u64x2(constants::k3, constants::k4);
                    prefetch_nta(p);

                    // calculates next state
                    state128 = f::fold_128(state128, k3_k4);
                    state128 ^= f::read_128(p, length);
                }

                // fold 1x128 state into crc32
                {
                    // fold 128 state into 64 bit
                    const state128_t mask32 = reinterpret<state128_t>(u32x4(~0u, 0u, 0u, 0u));
                    const vu64x2 k4_k5 = u64x2(constants::k4, constants::k5);
                    const vu64x2 kP_kM = u64x2(constants::kM, constants::kP);
                    state128_t s128 = state128;                                 //  s128: 1234 (32bit x 4)
                    state128_t s96 =                                            //   s96:   ABC_  (new state)
                        clmul<0, 0>(reinterpret<vu64x2>(s128), k4_k5) ^         //   <- :   sss_  (fold of 12)
                        byte_shift_r_128<8>(s128);                              //   <- :   34__  (shift of 34)
                    state128_t s64 =                                            //   s64:    XY__ (new state)
                        clmul<0, 1>(reinterpret<vu64x2>(s96 & mask32), k4_k5) ^ //   <- :    ss__ (fold of A)
                        byte_shift_r_128<4>(s96);                               //   <- :    BC__  (shift of BC)

                    // Barrett reduction to 32-bits.
                    state128_t t1 = clmul<0, 0>(reinterpret<vu64x2>(s64 & mask32), kP_kM); // a
                    state128_t t2 = clmul<0, 1>(reinterpret<vu64x2>(t1 & mask32), kP_kM);  // b
                    state128_t r = t2 ^ s64;

                    current = ~static_cast<uint32_t>(_mm_extract_epi32(r.v, 1));
                }
            }

            // process remain bytes
            return ref::calculate_crc32<polynomial>(p, length, current);
        }
    }
}
