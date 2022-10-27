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

#include <cstdint>
#include "../camellia.h"
#include "../ark/types.h"

namespace arkana::camellia
{
    struct key_vector_small_tag;
    struct key_vector_large_tag;
    struct ctr_vector_tag;
    using key_vector_small_t = tagged_memory_buffer<key_vector_small_tag, sizeof(uint64_t) * 26>;
    using key_vector_large_t = tagged_memory_buffer<key_vector_small_tag, sizeof(uint64_t) * 34>;
    using ctr_vector_t = tagged_memory_buffer<key_vector_small_tag, sizeof(uint32_t) * 4>;

    key_vector_small_t generate_key_vector_encrypt(const key_128bit_t* key);
    key_vector_large_t generate_key_vector_encrypt(const key_192bit_t* key);
    key_vector_large_t generate_key_vector_encrypt(const key_256bit_t* key);
    key_vector_small_t generate_key_vector_decrypt(const key_128bit_t* key);
    key_vector_large_t generate_key_vector_decrypt(const key_192bit_t* key);
    key_vector_large_t generate_key_vector_decrypt(const key_256bit_t* key);

    ctr_vector_t generate_ctr_vector(const ctr_iv_t* iv, const ctr_nonce_t* nonce);

    bool cpu_supports_ia32() noexcept;
    bool cpu_supports_avx2() noexcept;
    bool cpu_supports_avx2aesni() noexcept;

    void process_blocks_ecb_ia32(void* dst, const void* src, size_t length, const key_vector_small_t& kv);
    void process_blocks_ecb_ia32(void* dst, const void* src, size_t length, const key_vector_large_t& kv);
    void process_bytes_ctr_ia32(void* dst, const void* src, size_t position, size_t length, const key_vector_small_t& kv, const ctr_vector_t& cv);
    void process_bytes_ctr_ia32(void* dst, const void* src, size_t position, size_t length, const key_vector_large_t& kv, const ctr_vector_t& cv);

    void process_blocks_ecb_avx2(void* dst, const void* src, size_t length, const key_vector_small_t& kv);
    void process_blocks_ecb_avx2(void* dst, const void* src, size_t length, const key_vector_large_t& kv);
    void process_bytes_ctr_avx2(void* dst, const void* src, size_t position, size_t length, const key_vector_small_t& kv, const ctr_vector_t& cv);
    void process_bytes_ctr_avx2(void* dst, const void* src, size_t position, size_t length, const key_vector_large_t& kv, const ctr_vector_t& cv);

    void process_blocks_ecb_avx2aesni(void* dst, const void* src, size_t length, const key_vector_small_t& kv);
    void process_blocks_ecb_avx2aesni(void* dst, const void* src, size_t length, const key_vector_large_t& kv);
    void process_bytes_ctr_avx2aesni(void* dst, const void* src, size_t position, size_t length, const key_vector_small_t& kv, const ctr_vector_t& cv);
    void process_bytes_ctr_avx2aesni(void* dst, const void* src, size_t position, size_t length, const key_vector_large_t& kv, const ctr_vector_t& cv);

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
