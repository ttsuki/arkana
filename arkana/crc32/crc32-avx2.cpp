/// @file
/// @brief	arkana::crc32
///			- An implementation of CRC32
/// @author Copyright(c) 2020 ttsuki
/// 
/// This software is released under the MIT License.
/// https://opensource.org/licenses/MIT

#include "../crc32.h"
#include "./crc32.private.hpp"

#include "./crc32-avx2.h"

std::unique_ptr<arkana::crc32::crc32_context_t> arkana::crc32::create_crc32_context_avx2(crc32_value_t initial)
{
    return std::make_unique<crc32_context_impl_t<avx2::calculate_crc32<0xEDB88320>>>(initial);
}
