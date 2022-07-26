/// @file
/// @brief	arkana::cpuid
///         - Cpu feature flags
/// @author Copyright(c) 2021 ttsuki
/// 
/// This software is released under the MIT License.
/// https://opensource.org/licenses/MIT
#pragma once

#if defined(_MSC_VER)
#include <intrin.h>
#else
#include <cpuid.h>
#endif

namespace arkana::cpuid
{
    namespace functions
    {
#if defined(_MSC_VER)
        static inline void cpuid(int f, int data[4]) { return __cpuid(data, f); }
        static inline void cpuidex(int f, int c, int data[4]) { return __cpuidex(data, f, c); }
#else
        static inline void cpuid(int f, int data[4]) { __cpuid(f, data[0], data[1], data[2], data[3]); }
        static inline void cpuidex(int f, int c, int data[4]) { __cpuid_count(f, c, data[0], data[1], data[2], data[3]); }
#endif

        static inline bool cpuflag(int f, int r, int i)
        {
            int x[4];
            cpuid(0, x);
            if (x[0] < f) { return false; }
            cpuidex(f, 0, x);
            return x[r] >> i & 1;
        }
    }

	namespace cpu_supports
	{
        using functions::cpuflag;
		static inline const bool MMX = cpuflag(1, 3, 23);
		static inline const bool SSE = cpuflag(1, 3, 25);
		static inline const bool SSE2 = cpuflag(1, 3, 26);
		static inline const bool SSE3 = cpuflag(1, 2, 0);
		static inline const bool PCLMULQDQ = cpuflag(1, 2, 1);
		static inline const bool SSSE3 = cpuflag(1, 2, 9);
		static inline const bool SSE41 = cpuflag(1, 2, 19);
		static inline const bool SSE42 = cpuflag(1, 2, 20);
		static inline const bool AESNI = cpuflag(1, 2, 25);
		static inline const bool AVX = cpuflag(1, 2, 28);
		static inline const bool BMI = cpuflag(7, 1, 3);
		static inline const bool AVX2 = cpuflag(7, 1, 5);
		static inline const bool BMI2 = cpuflag(7, 1, 8);
	}
}
