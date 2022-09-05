/// @file
/// @brief	arkana::crc32
///			- An implementation of CRC32
/// @author Copyright(c) 2020 ttsuki
/// 
/// This software is released under the MIT License.
/// https://opensource.org/licenses/MIT

#include "../crc32.h"
#include <arkana/ark/cpuid.h>

namespace arkana::crc32
{
    crc32_value_t calculate_crc32(const void* data, size_t length, crc32_value_t current)
    {
        if (cpuid::cpu_supports::AVX2 && cpuid::cpu_supports::PCLMULQDQ)
            return calculate_crc32_avx2clmul(data, length, current);

        if (cpuid::cpu_supports::AVX2)
            return calculate_crc32_avx2(data, length, current);

        return calculate_crc32_ia32(data, length, current);
    }

    std::unique_ptr<crc32_context_t> create_crc32_context(crc32_value_t initial)
    {
        if (cpuid::cpu_supports::AVX2 && cpuid::cpu_supports::PCLMULQDQ)
            return create_crc32_context_avx2clmul(initial);

        if (cpuid::cpu_supports::AVX2)
            return create_crc32_context_avx2(initial);

        return create_crc32_context_ia32(initial);
    }
}

#include <arkana/crc32/crc32.h>

namespace arkana::crc32
{
    crc32_value_t calculate_crc32_ref(const void* data, size_t length, crc32_value_t current)
    {
        return ref::calculate_crc32<0xEDB88320>(data, length, current);
    }

    std::unique_ptr<crc32_context_t> create_crc32_context_ref(crc32_value_t initial)
    {
        struct crc32_context_impl_t final : public virtual crc32_context_t
        {
            crc32_value_t value{};
            crc32_context_impl_t(crc32_value_t initial) : value(initial) { }
            crc32_value_t current() const override { return value; }
            void update(const void* data, size_t length) override { value = calculate_crc32_ref(data, length, value); }
        };
        return std::make_unique<crc32_context_impl_t>(initial);
    }
}
