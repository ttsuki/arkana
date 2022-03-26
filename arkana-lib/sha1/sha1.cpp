/// @file
/// @brief	arkana::sha1
///			- An implementation of SHA-1
/// @author Copyright(c) 2022 ttsuki
/// 
/// This software is released under the MIT License.
/// https://opensource.org/licenses/MIT

#include "../sha1.h"

#include <arkana/bits/cpuid.h>

namespace arkana::sha1
{
    std::unique_ptr<sha1_context_t> create_sha1_context()
    {
        return cpuid::cpu_supports::AVX2
                   ? create_sha1_context_avx2()
                   : create_sha1_context_ref();
    }
}

#include <arkana/sha1/sha1.h>

namespace arkana::sha1
{
    std::unique_ptr<sha1_context_t> create_sha1_context_ref()
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
                ref::process_bytes(state, data, len);
            }

            sha1_digest_t finalize() noexcept override
            {
                return ref::finalize_and_get_digest(state);
            }
        };

        return std::make_unique<sha1_context_impl_t>();
    }
}
