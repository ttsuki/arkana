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

#include "../camellia.h"

namespace arkana::camellia
{
    std::unique_ptr<ecb_context_t> create_ecb_encrypt_context_ia32(const key_128bit_t* key);
    std::unique_ptr<ecb_context_t> create_ecb_encrypt_context_ia32(const key_192bit_t* key);
    std::unique_ptr<ecb_context_t> create_ecb_encrypt_context_ia32(const key_256bit_t* key);
    std::unique_ptr<ecb_context_t> create_ecb_decrypt_context_ia32(const key_128bit_t* key);
    std::unique_ptr<ecb_context_t> create_ecb_decrypt_context_ia32(const key_192bit_t* key);
    std::unique_ptr<ecb_context_t> create_ecb_decrypt_context_ia32(const key_256bit_t* key);
    std::unique_ptr<ctr_context_t> create_ctr_context_ia32(const key_128bit_t* key, const ctr_iv_t* iv, const ctr_nonce_t* nonce);
    std::unique_ptr<ctr_context_t> create_ctr_context_ia32(const key_192bit_t* key, const ctr_iv_t* iv, const ctr_nonce_t* nonce);
    std::unique_ptr<ctr_context_t> create_ctr_context_ia32(const key_256bit_t* key, const ctr_iv_t* iv, const ctr_nonce_t* nonce);

    std::unique_ptr<ecb_context_t> create_ecb_encrypt_context_avx2(const key_128bit_t* key);
    std::unique_ptr<ecb_context_t> create_ecb_encrypt_context_avx2(const key_192bit_t* key);
    std::unique_ptr<ecb_context_t> create_ecb_encrypt_context_avx2(const key_256bit_t* key);
    std::unique_ptr<ecb_context_t> create_ecb_decrypt_context_avx2(const key_128bit_t* key);
    std::unique_ptr<ecb_context_t> create_ecb_decrypt_context_avx2(const key_192bit_t* key);
    std::unique_ptr<ecb_context_t> create_ecb_decrypt_context_avx2(const key_256bit_t* key);
    std::unique_ptr<ctr_context_t> create_ctr_context_avx2(const key_128bit_t* key, const ctr_iv_t* iv, const ctr_nonce_t* nonce);
    std::unique_ptr<ctr_context_t> create_ctr_context_avx2(const key_192bit_t* key, const ctr_iv_t* iv, const ctr_nonce_t* nonce);
    std::unique_ptr<ctr_context_t> create_ctr_context_avx2(const key_256bit_t* key, const ctr_iv_t* iv, const ctr_nonce_t* nonce);

    std::unique_ptr<ecb_context_t> create_ecb_encrypt_context_avx2aesni(const key_128bit_t* key);
    std::unique_ptr<ecb_context_t> create_ecb_encrypt_context_avx2aesni(const key_192bit_t* key);
    std::unique_ptr<ecb_context_t> create_ecb_encrypt_context_avx2aesni(const key_256bit_t* key);
    std::unique_ptr<ecb_context_t> create_ecb_decrypt_context_avx2aesni(const key_128bit_t* key);
    std::unique_ptr<ecb_context_t> create_ecb_decrypt_context_avx2aesni(const key_192bit_t* key);
    std::unique_ptr<ecb_context_t> create_ecb_decrypt_context_avx2aesni(const key_256bit_t* key);
    std::unique_ptr<ctr_context_t> create_ctr_context_avx2aesni(const key_128bit_t* key, const ctr_iv_t* iv, const ctr_nonce_t* nonce);
    std::unique_ptr<ctr_context_t> create_ctr_context_avx2aesni(const key_192bit_t* key, const ctr_iv_t* iv, const ctr_nonce_t* nonce);
    std::unique_ptr<ctr_context_t> create_ctr_context_avx2aesni(const key_256bit_t* key, const ctr_iv_t* iv, const ctr_nonce_t* nonce);
}
