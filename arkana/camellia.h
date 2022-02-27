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

#include <cstddef>
#include <array>
#include <memory>

namespace arkana::camellia
{
    using key_128bit_t = std::array<std::byte, 128 / 8>;
    using key_192bit_t = std::array<std::byte, 192 / 8>;
    using key_256bit_t = std::array<std::byte, 256 / 8>;
    using block_t = std::array<std::byte, 16>;

    class ecb_context_t
    {
    public:
        ecb_context_t() = default;
        ecb_context_t(const ecb_context_t& other) = default;
        ecb_context_t(ecb_context_t&& other) noexcept = default;
        ecb_context_t& operator=(const ecb_context_t& other) = default;
        ecb_context_t& operator=(ecb_context_t&& other) noexcept = default;
        virtual ~ecb_context_t() = default;

    public:
        virtual void process_blocks(void* dst, const void* src, size_t length) = 0;
    };

    std::unique_ptr<ecb_context_t> create_ecb_encrypt_context(const key_128bit_t* key);
    std::unique_ptr<ecb_context_t> create_ecb_encrypt_context(const key_192bit_t* key);
    std::unique_ptr<ecb_context_t> create_ecb_encrypt_context(const key_256bit_t* key);
    std::unique_ptr<ecb_context_t> create_ecb_decrypt_context(const key_128bit_t* key);
    std::unique_ptr<ecb_context_t> create_ecb_decrypt_context(const key_192bit_t* key);
    std::unique_ptr<ecb_context_t> create_ecb_decrypt_context(const key_256bit_t* key);

    std::unique_ptr<ecb_context_t> create_ecb_encrypt_context_ia32(const key_128bit_t* key);
    std::unique_ptr<ecb_context_t> create_ecb_encrypt_context_ia32(const key_192bit_t* key);
    std::unique_ptr<ecb_context_t> create_ecb_encrypt_context_ia32(const key_256bit_t* key);
    std::unique_ptr<ecb_context_t> create_ecb_decrypt_context_ia32(const key_128bit_t* key);
    std::unique_ptr<ecb_context_t> create_ecb_decrypt_context_ia32(const key_192bit_t* key);
    std::unique_ptr<ecb_context_t> create_ecb_decrypt_context_ia32(const key_256bit_t* key);

    std::unique_ptr<ecb_context_t> create_ecb_encrypt_context_avx2(const key_128bit_t* key);
    std::unique_ptr<ecb_context_t> create_ecb_encrypt_context_avx2(const key_192bit_t* key);
    std::unique_ptr<ecb_context_t> create_ecb_encrypt_context_avx2(const key_256bit_t* key);
    std::unique_ptr<ecb_context_t> create_ecb_decrypt_context_avx2(const key_128bit_t* key);
    std::unique_ptr<ecb_context_t> create_ecb_decrypt_context_avx2(const key_192bit_t* key);
    std::unique_ptr<ecb_context_t> create_ecb_decrypt_context_avx2(const key_256bit_t* key);

    std::unique_ptr<ecb_context_t> create_ecb_encrypt_context_avx2aesni(const key_128bit_t* key);
    std::unique_ptr<ecb_context_t> create_ecb_encrypt_context_avx2aesni(const key_192bit_t* key);
    std::unique_ptr<ecb_context_t> create_ecb_encrypt_context_avx2aesni(const key_256bit_t* key);
    std::unique_ptr<ecb_context_t> create_ecb_decrypt_context_avx2aesni(const key_128bit_t* key);
    std::unique_ptr<ecb_context_t> create_ecb_decrypt_context_avx2aesni(const key_192bit_t* key);
    std::unique_ptr<ecb_context_t> create_ecb_decrypt_context_avx2aesni(const key_256bit_t* key);

    static_assert(std::is_pod_v<block_t> && sizeof(block_t) == sizeof(std::byte) * block_t{}.size());
}
