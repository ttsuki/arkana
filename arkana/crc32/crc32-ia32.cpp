/// @file
/// @brief	arkana::crc32
///			- An implementation of CRC32
/// @author Copyright(c) 2020 ttsuki
/// 
/// This software is released under the MIT License.
/// https://opensource.org/licenses/MIT

#include "./crc32.h"
#include "./crc32-ia32.h"

namespace arkana::crc32
{
    bool cpu_supports_ia32() noexcept
    {
        return true;
    }

    crc32_value_t calculate_crc32_ia32(const void* data, size_t length, crc32_value_t current)
    {
        return ia32::calculate_crc32<0xEDB88320>(data, length, current);
    }

    std::unique_ptr<crc32_context_t> create_crc32_context_ia32(crc32_value_t initial)
    {
        struct crc32_context_impl_t final : public virtual crc32_context_t
        {
            crc32_value_t value{};
            crc32_context_impl_t(crc32_value_t initial) : value(initial) { }
            crc32_value_t current() const override { return value; }
            void update(const void* data, size_t length) override { value = calculate_crc32_ia32(data, length, value); }
        };
        return std::make_unique<crc32_context_impl_t>(initial);
    }
}
