/// @file
/// @brief	arkana::sha2
///			- An implementation of SHA-1, SHA-2
/// @author Copyright(c) 2021 ttsuki
/// 
/// This software is released under the MIT License.
/// https://opensource.org/licenses/MIT

#pragma once

#include "../sha2.h"
#include "../ark/types.h"

namespace arkana::sha2
{
    using md5_state_t = tagged_memory_buffer<md5_digest_algorithm, sizeof(uint32_t) * 24 + sizeof(intmax_t)>;
    using sha1_state_t = tagged_memory_buffer<sha1_digest_algorithm, sizeof(uint32_t) * 24 + sizeof(intmax_t)>;
    using sha224_state_t = tagged_memory_buffer<sha224_digest_algorithm, sizeof(uint32_t) * 24 + sizeof(intmax_t)>;
    using sha256_state_t = tagged_memory_buffer<sha256_digest_algorithm, sizeof(uint32_t) * 24 + sizeof(intmax_t)>;
    using sha384_state_t = tagged_memory_buffer<sha384_digest_algorithm, sizeof(uint64_t) * 24 + sizeof(intmax_t)>;
    using sha512_state_t = tagged_memory_buffer<sha512_digest_algorithm, sizeof(uint64_t) * 24 + sizeof(intmax_t)>;
    using sha512_224_state_t = tagged_memory_buffer<sha512_224_digest_algorithm, sizeof(uint64_t) * 24 + sizeof(intmax_t)>;
    using sha512_256_state_t = tagged_memory_buffer<sha512_256_digest_algorithm, sizeof(uint64_t) * 24 + sizeof(intmax_t)>;

    void initialize_state(md5_state_t* s);
    void initialize_state(sha1_state_t* s);
    void initialize_state(sha224_state_t* s);
    void initialize_state(sha256_state_t* s);
    void initialize_state(sha384_state_t* s);
    void initialize_state(sha512_state_t* s);
    void initialize_state(sha512_224_state_t* s);
    void initialize_state(sha512_256_state_t* s);

    bool cpu_supports_ref() noexcept;
    bool cpu_supports_avx2() noexcept;

    void process_bytes_ref(md5_state_t* s, const void* data, size_t length);
    void process_bytes_ref(sha1_state_t* s, const void* data, size_t length);
    void process_bytes_ref(sha224_state_t* s, const void* data, size_t length);
    void process_bytes_ref(sha256_state_t* s, const void* data, size_t length);
    void process_bytes_ref(sha384_state_t* s, const void* data, size_t length);
    void process_bytes_ref(sha512_state_t* s, const void* data, size_t length);
    void process_bytes_ref(sha512_224_state_t* s, const void* data, size_t length);
    void process_bytes_ref(sha512_256_state_t* s, const void* data, size_t length);
    void finalize_and_get_digest_ref(md5_state_t* s, md5_digest_t* out);
    void finalize_and_get_digest_ref(sha1_state_t* s, sha1_digest_t* out);
    void finalize_and_get_digest_ref(sha224_state_t* s, sha224_digest_t* out);
    void finalize_and_get_digest_ref(sha256_state_t* s, sha256_digest_t* out);
    void finalize_and_get_digest_ref(sha384_state_t* s, sha384_digest_t* out);
    void finalize_and_get_digest_ref(sha512_state_t* s, sha512_digest_t* out);
    void finalize_and_get_digest_ref(sha512_224_state_t* s, sha512_224_digest_t* out);
    void finalize_and_get_digest_ref(sha512_256_state_t* s, sha512_256_digest_t* out);

    void process_bytes_avx2(md5_state_t* s, const void* data, size_t length);
    void process_bytes_avx2(sha1_state_t* s, const void* data, size_t length);
    void process_bytes_avx2(sha224_state_t* s, const void* data, size_t length);
    void process_bytes_avx2(sha256_state_t* s, const void* data, size_t length);
    void process_bytes_avx2(sha384_state_t* s, const void* data, size_t length);
    void process_bytes_avx2(sha512_state_t* s, const void* data, size_t length);
    void process_bytes_avx2(sha512_224_state_t* s, const void* data, size_t length);
    void process_bytes_avx2(sha512_256_state_t* s, const void* data, size_t length);
    void finalize_and_get_digest_avx2(md5_state_t* s, md5_digest_t* out);
    void finalize_and_get_digest_avx2(sha1_state_t* s, sha1_digest_t* out);
    void finalize_and_get_digest_avx2(sha224_state_t* s, sha224_digest_t* out);
    void finalize_and_get_digest_avx2(sha256_state_t* s, sha256_digest_t* out);
    void finalize_and_get_digest_avx2(sha384_state_t* s, sha384_digest_t* out);
    void finalize_and_get_digest_avx2(sha512_state_t* s, sha512_digest_t* out);
    void finalize_and_get_digest_avx2(sha512_224_state_t* s, sha512_224_digest_t* out);
    void finalize_and_get_digest_avx2(sha512_256_state_t* s, sha512_256_digest_t* out);

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
