/// @file
/// @brief	arkana::crc32
///			- An implementation of CRC32
/// @author Copyright(c) 2020 ttsuki
/// 
/// This software is released under the MIT License.
/// https://opensource.org/licenses/MIT
///
/// This implementation based on
/// "Fast CRC Computation for Generic Polynomials Using PCLMULQDQ Instruction"
/// -- V. Gopal, E. Ozturk, J. Guilford, et al., 2009,
/// https://www.intel.com/content/dam/www/public/us/en/documents/white-papers/fast-crc-computation-generic-polynomials-pclmulqdq-paper.pdf

#include "./crc32.h"
#include "./crc32-avx2clmul.h"
#include "../ark/cpuid.h"

namespace arkana::crc32
{
    bool cpu_supports_avx2clmul() noexcept
    {
        return cpuid::cpu_supports::AVX2 && cpuid::cpu_supports::PCLMULQDQ;
    }

    inline crc32_value_t calculate_crc32_avx2clmul(const void* data, size_t length, crc32_value_t current)
    {
        return avx2clmul::calculate_crc32<0xEDB88320>(data, length, current);
    }

    std::unique_ptr<crc32_context_t> create_crc32_context_avx2clmul(crc32_value_t initial)
    {
        struct crc32_context_impl_t final : public virtual crc32_context_t
        {
            crc32_value_t value{};
            crc32_context_impl_t(crc32_value_t initial) : value(initial) { }
            crc32_value_t current() const override { return value; }
            void update(const void* data, size_t length) override { value = calculate_crc32_avx2clmul(data, length, value); }
        };

        return std::make_unique<crc32_context_impl_t>(initial);
    }
}
