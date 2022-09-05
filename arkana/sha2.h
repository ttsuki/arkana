/// @file
/// @brief	arkana::sha2
///			- An implementation of SHA-1, SHA-2
/// @author Copyright(c) 2021 ttsuki
/// 
/// This software is released under the MIT License.
/// https://opensource.org/licenses/MIT

#pragma once

#include <cstddef>
#include <array>
#include <memory>

namespace arkana::sha2
{
    template <size_t bits> using sha2_digest_t = std::array<std::byte, bits / 8>;
    using md5_digest_t = std::array<std::byte, 128 / 8>;
    using sha1_digest_t = std::array<std::byte, 160 / 8>;
    using sha224_digest_t = std::array<std::byte, 224 / 8>;
    using sha256_digest_t = std::array<std::byte, 256 / 8>;
    using sha384_digest_t = std::array<std::byte, 384 / 8>;
    using sha512_digest_t = std::array<std::byte, 512 / 8>;
    using sha512_224_digest_t = std::array<std::byte, 224 / 8>;
    using sha512_256_digest_t = std::array<std::byte, 256 / 8>;

    template <class sha2_digest_t>
    class sha2_context_t
    {
    public:
        sha2_context_t() = default;
        sha2_context_t(const sha2_context_t& other) = delete;
        sha2_context_t(sha2_context_t&& other) noexcept = default;
        sha2_context_t& operator=(const sha2_context_t& other) = delete;
        sha2_context_t& operator=(sha2_context_t&& other) noexcept = default;
        virtual ~sha2_context_t() = default;

    public:
        using digest_t = sha2_digest_t;
        virtual void process_bytes(const void* data, size_t len) noexcept = 0;
        virtual digest_t finalize() noexcept = 0;
    };

    using md5_context_t = sha2_context_t<md5_digest_t>;
    using sha1_context_t = sha2_context_t<sha1_digest_t>;
    using sha224_context_t = sha2_context_t<sha224_digest_t>;
    using sha256_context_t = sha2_context_t<sha256_digest_t>;
    using sha384_context_t = sha2_context_t<sha384_digest_t>;
    using sha512_context_t = sha2_context_t<sha512_digest_t>;
    using sha512_224_context_t = sha2_context_t<sha512_224_digest_t>;
    using sha512_256_context_t = sha2_context_t<sha512_256_digest_t>;

    std::unique_ptr<md5_context_t> create_md5_context();
    std::unique_ptr<sha1_context_t> create_sha1_context();
    std::unique_ptr<sha224_context_t> create_sha224_context();
    std::unique_ptr<sha256_context_t> create_sha256_context();
    std::unique_ptr<sha384_context_t> create_sha384_context();
    std::unique_ptr<sha512_context_t> create_sha512_context();
    std::unique_ptr<sha512_224_context_t> create_sha512_224_context();
    std::unique_ptr<sha512_256_context_t> create_sha512_256_context();

    std::unique_ptr<md5_context_t> create_md5_context_ref();
    std::unique_ptr<sha1_context_t> create_sha1_context_ref();
    std::unique_ptr<sha224_context_t> create_sha224_context_ref();
    std::unique_ptr<sha256_context_t> create_sha256_context_ref();
    std::unique_ptr<sha384_context_t> create_sha384_context_ref();
    std::unique_ptr<sha512_context_t> create_sha512_context_ref();
    std::unique_ptr<sha512_224_context_t> create_sha512_224_context_ref();
    std::unique_ptr<sha512_256_context_t> create_sha512_256_context_ref();

    std::unique_ptr<md5_context_t> create_md5_context_avx2();
    std::unique_ptr<sha1_context_t> create_sha1_context_avx2();
    std::unique_ptr<sha224_context_t> create_sha224_context_avx2();
    std::unique_ptr<sha256_context_t> create_sha256_context_avx2();
    std::unique_ptr<sha384_context_t> create_sha384_context_avx2();
    std::unique_ptr<sha512_context_t> create_sha512_context_avx2();
    std::unique_ptr<sha512_224_context_t> create_sha512_224_context_avx2();
    std::unique_ptr<sha512_256_context_t> create_sha512_256_context_avx2();
}
