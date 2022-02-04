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

#include "../crc32.h"
#include "./crc32.private.hpp"

#include "./crc32-avx2clmul.h"

std::unique_ptr<arkana::crc32::crc32_context_t> arkana::crc32::create_crc32_context_avx2clmul(crc32_value_t initial)
{
    return std::make_unique<crc32_context_impl_t<avx2clmul::calculate_crc32<0xEDB88320>>>(initial);
}
