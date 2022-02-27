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

namespace arkana::camellia
{
    static bool AVX2_SUPPORTED = cpuid::cpu_supports::AVX2;
    static bool AESNI_SUPPORTED = cpuid::cpu_supports::AESNI;

    std::unique_ptr<ecb_context_t> create_ecb_encrypt_context(const key_128bit_t* key)
    {
        return (AVX2_SUPPORTED
                    ? (AESNI_SUPPORTED
                           ? create_ecb_encrypt_context_avx2aesni(key)
                           : create_ecb_encrypt_context_avx2(key))
                    : create_ecb_encrypt_context_ia32(key));
    }

    std::unique_ptr<ecb_context_t> create_ecb_encrypt_context(const key_192bit_t* key)
    {
        return (AVX2_SUPPORTED
                    ? (AESNI_SUPPORTED
                           ? create_ecb_encrypt_context_avx2aesni(key)
                           : create_ecb_encrypt_context_avx2(key))
                    : create_ecb_encrypt_context_ia32(key));
    }

    std::unique_ptr<ecb_context_t> create_ecb_encrypt_context(const key_256bit_t* key)
    {
        return (AVX2_SUPPORTED
                    ? (AESNI_SUPPORTED
                           ? create_ecb_encrypt_context_avx2aesni(key)
                           : create_ecb_encrypt_context_avx2(key))
                    : create_ecb_encrypt_context_ia32(key));
    }

    std::unique_ptr<ecb_context_t> create_ecb_decrypt_context(const key_128bit_t* key)
    {
        return (AVX2_SUPPORTED
                    ? (AESNI_SUPPORTED
                           ? create_ecb_decrypt_context_avx2aesni(key)
                           : create_ecb_decrypt_context_avx2(key))
                    : create_ecb_decrypt_context_ia32(key));
    }

    std::unique_ptr<ecb_context_t> create_ecb_decrypt_context(const key_192bit_t* key)
    {
        return (AVX2_SUPPORTED
                    ? (AESNI_SUPPORTED
                           ? create_ecb_decrypt_context_avx2aesni(key)
                           : create_ecb_decrypt_context_avx2(key))
                    : create_ecb_decrypt_context_ia32(key));
    }

    std::unique_ptr<ecb_context_t> create_ecb_decrypt_context(const key_256bit_t* key)
    {
        return (AVX2_SUPPORTED
                    ? (AESNI_SUPPORTED
                           ? create_ecb_decrypt_context_avx2aesni(key)
                           : create_ecb_decrypt_context_avx2(key))
                    : create_ecb_decrypt_context_ia32(key));
    }
}
