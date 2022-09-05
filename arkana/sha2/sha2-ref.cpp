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
    template <class sha2_state_t>
    static inline auto make_sha2_context_ref(sha2_state_t state)
    {
        class sha2_context_impl_t
            : public virtual sha2_context_t<typename sha2_state_t::digest_t>
        {
        public:
            sha2_state_t state;

            explicit sha2_context_impl_t(sha2_state_t state)
                : state(std::move(state)) { }

            void process_bytes(const void* data, size_t len) noexcept override
            {
                ref::process_bytes(state, data, len);
            }

            typename sha2_state_t::digest_t finalize() noexcept override
            {
                return ref::finalize_and_get_digest(state);
            }
        };

        return std::make_unique<sha2_context_impl_t>(std::move(state));
    }

    std::unique_ptr<md5_context_t> create_md5_context_ref() { return make_sha2_context_ref(create_md5_state()); }
    std::unique_ptr<sha1_context_t> create_sha1_context_ref() { return make_sha2_context_ref(create_sha1_state()); }
    std::unique_ptr<sha224_context_t> create_sha224_context_ref() { return make_sha2_context_ref(create_sha224_state()); }
    std::unique_ptr<sha256_context_t> create_sha256_context_ref() { return make_sha2_context_ref(create_sha256_state()); }
    std::unique_ptr<sha384_context_t> create_sha384_context_ref() { return make_sha2_context_ref(create_sha384_state()); }
    std::unique_ptr<sha512_context_t> create_sha512_context_ref() { return make_sha2_context_ref(create_sha512_state()); }
    std::unique_ptr<sha512_224_context_t> create_sha512_224_context_ref() { return make_sha2_context_ref(create_sha512_224_state()); }
    std::unique_ptr<sha512_256_context_t> create_sha512_256_context_ref() { return make_sha2_context_ref(create_sha512_256_state()); }
}
