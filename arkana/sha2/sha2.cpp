/// @file
/// @brief	arkana::sha2
///			- An implementation of SHA-2
/// @author Copyright(c) 2021 ttsuki
/// 
/// This software is released under the MIT License.
/// https://opensource.org/licenses/MIT

#include "../sha2.h"
#include "./sha2.h"

namespace arkana::sha2
{
    namespace // internal linkage
    {
        template <
            auto create_sha2_state,
            class sha2_state_t = decltype(create_sha2_state()),
            size_t digest_bits = sha2_state_t::digest_bits>
        class sha2_context_impl_t
            : public virtual sha2_context_t<sha2_digest_t<digest_bits>>
        {
        public:
            sha2_state_t state = create_sha2_state();

            void process_bytes(const void* data, size_t len) noexcept override { arkana::sha2::process_bytes(state, data, len); }
            sha2_digest_t<digest_bits> finalize() noexcept override { return arkana::sha2::finalize_and_get_digest(state); }
        };
    }

    std::unique_ptr<sha224_context_t> create_sha224_context() { return std::make_unique<sha2_context_impl_t<create_sha224_state>>(); }
    std::unique_ptr<sha256_context_t> create_sha256_context() { return std::make_unique<sha2_context_impl_t<create_sha256_state>>(); }
    std::unique_ptr<sha384_context_t> create_sha384_context() { return std::make_unique<sha2_context_impl_t<create_sha384_state>>(); }
    std::unique_ptr<sha512_context_t> create_sha512_context() { return std::make_unique<sha2_context_impl_t<create_sha512_state>>(); }
}
