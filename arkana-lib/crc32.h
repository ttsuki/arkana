/// @file
/// @brief	arkana::crc32
///			- An implementation of CRC32
/// @author Copyright(c) 2020 ttsuki
/// 
/// This software is released under the MIT License.
/// https://opensource.org/licenses/MIT

#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>

namespace arkana::crc32
{
    using crc32_value_t = std::uint32_t;
    crc32_value_t calculate_crc32(const void* data, size_t length, crc32_value_t current = 0);
    crc32_value_t calculate_crc32_ref(const void* data, size_t length, crc32_value_t current = 0);
    crc32_value_t calculate_crc32_ia32(const void* data, size_t length, crc32_value_t current = 0);
    crc32_value_t calculate_crc32_avx2(const void* data, size_t length, crc32_value_t current = 0);
    crc32_value_t calculate_crc32_avx2clmul(const void* data, size_t length, crc32_value_t current = 0);

    class crc32_context_t
    {
    public:
        crc32_context_t() = default;
        crc32_context_t(const crc32_context_t& other) = default;
        crc32_context_t(crc32_context_t&& other) noexcept = default;
        crc32_context_t& operator=(const crc32_context_t& other) = default;
        crc32_context_t& operator=(crc32_context_t&& other) noexcept = default;
        virtual ~crc32_context_t() = default;

    public:
        virtual crc32_value_t current() const = 0;
        virtual void update(const void* data, size_t length) = 0;
    };

    std::unique_ptr<crc32_context_t> create_crc32_context(crc32_value_t initial = 0);
    std::unique_ptr<crc32_context_t> create_crc32_context_ref(crc32_value_t initial = 0);
    std::unique_ptr<crc32_context_t> create_crc32_context_ia32(crc32_value_t initial = 0);
    std::unique_ptr<crc32_context_t> create_crc32_context_avx2(crc32_value_t initial = 0);
    std::unique_ptr<crc32_context_t> create_crc32_context_avx2clmul(crc32_value_t initial = 0);
}
