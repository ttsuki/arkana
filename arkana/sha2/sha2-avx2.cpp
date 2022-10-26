/// @file
/// @brief	arkana::sha2
///			- An implementation of SHA-1, SHA-2
/// @author Copyright(c) 2021 ttsuki
/// 
/// This software is released under the MIT License.
/// https://opensource.org/licenses/MIT
///
/// This SHA-2 implementation based on
///   "Fast SHA-256 Implementations on Intel® Architecture Processors"
///   -- J. Guilford, K. Yap, V. Gopal, 2012,
/// https://www.intel.com/content/dam/www/public/us/en/documents/white-papers/sha-256-implementations-paper.pdf

#include "./sha2.h"
#include "./sha2-avx2.h"
#include "../ark/cpuid.h"

namespace arkana::sha2
{
    inline bool cpu_supports_avx2() noexcept
    {
        return cpuid::cpu_supports::AVX2;
    }

    inline void process_bytes_avx2(md5_state_t* s, const void* data, size_t length) { avx2::process_bytes(type_punning_cast<avx2::md5_state_t&>(*s), data, length); }
    inline void process_bytes_avx2(sha1_state_t* s, const void* data, size_t length) { avx2::process_bytes(type_punning_cast<avx2::sha1_state_t&>(*s), data, length); }
    inline void process_bytes_avx2(sha224_state_t* s, const void* data, size_t length) { avx2::process_bytes(type_punning_cast<avx2::sha224_state_t&>(*s), data, length); }
    inline void process_bytes_avx2(sha256_state_t* s, const void* data, size_t length) { avx2::process_bytes(type_punning_cast<avx2::sha256_state_t&>(*s), data, length); }
    inline void process_bytes_avx2(sha384_state_t* s, const void* data, size_t length) { avx2::process_bytes(type_punning_cast<avx2::sha384_state_t&>(*s), data, length); }
    inline void process_bytes_avx2(sha512_state_t* s, const void* data, size_t length) { avx2::process_bytes(type_punning_cast<avx2::sha512_state_t&>(*s), data, length); }
    inline void process_bytes_avx2(sha512_224_state_t* s, const void* data, size_t length) { avx2::process_bytes(type_punning_cast<avx2::sha512_224_state_t&>(*s), data, length); }
    inline void process_bytes_avx2(sha512_256_state_t* s, const void* data, size_t length) { avx2::process_bytes(type_punning_cast<avx2::sha512_256_state_t&>(*s), data, length); }

    inline void finalize_and_get_digest_avx2(md5_state_t* s, md5_digest_t* out) { avx2::finalize_and_get_digest(type_punning_cast<avx2::md5_state_t&>(*s), out); }
    inline void finalize_and_get_digest_avx2(sha1_state_t* s, sha1_digest_t* out) { avx2::finalize_and_get_digest(type_punning_cast<avx2::sha1_state_t&>(*s), out); }
    inline void finalize_and_get_digest_avx2(sha224_state_t* s, sha224_digest_t* out) { avx2::finalize_and_get_digest(type_punning_cast<avx2::sha224_state_t&>(*s), out); }
    inline void finalize_and_get_digest_avx2(sha256_state_t* s, sha256_digest_t* out) { avx2::finalize_and_get_digest(type_punning_cast<avx2::sha256_state_t&>(*s), out); }
    inline void finalize_and_get_digest_avx2(sha384_state_t* s, sha384_digest_t* out) { avx2::finalize_and_get_digest(type_punning_cast<avx2::sha384_state_t&>(*s), out); }
    inline void finalize_and_get_digest_avx2(sha512_state_t* s, sha512_digest_t* out) { avx2::finalize_and_get_digest(type_punning_cast<avx2::sha512_state_t&>(*s), out); }
    inline void finalize_and_get_digest_avx2(sha512_224_state_t* s, sha512_224_digest_t* out) { avx2::finalize_and_get_digest(type_punning_cast<avx2::sha512_224_state_t&>(*s), out); }
    inline void finalize_and_get_digest_avx2(sha512_256_state_t* s, sha512_256_digest_t* out) { avx2::finalize_and_get_digest(type_punning_cast<avx2::sha512_256_state_t&>(*s), out); }

    template <class state_t>
    static inline auto make_context_avx2(state_t state)
    {
        struct context_impl_t : public virtual digest_context_t<typename state_t::tag_t, typename state_t::digest_t>
        {
            state_t state;
            explicit context_impl_t(state_t state) : state(std::move(state)) { }
            ~context_impl_t() override { secure_be_zero(state); }
            void process_bytes(const void* data, size_t len) noexcept override { avx2::process_bytes(state, data, len); }

            typename state_t::digest_t finalize() noexcept override
            {
                typename state_t::digest_t value;
                avx2::finalize_and_get_digest(state, &value);
                return value;
            }
        };

        return std::make_unique<context_impl_t>(std::move(state));
    }

    std::unique_ptr<md5_context_t> create_md5_context_avx2() { return make_context_avx2(avx2::create_md5_state()); }
    std::unique_ptr<sha1_context_t> create_sha1_context_avx2() { return make_context_avx2(avx2::create_sha1_state()); }
    std::unique_ptr<sha224_context_t> create_sha224_context_avx2() { return make_context_avx2(avx2::create_sha224_state()); }
    std::unique_ptr<sha256_context_t> create_sha256_context_avx2() { return make_context_avx2(avx2::create_sha256_state()); }
    std::unique_ptr<sha384_context_t> create_sha384_context_avx2() { return make_context_avx2(avx2::create_sha384_state()); }
    std::unique_ptr<sha512_context_t> create_sha512_context_avx2() { return make_context_avx2(avx2::create_sha512_state()); }
    std::unique_ptr<sha512_224_context_t> create_sha512_224_context_avx2() { return make_context_avx2(avx2::create_sha512_224_state()); }
    std::unique_ptr<sha512_256_context_t> create_sha512_256_context_avx2() { return make_context_avx2(avx2::create_sha512_256_state()); }
}
