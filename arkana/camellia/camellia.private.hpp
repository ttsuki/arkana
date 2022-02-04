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
        template <class t_key_t, class t_unit_t, auto t_generate_key_vector, auto t_process_blocks>
        struct ecb_context_impl_t : public virtual ecb_context_t
        {
            using key_t = t_key_t;
            using key_vector_t = decltype(t_generate_key_vector(std::declval<key_t>()));
            using unit_t = t_unit_t;

            const key_vector_t key_vector;

            explicit ecb_context_impl_t(const key_t* key)
                : key_vector(t_generate_key_vector(*key))
            {
            }

            void process_blocks(void* dst, const void* src, size_t length) override
            {
                if (length % 16 != 0)
                    throw std::invalid_argument("invalid length.");

                const size_t unit_count = length / sizeof(unit_t);
                t_process_blocks(static_cast<unit_t*>(dst), static_cast<const unit_t*>(src), unit_count, key_vector);

                if (const size_t remain_bytes = length % sizeof(unit_t))
                {
                    unit_t buf{};
                    static_assert(std::is_trivial_v<unit_t>);
                    memcpy(&buf, static_cast<const std::byte*>(src) + unit_count * sizeof(unit_t), remain_bytes);
                    t_process_blocks(&buf, &buf, 1, key_vector);
                    memcpy(static_cast<std::byte*>(dst) + unit_count * sizeof(unit_t), &buf, remain_bytes);
                }
            }
        };
    }
}
