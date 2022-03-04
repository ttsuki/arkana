/// @file
/// @brief	arkana::bits
/// @author Copyright(c) 2021 ttsuki
/// 
/// This software is released under the MIT License.
/// https://opensource.org/licenses/MIT

#pragma once

#include <arkana/bits/types.h>
#include <arkana/bits/table.h>
#include <arkana/bits/macros.h>
#include <arkana/bits/bit-manip.h>
#include <arkana/bits/memory.h>
#include <arkana/bits/cpuid.h>
#include <arkana/bits/hex-int-literals.h>
#include <arkana/bits/uint128.h>
#include <arkana/bits/base64-decoder.h>

#ifdef __AVX2__
#include <arkana/bits/xmm.h>
#endif
