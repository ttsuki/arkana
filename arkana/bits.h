/// @file
/// @brief	arkana::bits
/// @author Copyright(c) 2021 ttsuki
/// 
/// This software is released under the MIT License.
/// https://opensource.org/licenses/MIT

#pragma once

#include "./bits/types.h"
#include "./bits/table.h"
#include "./bits/macros.h"
#include "./bits/bit-manip.h"
#include "./bits/memory.h"
#include "./bits/cpuid.h"
#include "./bits/hex-int-literals.h"
#include "./bits/uint128.h"

#ifdef __AVX2__
#include "./bits/xmm.h"
#endif
