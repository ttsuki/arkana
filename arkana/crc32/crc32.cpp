/// @file
/// @brief	arkana::crc32
///			- An implementation of CRC32
/// @author Copyright(c) 2020 ttsuki
/// 
/// This software is released under the MIT License.
/// https://opensource.org/licenses/MIT

#include "../crc32.h"

#include "../bits/cpuid.h"

std::unique_ptr<arkana::crc32::crc32_context_t> arkana::crc32::create_crc32_context(crc32_value_t initial)
{
    if (arkana::cpuid::cpu_supports::AVX2 && arkana::cpuid::cpu_supports::PCLMULQDQ)
        return create_crc32_context_avx2clmul(initial);

    if (arkana::cpuid::cpu_supports::AVX2)
        return create_crc32_context_avx2(initial);

    return create_crc32_context_ia32(initial);
}
