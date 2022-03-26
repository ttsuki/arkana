/// @file
/// @brief	arkana::sha2
///			- An implementation of SHA-2
/// @author Copyright(c) 2021 ttsuki
/// 
/// This software is released under the MIT License.
/// https://opensource.org/licenses/MIT

#include "../sha1.h"
#include <arkana/sha1/sha1-avx2.h>

namespace arkana::sha1
{
    std::unique_ptr<sha1_context_t> create_sha1_context_avx2()
    {
        class sha1_context_impl_t
            : public virtual sha1_context_t
        {
        public:
            sha1_state_t state;

            sha1_context_impl_t()
                : state(create_sha1_state())
            {
            }

            void process_bytes(const void* data, size_t len) noexcept override
            {
                avx2::process_bytes(state, data, len);
            }

            sha1_digest_t finalize() noexcept override
            {
                return avx2::finalize_and_get_digest(state);
            }
        };

        return std::make_unique<sha1_context_impl_t>();
    }
}
