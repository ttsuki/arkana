/// @file
/// @brief	arkana::bits - bit operation library
/// @author Copyright(c) 2021 ttsuki
/// 
/// This software is released under the MIT License.
/// https://opensource.org/licenses/MIT

#pragma once
#include <cstddef>
#include <cstdint>
#include <cstdlib>

#ifdef _MSC_VER
#include <intrin.h>
#else
#include <x86intrin.h>
#endif

namespace arkana::bits
{
	namespace naive
	{
		static inline constexpr uint8_t rotl(uint8_t v, int i) noexcept { return static_cast<uint8_t>(v << (i & 7) | v >> (-i & 7)); }
		static inline constexpr uint16_t rotl(uint16_t v, int i) noexcept { return static_cast<uint16_t>(v << (i & 15) | v >> (-i & 15)); }
		static inline constexpr uint32_t rotl(uint32_t v, int i) noexcept { return static_cast<uint32_t>(v << (i & 31) | v >> (-i & 31)); }
		static inline constexpr uint64_t rotl(uint64_t v, int i) noexcept { return static_cast<uint64_t>(v << (i & 63) | v >> (-i & 63)); }

		static inline constexpr uint8_t rotr(uint8_t v, int i) noexcept { return rotl(v, -i); }
		static inline constexpr uint16_t rotr(uint16_t v, int i) noexcept { return rotl(v, -i); }
		static inline constexpr uint32_t rotr(uint32_t v, int i) noexcept { return rotl(v, -i); }
		static inline constexpr uint64_t rotr(uint64_t v, int i) noexcept { return rotl(v, -i); }

		static inline constexpr uint8_t byteswap(uint8_t v) noexcept { return v; }
		static inline constexpr uint16_t byteswap(uint16_t v) noexcept { return static_cast<uint16_t>(v << 8 | v >> 8); }
		static inline constexpr uint32_t byteswap(uint32_t v) noexcept { return v << 24 | (v >> 8 & 0xFF00) | (v & 0xFF00) << 8 | v >> 24; }

		static inline constexpr uint64_t byteswap(uint64_t v) noexcept
		{
			v = (v >> 8 & 0x00FF00FF00FF00FFull) | (v & 0x00FF00FF00FF00FFull) << 8;
			v = (v >> 16 & 0x0000FFFF0000FFFFull) | (v & 0x0000FFFF0000FFFFull) << 16;
			v = (v >> 32 & 0x00000000FFFFFFFFull) | (v & 0x00000000FFFFFFFFull) << 32;
			return v;
		}
	}

#if defined(_MSC_VER)
	static inline uint8_t rotl(uint8_t v, int i) noexcept { return ::_rotl8(v, static_cast<unsigned char>(i)); }
	static inline uint16_t rotl(uint16_t v, int i) noexcept { return ::_rotl16(v, static_cast<unsigned char>(i)); }
	static inline uint32_t rotl(uint32_t v, int i) noexcept { return ::_rotl(v, i); }
	static inline uint64_t rotl(uint64_t v, int i) noexcept { return ::_rotl64(v, i); }
	static inline uint8_t rotr(uint8_t v, int i) noexcept { return ::_rotr8(v, static_cast<unsigned char>(i)); }
	static inline uint16_t rotr(uint16_t v, int i) noexcept { return ::_rotr16(v, static_cast<unsigned char>(i)); }
	static inline uint32_t rotr(uint32_t v, int i) noexcept { return ::_rotr(v, i); }
	static inline uint64_t rotr(uint64_t v, int i) noexcept { return ::_rotr64(v, i); }
	static inline uint8_t byteswap(uint8_t v) noexcept { return v; }
	static inline uint16_t byteswap(uint16_t v) noexcept { return ::_byteswap_ushort(v); }
	static inline uint32_t byteswap(uint32_t v) noexcept { return ::_byteswap_ulong(v); }
	static inline uint64_t byteswap(uint64_t v) noexcept { return ::_byteswap_uint64(v); }
#elif defined(__clang__)
	static inline uint8_t rotl(uint8_t v, int i) noexcept { return ::__builtin_rotateleft8(v, static_cast<unsigned char>(i)); }
	static inline uint16_t rotl(uint16_t v, int i) noexcept { return ::__builtin_rotateleft16(v,  static_cast<unsigned char>(i)); }
	static inline uint32_t rotl(uint32_t v, int i) noexcept { return ::__builtin_rotateleft32(v,  static_cast<unsigned char>(i)); }
	static inline uint64_t rotl(uint64_t v, int i) noexcept { return ::__builtin_rotateleft64(v,  static_cast<unsigned char>(i)); }
	static inline uint8_t rotr(uint8_t v, int i) noexcept { return ::__builtin_rotateright8(v, static_cast<unsigned char>(i)); }
	static inline uint16_t rotr(uint16_t v, int i) noexcept { return ::__builtin_rotateright16(v,  static_cast<unsigned char>(i)); }
	static inline uint32_t rotr(uint32_t v, int i) noexcept { return ::__builtin_rotateright32(v,  static_cast<unsigned char>(i)); }
	static inline uint64_t rotr(uint64_t v, int i) noexcept { return ::__builtin_rotateright64(v,  static_cast<unsigned char>(i)); }
	static inline uint8_t byteswap(uint8_t v) noexcept { return v; }
	static inline uint16_t byteswap(uint16_t v) noexcept { return ::__builtin_bswap16(v); }
	static inline uint32_t byteswap(uint32_t v) noexcept { return ::__builtin_bswap32(v); }
	static inline uint64_t byteswap(uint64_t v) noexcept { return ::__builtin_bswap64(v); }
#elif defined(__GNUC__)
	static inline uint8_t rotl(uint8_t v, int i) noexcept { return ::__rolb(v, i); }
	static inline uint16_t rotl(uint16_t v, int i) noexcept { return ::__rolw(v, i); }
	static inline uint32_t rotl(uint32_t v, int i) noexcept { return ::__rold(v, i); }
	static inline uint64_t rotl(uint64_t v, int i) noexcept { return ::__rolq(v, i); }
	static inline uint8_t rotr(uint8_t v, int i) noexcept { return ::__rorb(v, i); }
	static inline uint16_t rotr(uint16_t v, int i) noexcept { return ::__rorw(v, i); }
	static inline uint32_t rotr(uint32_t v, int i) noexcept { return ::__rord(v, i); }
	static inline uint64_t rotr(uint64_t v, int i) noexcept { return ::__rorq(v, i); }
	static inline uint8_t byteswap(uint8_t v) noexcept { return v; }
	static inline uint16_t byteswap(uint16_t v) noexcept { return ::__builtin_bswap16(v); }
	static inline uint32_t byteswap(uint32_t v) noexcept { return ::__builtin_bswap32(v); }
	static inline uint64_t byteswap(uint64_t v) noexcept { return ::__builtin_bswap64(v); }
#else
	using naive::rotl;
	using naive::rotr;
	using naive::byteswap;
#endif
}
