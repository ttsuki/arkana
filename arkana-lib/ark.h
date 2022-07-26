/// @file
/// @brief	arkana::ark
/// @author Copyright(c) 2021 ttsuki
/// 
/// This software is released under the MIT License.
/// https://opensource.org/licenses/MIT

#pragma once

#include <arkana/ark/types.h>
#include <arkana/ark/table.h>
#include <arkana/ark/macros.h>
#include <arkana/ark/bit-manip.h>
#include <arkana/ark/memory.h>
#include <arkana/ark/cpuid.h>
#include <arkana/ark/hexilit.h>
#include <arkana/ark/uint128.h>
#include <arkana/ark/base64-decoder.h>

#ifdef __AVX2__
#include <arkana/ark/xmm.h>
#endif
