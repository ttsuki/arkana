#pragma once

#include "../crc32.h"

namespace arkana::crc32
{
    namespace // internal linkage
    {
        template <auto t_update>
        struct crc32_context_impl_t : public virtual crc32_context_t
        {
            crc32_value_t value{};

            explicit crc32_context_impl_t(crc32_value_t initial)
                : value(initial)
            {
            }

            crc32_value_t current() const override
            {
                return value;
            }

            void update(const void* data, size_t length) override
            {
                value = t_update(data, length, value);
            }
        };
    }
}
