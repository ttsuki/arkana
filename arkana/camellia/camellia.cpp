/// @file
/// @brief	arkana::camellia
///			- An implementation of Camellia cipher
/// @author Copyright(c) 2021 ttsuki
/// 
/// This software is released under the MIT License.
/// https://opensource.org/licenses/MIT
///
/// - camellia https://info.isl.ntt.co.jp/crypt/camellia/ 

#include "../camellia.h"

#include "../bits/cpuid.h"

std::unique_ptr<arkana::camellia::ecb_context_t> arkana::camellia::create_ecb_context(const key_128bit_t* key, bool encrypt)
{
    if (arkana::cpuid::cpu_supports::AVX2 && arkana::cpuid::cpu_supports::AESNI)
        return create_ecb_context_avx2aesni(key, encrypt);

    if (arkana::cpuid::cpu_supports::AVX2)
        return create_ecb_context_avx2(key, encrypt);

    return create_ecb_context_ia32(key, encrypt);
}

std::unique_ptr<arkana::camellia::ecb_context_t> arkana::camellia::create_ecb_context(const key_192bit_t* key, bool encrypt)
{
    if (arkana::cpuid::cpu_supports::AVX2 && arkana::cpuid::cpu_supports::AESNI)
        return create_ecb_context_avx2aesni(key, encrypt);

    if (arkana::cpuid::cpu_supports::AVX2)
        return create_ecb_context_avx2(key, encrypt);

    return create_ecb_context_ia32(key, encrypt);
}

std::unique_ptr<arkana::camellia::ecb_context_t> arkana::camellia::create_ecb_context(const key_256bit_t* key, bool encrypt)
{
    if (arkana::cpuid::cpu_supports::AVX2 && arkana::cpuid::cpu_supports::AESNI)
        return create_ecb_context_avx2aesni(key, encrypt);

    if (arkana::cpuid::cpu_supports::AVX2)
        return create_ecb_context_avx2(key, encrypt);

    return create_ecb_context_ia32(key, encrypt);
}
