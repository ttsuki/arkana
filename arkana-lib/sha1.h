/// @file
/// @brief	arkana::sha2
///			- An implementation of SHA-2
/// @author Copyright(c) 2021 ttsuki
/// 
/// This software is released under the MIT License.
/// https://opensource.org/licenses/MIT

#pragma once

#include <cstddef>
#include <array>
#include <memory>

namespace arkana::sha1
{
    using sha1_digest_t = std::array<std::byte, 160 / 8>;

    class sha1_context_t
    {
    public:
        sha1_context_t() = default;
        sha1_context_t(const sha1_context_t& other) = delete;
        sha1_context_t(sha1_context_t&& other) noexcept = default;
        sha1_context_t& operator=(const sha1_context_t& other) = delete;
        sha1_context_t& operator=(sha1_context_t&& other) noexcept = default;
        virtual ~sha1_context_t() = default;

    public:
        using digest_t = sha1_digest_t;
        virtual void process_bytes(const void* data, size_t len) noexcept = 0;
        virtual digest_t finalize() noexcept = 0;
    };

    std::unique_ptr<sha1_context_t> create_sha1_context();
    std::unique_ptr<sha1_context_t> create_sha1_context_ref();
    std::unique_ptr<sha1_context_t> create_sha1_context_avx2();
}
