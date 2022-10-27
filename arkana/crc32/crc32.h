/// @file
/// @brief	arkana::crc32
///			- An implementation of CRC32
/// @author Copyright(c) 2020 ttsuki
/// 
/// This software is released under the MIT License.
/// https://opensource.org/licenses/MIT

#pragma once

#include "../crc32.h"

namespace arkana::crc32
{
    bool cpu_supports_ref() noexcept;
    bool cpu_supports_ia32() noexcept;
    bool cpu_supports_avx2() noexcept;
    bool cpu_supports_avx2clmul() noexcept;

    crc32_value_t calculate_crc32_ref(const void* data, size_t length, crc32_value_t current = 0);
    crc32_value_t calculate_crc32_ia32(const void* data, size_t length, crc32_value_t current = 0);
    crc32_value_t calculate_crc32_avx2(const void* data, size_t length, crc32_value_t current = 0);
    crc32_value_t calculate_crc32_avx2clmul(const void* data, size_t length, crc32_value_t current = 0);

    std::unique_ptr<crc32_context_t> create_crc32_context_ref(crc32_value_t initial = 0);
    std::unique_ptr<crc32_context_t> create_crc32_context_ia32(crc32_value_t initial = 0);
    std::unique_ptr<crc32_context_t> create_crc32_context_avx2(crc32_value_t initial = 0);
    std::unique_ptr<crc32_context_t> create_crc32_context_avx2clmul(crc32_value_t initial = 0);
}
