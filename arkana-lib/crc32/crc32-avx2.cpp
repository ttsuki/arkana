/// @file
/// @brief	arkana::crc32
///			- An implementation of CRC32
/// @author Copyright(c) 2020 ttsuki
/// 
/// This software is released under the MIT License.
/// https://opensource.org/licenses/MIT

#include "../crc32.h"
#include <arkana/crc32/crc32-avx2.h>

namespace arkana::crc32
{
    std::unique_ptr<crc32_context_t> create_crc32_context_avx2(crc32_value_t initial)
    {
        struct crc32_context_impl_t final : public virtual crc32_context_t
        {
            crc32_value_t value{};
            crc32_context_impl_t(crc32_value_t initial) : value(initial) { }
            crc32_value_t current() const override { return value; }
            void update(const void* data, size_t length) override { value = avx2::calculate_crc32<0xEDB88320>(data, length, value); }
        };
        return std::make_unique<crc32_context_impl_t>(initial);
    }
}

