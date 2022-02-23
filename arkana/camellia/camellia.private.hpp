/// @file
/// @brief	arkana::camellia
///			- An implementation of Camellia cipher
/// @author Copyright(c) 2021 ttsuki
/// 
/// This software is released under the MIT License.
/// https://opensource.org/licenses/MIT
///
/// - camellia https://info.isl.ntt.co.jp/crypt/camellia/ 

#pragma once

#include <stdexcept>

#include "../camellia.h"

namespace arkana::camellia
{
    namespace // internal linkage
    {
        template <class t_key_t, auto t_generate_key_vector, auto t_process_blocks>
        struct ecb_context_impl_t : public virtual ecb_context_t
        {
            using key_t = t_key_t;
            using key_vector_t = decltype(t_generate_key_vector(std::declval<key_t>()));
            const key_vector_t key_vector;

            explicit ecb_context_impl_t(const key_t* key)
                : key_vector(t_generate_key_vector(*key))
            {
            }

            void process_blocks(void* dst, const void* src, size_t length) override
            {
                t_process_blocks(dst, src, length, key_vector);
            }
        };
    }
}
