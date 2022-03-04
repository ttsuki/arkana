/// @file
/// @brief	arkana::crc32
///			- An implementation of CRC32
/// @author Copyright(c) 2020 ttsuki
/// 
/// This software is released under the MIT License.
/// https://opensource.org/licenses/MIT

#include "../crc32.h"

#include <arkana/bits/cpuid.h>

namespace arkana::crc32
{
    std::unique_ptr<crc32_context_t> create_crc32_context(crc32_value_t initial)
    {
        if (cpuid::cpu_supports::AVX2 && cpuid::cpu_supports::PCLMULQDQ)
            return create_crc32_context_avx2clmul(initial);

        if (arkana::cpuid::cpu_supports::AVX2)
            return create_crc32_context_avx2(initial);

        return create_crc32_context_ia32(initial);
    }
}

#include <arkana/crc32/crc32.h>

namespace arkana::crc32
{
    std::unique_ptr<crc32_context_t> create_crc32_context_ref(crc32_value_t initial)
    {
        struct crc32_context_impl_t final : public virtual crc32_context_t
        {
            crc32_value_t value{};

            crc32_context_impl_t(crc32_value_t initial) : value(initial)
            {
            }

            crc32_value_t current() const override { return value; }
            void update(const void* data, size_t length) override { value = ref::calculate_crc32<0xEDB88320>(data, length, value); }
        };
        return std::make_unique<crc32_context_impl_t>(initial);
    }
}