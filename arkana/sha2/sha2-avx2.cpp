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

#include "../sha2.h"
#include "./sha2-avx2.h"

namespace arkana::sha2
{
    template <class sha2_state_t>
    static inline auto make_sha2_context_avx2(sha2_state_t state)
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
                avx2::process_bytes(state, data, len);
            }

            typename sha2_state_t::digest_t finalize() noexcept override
            {
                return avx2::finalize_and_get_digest(state);
            }
        };

        return std::make_unique<sha2_context_impl_t>(std::move(state));
    }

    std::unique_ptr<md5_context_t> create_md5_context_avx2() { return make_sha2_context_avx2(create_md5_state()); }
    std::unique_ptr<sha1_context_t> create_sha1_context_avx2() { return make_sha2_context_avx2(create_sha1_state()); }
    std::unique_ptr<sha224_context_t> create_sha224_context_avx2() { return make_sha2_context_avx2(create_sha224_state()); }
    std::unique_ptr<sha256_context_t> create_sha256_context_avx2() { return make_sha2_context_avx2(create_sha256_state()); }
    std::unique_ptr<sha384_context_t> create_sha384_context_avx2() { return make_sha2_context_avx2(create_sha384_state()); }
    std::unique_ptr<sha512_context_t> create_sha512_context_avx2() { return make_sha2_context_avx2(create_sha512_state()); }
    std::unique_ptr<sha512_224_context_t> create_sha512_224_context_avx2() { return make_sha2_context_avx2(create_sha512_224_state()); }
    std::unique_ptr<sha512_256_context_t> create_sha512_256_context_avx2() { return make_sha2_context_avx2(create_sha512_256_state()); }
}
