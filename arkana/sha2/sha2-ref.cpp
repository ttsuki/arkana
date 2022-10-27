/// @file
/// @brief	arkana::sha2
///			- An implementation of SHA-1, SHA-2
/// @author Copyright(c) 2021 ttsuki
/// 
/// This software is released under the MIT License.
/// https://opensource.org/licenses/MIT

#include "./sha2.h"
#include "./sha2-ref.h"

namespace arkana::sha2
{
    inline bool cpu_supports_ref() noexcept
    {
        return true;
    }

    inline void initialize_state(md5_state_t* s) { *s = bit_cast<md5_state_t>(ref::create_md5_state()); }
    inline void initialize_state(sha1_state_t* s) { *s = bit_cast<sha1_state_t>(ref::create_sha1_state()); }
    inline void initialize_state(sha224_state_t* s) { *s = bit_cast<sha224_state_t>(ref::create_sha224_state()); }
    inline void initialize_state(sha256_state_t* s) { *s = bit_cast<sha256_state_t>(ref::create_sha256_state()); }
    inline void initialize_state(sha384_state_t* s) { *s = bit_cast<sha384_state_t>(ref::create_sha384_state()); }
    inline void initialize_state(sha512_state_t* s) { *s = bit_cast<sha512_state_t>(ref::create_sha512_state()); }
    inline void initialize_state(sha512_224_state_t* s) { *s = bit_cast<sha512_224_state_t>(ref::create_sha512_224_state()); }
    inline void initialize_state(sha512_256_state_t* s) { *s = bit_cast<sha512_256_state_t>(ref::create_sha512_256_state()); }

    inline void process_bytes_ref(md5_state_t* s, const void* data, size_t length) { ref::process_bytes(type_punning_cast<ref::md5_state_t&>(*s), data, length); }
    inline void process_bytes_ref(sha1_state_t* s, const void* data, size_t length) { ref::process_bytes(type_punning_cast<ref::sha1_state_t&>(*s), data, length); }
    inline void process_bytes_ref(sha224_state_t* s, const void* data, size_t length) { ref::process_bytes(type_punning_cast<ref::sha224_state_t&>(*s), data, length); }
    inline void process_bytes_ref(sha256_state_t* s, const void* data, size_t length) { ref::process_bytes(type_punning_cast<ref::sha256_state_t&>(*s), data, length); }
    inline void process_bytes_ref(sha384_state_t* s, const void* data, size_t length) { ref::process_bytes(type_punning_cast<ref::sha384_state_t&>(*s), data, length); }
    inline void process_bytes_ref(sha512_state_t* s, const void* data, size_t length) { ref::process_bytes(type_punning_cast<ref::sha512_state_t&>(*s), data, length); }
    inline void process_bytes_ref(sha512_224_state_t* s, const void* data, size_t length) { ref::process_bytes(type_punning_cast<ref::sha512_224_state_t&>(*s), data, length); }
    inline void process_bytes_ref(sha512_256_state_t* s, const void* data, size_t length) { ref::process_bytes(type_punning_cast<ref::sha512_256_state_t&>(*s), data, length); }

    inline void finalize_and_get_digest_ref(md5_state_t* s, md5_digest_t* out) { ref::finalize_and_get_digest(type_punning_cast<ref::md5_state_t&>(*s), out); }
    inline void finalize_and_get_digest_ref(sha1_state_t* s, sha1_digest_t* out) { ref::finalize_and_get_digest(type_punning_cast<ref::sha1_state_t&>(*s), out); }
    inline void finalize_and_get_digest_ref(sha224_state_t* s, sha224_digest_t* out) { ref::finalize_and_get_digest(type_punning_cast<ref::sha224_state_t&>(*s), out); }
    inline void finalize_and_get_digest_ref(sha256_state_t* s, sha256_digest_t* out) { ref::finalize_and_get_digest(type_punning_cast<ref::sha256_state_t&>(*s), out); }
    inline void finalize_and_get_digest_ref(sha384_state_t* s, sha384_digest_t* out) { ref::finalize_and_get_digest(type_punning_cast<ref::sha384_state_t&>(*s), out); }
    inline void finalize_and_get_digest_ref(sha512_state_t* s, sha512_digest_t* out) { ref::finalize_and_get_digest(type_punning_cast<ref::sha512_state_t&>(*s), out); }
    inline void finalize_and_get_digest_ref(sha512_224_state_t* s, sha512_224_digest_t* out) { ref::finalize_and_get_digest(type_punning_cast<ref::sha512_224_state_t&>(*s), out); }
    inline void finalize_and_get_digest_ref(sha512_256_state_t* s, sha512_256_digest_t* out) { ref::finalize_and_get_digest(type_punning_cast<ref::sha512_256_state_t&>(*s), out); }

    template <class state_t>
    static inline auto make_context_ref(state_t state)
    {
        struct context_impl_t : public virtual digest_context_t<typename state_t::tag_t, typename state_t::digest_t>
        {
            state_t state;
            explicit context_impl_t(state_t state) : state(std::move(state)) { }
            ~context_impl_t() override { secure_be_zero(state); }
            void process_bytes(const void* data, size_t len) noexcept override { ref::process_bytes(state, data, len); }

            typename state_t::digest_t finalize() noexcept override
            {
                typename state_t::digest_t value;
                ref::finalize_and_get_digest(state, &value);
                return value;
            }
        };

        return std::make_unique<context_impl_t>(std::move(state));
    }

    std::unique_ptr<md5_context_t> create_md5_context_ref() { return make_context_ref(ref::create_md5_state()); }
    std::unique_ptr<sha1_context_t> create_sha1_context_ref() { return make_context_ref(ref::create_sha1_state()); }
    std::unique_ptr<sha224_context_t> create_sha224_context_ref() { return make_context_ref(ref::create_sha224_state()); }
    std::unique_ptr<sha256_context_t> create_sha256_context_ref() { return make_context_ref(ref::create_sha256_state()); }
    std::unique_ptr<sha384_context_t> create_sha384_context_ref() { return make_context_ref(ref::create_sha384_state()); }
    std::unique_ptr<sha512_context_t> create_sha512_context_ref() { return make_context_ref(ref::create_sha512_state()); }
    std::unique_ptr<sha512_224_context_t> create_sha512_224_context_ref() { return make_context_ref(ref::create_sha512_224_state()); }
    std::unique_ptr<sha512_256_context_t> create_sha512_256_context_ref() { return make_context_ref(ref::create_sha512_256_state()); }
}
