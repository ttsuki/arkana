/// @file
/// @brief	arkana::sha2
///			- An implementation of SHA-1, SHA-2
/// @author Copyright(c) 2021 ttsuki
/// 
/// This software is released under the MIT License.
/// https://opensource.org/licenses/MIT

#pragma once

#include <cstddef>
#include <climits>
#include <array>
#include <memory>

namespace arkana::sha2
{
    struct md5_digest_algorithm;
    struct sha1_digest_algorithm;
    struct sha224_digest_algorithm;
    struct sha256_digest_algorithm;
    struct sha384_digest_algorithm;
    struct sha512_digest_algorithm;
    struct sha512_224_digest_algorithm;
    struct sha512_256_digest_algorithm;

    template <size_t bytes>
    using digest_value_t = std::array<std::byte, bytes>;

    using md5_digest_t = digest_value_t<128 / CHAR_BIT>;
    using sha1_digest_t = digest_value_t<160 / CHAR_BIT>;
    using sha224_digest_t = digest_value_t<224 / CHAR_BIT>;
    using sha256_digest_t = digest_value_t<256 / CHAR_BIT>;
    using sha384_digest_t = digest_value_t<384 / CHAR_BIT>;
    using sha512_digest_t = digest_value_t<512 / CHAR_BIT>;
    using sha512_224_digest_t = digest_value_t<224 / CHAR_BIT>;
    using sha512_256_digest_t = digest_value_t<256 / CHAR_BIT>;

    template <class algorithm_tag, class digest_type>
    class digest_context_t
    {
    public:
        digest_context_t() = default;
        digest_context_t(const digest_context_t& other) = delete;
        digest_context_t(digest_context_t&& other) noexcept = default;
        digest_context_t& operator=(const digest_context_t& other) = delete;
        digest_context_t& operator=(digest_context_t&& other) noexcept = default;
        virtual ~digest_context_t() = default;

    public:
        using algorithm_t = algorithm_tag;
        using digest_t = digest_type;

        // Calculates digest
        //   data: input data
        //   len: length in bytes
        virtual void process_bytes(const void* data, size_t len) noexcept = 0;

        // Finalizes stream and calculates digest value.
        virtual digest_t finalize() noexcept = 0;
    };

    using md5_context_t = digest_context_t<md5_digest_algorithm, md5_digest_t>;
    using sha1_context_t = digest_context_t<sha1_digest_algorithm, sha1_digest_t>;
    using sha224_context_t = digest_context_t<sha224_digest_algorithm, sha224_digest_t>;
    using sha256_context_t = digest_context_t<sha256_digest_algorithm, sha256_digest_t>;
    using sha384_context_t = digest_context_t<sha384_digest_algorithm, sha384_digest_t>;
    using sha512_context_t = digest_context_t<sha512_digest_algorithm, sha512_digest_t>;
    using sha512_224_context_t = digest_context_t<sha512_224_digest_algorithm, sha512_224_digest_t>;
    using sha512_256_context_t = digest_context_t<sha512_256_digest_algorithm, sha512_256_digest_t>;

    std::unique_ptr<md5_context_t> create_md5_context();
    std::unique_ptr<sha1_context_t> create_sha1_context();
    std::unique_ptr<sha224_context_t> create_sha224_context();
    std::unique_ptr<sha256_context_t> create_sha256_context();
    std::unique_ptr<sha384_context_t> create_sha384_context();
    std::unique_ptr<sha512_context_t> create_sha512_context();
    std::unique_ptr<sha512_224_context_t> create_sha512_224_context();
    std::unique_ptr<sha512_256_context_t> create_sha512_256_context();
}
