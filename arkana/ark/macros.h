/// @file
/// @brief	arkana::config
/// @author Copyright(c) 2021 ttsuki
/// 
/// This software is released under the MIT License.
/// https://opensource.org/licenses/MIT
#pragma once

#include <cstddef>
#include <cstdint>
#include <cassert>

#ifdef NDEBUG
#if defined(_MSC_VER)
#define ARKANA_FORCEINLINE inline __forceinline
#define ARKANA_ASSUME __assume
#else // ifdef _MSC_VER
#define ARKANA_FORCEINLINE inline __attribute__((always_inline))
#define ARKANA_ASSUME do { if (!(cond)) __builtin_unreachable(); } while (0)
#endif
#else
#define ARKANA_FORCEINLINE
#define ARKANA_ASSUME assert
#endif

