/// @file
/// @brief	arkana::camellia
///			- An implementation of Camellia cipher
/// @author Copyright(c) 2021 ttsuki
/// 
/// This software is released under the MIT License.
/// https://opensource.org/licenses/MIT
///
/// - camellia https://info.isl.ntt.co.jp/crypt/camellia/ 

#include "./camellia.h"

namespace arkana::camellia
{
    inline std::unique_ptr<ecb_context_t> create_ecb_encrypt_context(const key_128bit_t* key)
    {
        if (cpu_supports_avx2aesni()) return create_ecb_encrypt_context_avx2aesni(key);
        if (cpu_supports_avx2()) return create_ecb_encrypt_context_avx2(key);
        return create_ecb_encrypt_context_ia32(key);
    }

    inline std::unique_ptr<ecb_context_t> create_ecb_encrypt_context(const key_192bit_t* key)
    {
        if (cpu_supports_avx2aesni()) return create_ecb_encrypt_context_avx2aesni(key);
        if (cpu_supports_avx2()) return create_ecb_encrypt_context_avx2(key);
        return create_ecb_encrypt_context_ia32(key);
    }

    inline std::unique_ptr<ecb_context_t> create_ecb_encrypt_context(const key_256bit_t* key)
    {
        if (cpu_supports_avx2aesni()) return create_ecb_encrypt_context_avx2aesni(key);
        if (cpu_supports_avx2()) return create_ecb_encrypt_context_avx2(key);
        return create_ecb_encrypt_context_ia32(key);
    }

    inline std::unique_ptr<ecb_context_t> create_ecb_decrypt_context(const key_128bit_t* key)
    {
        if (cpu_supports_avx2aesni()) return create_ecb_decrypt_context_avx2aesni(key);
        if (cpu_supports_avx2()) return create_ecb_decrypt_context_avx2(key);
        return create_ecb_decrypt_context_ia32(key);
    }

    inline std::unique_ptr<ecb_context_t> create_ecb_decrypt_context(const key_192bit_t* key)
    {
        if (cpu_supports_avx2aesni()) return create_ecb_decrypt_context_avx2aesni(key);
        if (cpu_supports_avx2()) return create_ecb_decrypt_context_avx2(key);
        return create_ecb_decrypt_context_ia32(key);
    }

    inline std::unique_ptr<ecb_context_t> create_ecb_decrypt_context(const key_256bit_t* key)
    {
        if (cpu_supports_avx2aesni()) return create_ecb_decrypt_context_avx2aesni(key);
        if (cpu_supports_avx2()) return create_ecb_decrypt_context_avx2(key);
        return create_ecb_decrypt_context_ia32(key);
    }

    inline std::unique_ptr<ctr_context_t> create_ctr_context(const key_128bit_t* key, const ctr_iv_t* iv, const ctr_nonce_t* nonce)
    {
        if (cpu_supports_avx2aesni()) return create_ctr_context_avx2aesni(key, iv, nonce);
        if (cpu_supports_avx2()) return create_ctr_context_avx2(key, iv, nonce);
        return create_ctr_context_ia32(key, iv, nonce);
    }

    inline std::unique_ptr<ctr_context_t> create_ctr_context(const key_192bit_t* key, const ctr_iv_t* iv, const ctr_nonce_t* nonce)
    {
        if (cpu_supports_avx2aesni()) return create_ctr_context_avx2aesni(key, iv, nonce);
        if (cpu_supports_avx2()) return create_ctr_context_avx2(key, iv, nonce);
        return create_ctr_context_ia32(key, iv, nonce);
    }

    inline std::unique_ptr<ctr_context_t> create_ctr_context(const key_256bit_t* key, const ctr_iv_t* iv, const ctr_nonce_t* nonce)
    {
        if (cpu_supports_avx2aesni()) return create_ctr_context_avx2aesni(key, iv, nonce);
        if (cpu_supports_avx2()) return create_ctr_context_avx2(key, iv, nonce);
        return create_ctr_context_ia32(key, iv, nonce);
    }
}
