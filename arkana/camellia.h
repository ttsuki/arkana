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
    using key_128bit_t = std::array<std::byte, 128 / CHAR_BIT>;
    using key_192bit_t = std::array<std::byte, 192 / CHAR_BIT>;
    using key_256bit_t = std::array<std::byte, 256 / CHAR_BIT>;

    using block_t = std::array<std::byte, 16>;
    static_assert(std::is_trivial_v<block_t> && sizeof(block_t) == 16);

    using ctr_iv_t = std::array<std::byte, 8>;
    using ctr_nonce_t = std::array<std::byte, 4>;

    /// RFC 3713 context
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
        // Process blocks.
        //   dst: destination buffer.
        //   src: source buffer.
        //   length: length in bytes to process must be multiple sizeof block (=16).
        virtual void process_blocks(void* dst, const void* src, size_t length) = 0;
    };

    /// RFC 5528 context
    class ctr_context_t
    {
    public:
        ctr_context_t() = default;
        ctr_context_t(const ctr_context_t& other) = default;
        ctr_context_t(ctr_context_t&& other) noexcept = default;
        ctr_context_t& operator=(const ctr_context_t& other) = default;
        ctr_context_t& operator=(ctr_context_t&& other) noexcept = default;
        virtual ~ctr_context_t() = default;

    public:
        // Process bytes.
        //   dst: destination buffer.
        //   src: source buffer.
        //   position: current position in stream in bytes.
        //   length: length in bytes to process must be multiple sizeof block (=16).
        virtual void process_bytes(void* dst, const void* src, size_t position, size_t length) = 0;
    };

    std::unique_ptr<ecb_context_t> create_ecb_encrypt_context(const key_128bit_t* key);
    std::unique_ptr<ecb_context_t> create_ecb_encrypt_context(const key_192bit_t* key);
    std::unique_ptr<ecb_context_t> create_ecb_encrypt_context(const key_256bit_t* key);

    std::unique_ptr<ecb_context_t> create_ecb_decrypt_context(const key_128bit_t* key);
    std::unique_ptr<ecb_context_t> create_ecb_decrypt_context(const key_192bit_t* key);
    std::unique_ptr<ecb_context_t> create_ecb_decrypt_context(const key_256bit_t* key);

    std::unique_ptr<ctr_context_t> create_ctr_context(const key_128bit_t* key, const ctr_iv_t* iv, const ctr_nonce_t* nonce);
    std::unique_ptr<ctr_context_t> create_ctr_context(const key_192bit_t* key, const ctr_iv_t* iv, const ctr_nonce_t* nonce);
    std::unique_ptr<ctr_context_t> create_ctr_context(const key_256bit_t* key, const ctr_iv_t* iv, const ctr_nonce_t* nonce);
}
