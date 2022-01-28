/// @file
/// @brief	arkana::xmm - x86 SIMD Operation wrappers
/// @author Copyright(c) 2021 ttsuki
/// 
/// This software is released under the MIT License.
/// https://opensource.org/licenses/MIT

#pragma once

#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <array>

#if defined(_MSC_VER)
#include <intrin.h>
#else
#include <x86intrin.h>
#endif

#if __has_include("./hex-int-literals.h")
#  include "./hex-int-literals.h"
#endif

#ifdef NDEBUG
#if defined(_MSC_VER)
#define ARKXMM_INLINE inline __forceinline
#define ARKXMM_VECTORCALL __vectorcall
#else
#define ARKXMM_INLINE inline __attribute__((always_inline))
#define ARKXMM_VECTORCALL
#endif
#else
#define ARKXMM_INLINE inline
#define ARKXMM_VECTORCALL
#endif

#define ARKXMM_API  static ARKXMM_INLINE auto ARKXMM_VECTORCALL
#define ARKXMM_DEFINE_EXTENSION(...) decltype(__VA_ARGS__) { return (__VA_ARGS__); }

namespace arkana::xmm
{
    /// wraps 128bit int (to suppress gcc's warning `ignoring attributes on template argument...')
    struct xm128
    {
        __m128i i;
    };

    /// SSE __m128i
    template <class T>
    struct alignas(16) XMM
    {
        __m128i v;
        using elem_t = T;
        static constexpr inline size_t elem_bits = sizeof(elem_t) * 8;
        static constexpr inline size_t size = sizeof(__m128i) / sizeof(elem_t);
        using array_t = std::array<elem_t, size>;
    };

    using vi8x16 = XMM<int8_t>;
    using vu8x16 = XMM<uint8_t>;
    using vi16x8 = XMM<int16_t>;
    using vu16x8 = XMM<uint16_t>;
    using vi32x4 = XMM<int32_t>;
    using vu32x4 = XMM<uint32_t>;
    using vi64x2 = XMM<int64_t>;
    using vu64x2 = XMM<uint64_t>;
    using vx128x1 = XMM<xm128>;

    /// AVX __m256i
    template <class T>
    struct alignas(32) YMM
    {
        __m256i v;
        using elem_t = T;
        static constexpr inline size_t elem_bits = sizeof(elem_t) * 8;
        static constexpr inline size_t size = sizeof(__m256i) / sizeof(elem_t);
        using array_t = std::array<elem_t, size>;
    };

    using vi8x32 = YMM<int8_t>;
    using vu8x32 = YMM<uint8_t>;
    using vi16x16 = YMM<int16_t>;
    using vu16x16 = YMM<uint16_t>;
    using vi32x8 = YMM<int32_t>;
    using vu32x8 = YMM<uint32_t>;
    using vi64x4 = YMM<int64_t>;
    using vu64x4 = YMM<uint64_t>;
    using vx128x2 = YMM<xm128>;

    /// Register pair
    template <class T>
    struct PAIR
    {
        T l, r;
        using elem_t = typename T::elem_t;
        static constexpr inline size_t elem_bits = T::elem_bits;
        static constexpr inline int size = sizeof(T) * 2 / sizeof(elem_t);
        using array_t = std::array<elem_t, size>;
    };

    struct SHIFT
    {
        int64_t i;

        explicit SHIFT(int64_t i) : i(i)
        {
        }

        ARKXMM_INLINE ARKXMM_VECTORCALL operator XMM<int64_t>() const { return {_mm_set1_epi64x(i)}; }
        ARKXMM_INLINE ARKXMM_VECTORCALL operator YMM<int64_t>() const { return {_mm256_set1_epi64x(i)}; }
        ARKXMM_INLINE ARKXMM_VECTORCALL operator __m128i() const { return _mm_set1_epi64x(i); }
        ARKXMM_INLINE ARKXMM_VECTORCALL operator __m256i() const { return _mm256_set1_epi64x(i); }
    };

    namespace enable
    {
        template <class, template <class> class>
        struct is_instance_of : std::false_type
        {
        };

        template <class T, template <class> class U>
        struct is_instance_of<U<T>, U> : std::true_type
        {
        };

        template <class T> static inline constexpr bool is_xmm_v = is_instance_of<T, XMM>::value; // T is XMM
        template <class T> static inline constexpr bool is_ymm_v = is_instance_of<T, YMM>::value; // T is YMM
        template <class T> static inline constexpr bool is_nmm_v = is_xmm_v<T> || is_ymm_v<T>;    // T is ?MM

        template <class XMM> using if_XMM = std::enable_if_t<is_xmm_v<XMM>, XMM>;
        template <class XMM> using if_8x16 = std::enable_if_t<is_xmm_v<XMM> && XMM::elem_bits == 8, XMM>;
        template <class XMM> using if_16x8 = std::enable_if_t<is_xmm_v<XMM> && XMM::elem_bits == 16, XMM>;
        template <class XMM> using if_32x4 = std::enable_if_t<is_xmm_v<XMM> && XMM::elem_bits == 32, XMM>;
        template <class XMM> using if_64x2 = std::enable_if_t<is_xmm_v<XMM> && XMM::elem_bits == 64, XMM>;

        template <class YMM> using if_YMM = std::enable_if_t<is_ymm_v<YMM>, YMM>;
        template <class YMM> using if_8x32 = std::enable_if_t<is_ymm_v<YMM> && YMM::elem_bits == 8, YMM>;
        template <class YMM> using if_16x16 = std::enable_if_t<is_ymm_v<YMM> && YMM::elem_bits == 16, YMM>;
        template <class YMM> using if_32x8 = std::enable_if_t<is_ymm_v<YMM> && YMM::elem_bits == 32, YMM>;
        template <class YMM> using if_64x4 = std::enable_if_t<is_ymm_v<YMM> && YMM::elem_bits == 64, YMM>;

        template <class NMM> using if_NMM = std::enable_if_t<is_nmm_v<NMM>, NMM>;
        template <class NMM> using if_8xN = std::enable_if_t<is_nmm_v<NMM> && NMM::elem_bits == 8, NMM>;
        template <class NMM> using if_16xN = std::enable_if_t<is_nmm_v<NMM> && NMM::elem_bits == 16, NMM>;
        template <class NMM> using if_32xN = std::enable_if_t<is_nmm_v<NMM> && NMM::elem_bits == 32, NMM>;
        template <class NMM> using if_64xN = std::enable_if_t<is_nmm_v<NMM> && NMM::elem_bits == 64, NMM>;
    }

    template <class XMM> ARKXMM_API load_u(const XMM* src) -> enable::if_XMM<XMM> { return XMM{_mm_lddqu_si128(&src->v)}; }          // SSE3
    template <class XMM> ARKXMM_API load_a(const XMM* src) -> enable::if_XMM<XMM> { return XMM{_mm_load_si128(&src->v)}; }           // SSE2
    template <class XMM> ARKXMM_API load_s(const XMM* src) -> enable::if_XMM<XMM> { return XMM{_mm_stream_load_si128(&src->v)}; }    // SSE41
    template <class YMM> ARKXMM_API load_u(const YMM* src) -> enable::if_YMM<YMM> { return YMM{_mm256_lddqu_si256(&src->v)}; }       // AVX
    template <class YMM> ARKXMM_API load_a(const YMM* src) -> enable::if_YMM<YMM> { return YMM{_mm256_load_si256(&src->v)}; }        // AVX
    template <class YMM> ARKXMM_API load_s(const YMM* src) -> enable::if_YMM<YMM> { return YMM{_mm256_stream_load_si256(&src->v)}; } // AVX2

    template <class XMM> ARKXMM_API store_u(XMM* dst, const XMM v) -> enable::if_XMM<XMM> { return _mm_storeu_si128(&dst->v, v.v), v; }    // SSE2
    template <class XMM> ARKXMM_API store_a(XMM* dst, const XMM v) -> enable::if_XMM<XMM> { return _mm_store_si128(&dst->v, v.v), v; }     // SSE2
    template <class XMM> ARKXMM_API store_s(XMM* dst, const XMM v) -> enable::if_XMM<XMM> { return _mm_stream_si128(&dst->v, v.v), v; }    // SSE2
    template <class YMM> ARKXMM_API store_u(YMM* dst, const YMM v) -> enable::if_YMM<YMM> { return _mm256_storeu_si256(&dst->v, v.v), v; } // AVX
    template <class YMM> ARKXMM_API store_a(YMM* dst, const YMM v) -> enable::if_YMM<YMM> { return _mm256_store_si256(&dst->v, v.v), v; }  // AVX
    template <class YMM> ARKXMM_API store_s(YMM* dst, const YMM v) -> enable::if_YMM<YMM> { return _mm256_stream_si256(&dst->v, v.v), v; } // AVX2

    /// to array
    template <class NMM> ARKXMM_API to_array(NMM v) -> typename NMM::array_t
    {
        typename NMM::array_t a;
        store_u(reinterpret_cast<NMM*>(a.data()), v);
        return a;
    }

    template <class To, class T> ARKXMM_API reinterpret(XMM<T> v) -> enable::if_XMM<To> { return To{v.v}; } // cast XMM to another XMM
    template <class To, class T> ARKXMM_API reinterpret(YMM<T> v) -> enable::if_YMM<To> { return To{v.v}; } // cast YMM to another YMM

    template <class XMM> ARKXMM_API zero() -> enable::if_XMM<XMM> { return {_mm_setzero_si128()}; }    // SSE2
    template <class YMM> ARKXMM_API zero() -> enable::if_YMM<YMM> { return {_mm256_setzero_si256()}; } // AVX

    // broadcast - use as `broadcast<vu32x4>(123)`
    template <class XMM> ARKXMM_API broadcast(typename XMM::elem_t val) -> enable::if_8x16<XMM> { return {_mm_set1_epi8(static_cast<int8_t>(val))}; }       // SSE2
    template <class XMM> ARKXMM_API broadcast(typename XMM::elem_t val) -> enable::if_16x8<XMM> { return {_mm_set1_epi16(static_cast<int16_t>(val))}; }     // SSE2
    template <class XMM> ARKXMM_API broadcast(typename XMM::elem_t val) -> enable::if_32x4<XMM> { return {_mm_set1_epi32(static_cast<int32_t>(val))}; }     // SSE2
    template <class XMM> ARKXMM_API broadcast(typename XMM::elem_t val) -> enable::if_64x2<XMM> { return {_mm_set1_epi64x(static_cast<int64_t>(val))}; }    // SSE2
    template <class YMM> ARKXMM_API broadcast(typename YMM::elem_t val) -> enable::if_8x32<YMM> { return {_mm256_set1_epi8(static_cast<int8_t>(val))}; }    // AVX
    template <class YMM> ARKXMM_API broadcast(typename YMM::elem_t val) -> enable::if_16x16<YMM> { return {_mm256_set1_epi16(static_cast<int16_t>(val))}; } // AVX
    template <class YMM> ARKXMM_API broadcast(typename YMM::elem_t val) -> enable::if_32x8<YMM> { return {_mm256_set1_epi32(static_cast<int32_t>(val))}; }  // AVX
    template <class YMM> ARKXMM_API broadcast(typename YMM::elem_t val) -> enable::if_64x4<YMM> { return {_mm256_set1_epi64x(static_cast<int64_t>(val))}; } // AVX
    template <class YMM> ARKXMM_API broadcast(XMM<typename YMM::elem_t> val) -> enable::if_YMM<YMM> { return {_mm256_broadcastsi128_si256(val.v)}; }        // AVX2

    // from_values - use as `from_values<vu32x4>(1, 2, 3, 4)`
    template <class XMM> ARKXMM_API from_values(typename XMM::elem_t x0, typename XMM::elem_t x1, typename XMM::elem_t x2, typename XMM::elem_t x3, typename XMM::elem_t x4, typename XMM::elem_t x5, typename XMM::elem_t x6, typename XMM::elem_t x7, typename XMM::elem_t x8, typename XMM::elem_t x9, typename XMM::elem_t xA, typename XMM::elem_t xB, typename XMM::elem_t xC, typename XMM::elem_t xD, typename XMM::elem_t xE, typename XMM::elem_t xF) -> enable::if_8x16<XMM> { return {_mm_setr_epi8(static_cast<int8_t>(x0), static_cast<int8_t>(x1), static_cast<int8_t>(x2), static_cast<int8_t>(x3), static_cast<int8_t>(x4), static_cast<int8_t>(x5), static_cast<int8_t>(x6), static_cast<int8_t>(x7), static_cast<int8_t>(x8), static_cast<int8_t>(x9), static_cast<int8_t>(xA), static_cast<int8_t>(xB), static_cast<int8_t>(xC), static_cast<int8_t>(xD), static_cast<int8_t>(xE), static_cast<int8_t>(xF))}; }
    template <class XMM> ARKXMM_API from_values(typename XMM::elem_t x0, typename XMM::elem_t x1, typename XMM::elem_t x2, typename XMM::elem_t x3, typename XMM::elem_t x4, typename XMM::elem_t x5, typename XMM::elem_t x6, typename XMM::elem_t x7) -> enable::if_16x8<XMM> { return {_mm_setr_epi16(static_cast<int16_t>(x0), static_cast<int16_t>(x1), static_cast<int16_t>(x2), static_cast<int16_t>(x3), static_cast<int16_t>(x4), static_cast<int16_t>(x5), static_cast<int16_t>(x6), static_cast<int16_t>(x7))}; }
    template <class XMM> ARKXMM_API from_values(typename XMM::elem_t x0, typename XMM::elem_t x1, typename XMM::elem_t x2, typename XMM::elem_t x3) -> enable::if_32x4<XMM> { return {_mm_setr_epi32(static_cast<int32_t>(x0), static_cast<int32_t>(x1), static_cast<int32_t>(x2), static_cast<int32_t>(x3))}; }
    template <class XMM> ARKXMM_API from_values(typename XMM::elem_t x0, typename XMM::elem_t x1) -> enable::if_64x2<XMM> { return {_mm_set_epi64x(static_cast<int64_t>(x1), static_cast<int64_t>(x0))}; }
    template <class YMM> ARKXMM_API from_values(typename YMM::elem_t x00, typename YMM::elem_t x01, typename YMM::elem_t x02, typename YMM::elem_t x03, typename YMM::elem_t x04, typename YMM::elem_t x05, typename YMM::elem_t x06, typename YMM::elem_t x07, typename YMM::elem_t x08, typename YMM::elem_t x09, typename YMM::elem_t x0A, typename YMM::elem_t x0B, typename YMM::elem_t x0C, typename YMM::elem_t x0D, typename YMM::elem_t x0E, typename YMM::elem_t x0F, typename YMM::elem_t x10, typename YMM::elem_t x11, typename YMM::elem_t x12, typename YMM::elem_t x13, typename YMM::elem_t x14, typename YMM::elem_t x15, typename YMM::elem_t x16, typename YMM::elem_t x17, typename YMM::elem_t x18, typename YMM::elem_t x19, typename YMM::elem_t x1A, typename YMM::elem_t x1B, typename YMM::elem_t x1C, typename YMM::elem_t x1D, typename YMM::elem_t x1E, typename YMM::elem_t x1F) -> enable::if_8x32<YMM> { return {_mm256_setr_epi8(static_cast<int8_t>(x00), static_cast<int8_t>(x01), static_cast<int8_t>(x02), static_cast<int8_t>(x03), static_cast<int8_t>(x04), static_cast<int8_t>(x05), static_cast<int8_t>(x06), static_cast<int8_t>(x07), static_cast<int8_t>(x08), static_cast<int8_t>(x09), static_cast<int8_t>(x0A), static_cast<int8_t>(x0B), static_cast<int8_t>(x0C), static_cast<int8_t>(x0D), static_cast<int8_t>(x0E), static_cast<int8_t>(x0F), static_cast<int8_t>(x10), static_cast<int8_t>(x11), static_cast<int8_t>(x12), static_cast<int8_t>(x13), static_cast<int8_t>(x14), static_cast<int8_t>(x15), static_cast<int8_t>(x16), static_cast<int8_t>(x17), static_cast<int8_t>(x18), static_cast<int8_t>(x19), static_cast<int8_t>(x1A), static_cast<int8_t>(x1B), static_cast<int8_t>(x1C), static_cast<int8_t>(x1D), static_cast<int8_t>(x1E), static_cast<int8_t>(x1F))}; }
    template <class YMM> ARKXMM_API from_values(typename YMM::elem_t x0, typename YMM::elem_t x1, typename YMM::elem_t x2, typename YMM::elem_t x3, typename YMM::elem_t x4, typename YMM::elem_t x5, typename YMM::elem_t x6, typename YMM::elem_t x7, typename YMM::elem_t x8, typename YMM::elem_t x9, typename YMM::elem_t xA, typename YMM::elem_t xB, typename YMM::elem_t xC, typename YMM::elem_t xD, typename YMM::elem_t xE, typename YMM::elem_t xF) -> enable::if_16x16<YMM> { return {_mm256_setr_epi16(static_cast<int16_t>(x0), static_cast<int16_t>(x1), static_cast<int16_t>(x2), static_cast<int16_t>(x3), static_cast<int16_t>(x4), static_cast<int16_t>(x5), static_cast<int16_t>(x6), static_cast<int16_t>(x7), static_cast<int16_t>(x8), static_cast<int16_t>(x9), static_cast<int16_t>(xA), static_cast<int16_t>(xB), static_cast<int16_t>(xC), static_cast<int16_t>(xD), static_cast<int16_t>(xE), static_cast<int16_t>(xF))}; }
    template <class YMM> ARKXMM_API from_values(typename YMM::elem_t x0, typename YMM::elem_t x1, typename YMM::elem_t x2, typename YMM::elem_t x3, typename YMM::elem_t x4, typename YMM::elem_t x5, typename YMM::elem_t x6, typename YMM::elem_t x7) -> enable::if_32x8<YMM> { return {_mm256_setr_epi32(static_cast<int32_t>(x0), static_cast<int32_t>(x1), static_cast<int32_t>(x2), static_cast<int32_t>(x3), static_cast<int32_t>(x4), static_cast<int32_t>(x5), static_cast<int32_t>(x6), static_cast<int32_t>(x7))}; }
    template <class YMM> ARKXMM_API from_values(typename YMM::elem_t x0, typename YMM::elem_t x1, typename YMM::elem_t x2, typename YMM::elem_t x3) -> enable::if_64x4<YMM> { return {_mm256_setr_epi64x(static_cast<int64_t>(x0), static_cast<int64_t>(x1), static_cast<int64_t>(x2), static_cast<int64_t>(x3))}; }
    template <class YMM> ARKXMM_API from_values(XMM<typename YMM::elem_t> x0, XMM<typename YMM::elem_t> x1) -> enable::if_YMM<YMM> { return {_mm256_setr_m128i(x0.v, x1.v)}; }

    // bitwise operators
    template <class T> ARKXMM_API operator ~(const XMM<T> a) -> XMM<T> { return {_mm_xor_si128(a.v, _mm_cmpeq_epi32(a.v, a.v))}; }       // SSE2
    template <class T> ARKXMM_API operator ~(const YMM<T> a) -> YMM<T> { return {_mm256_xor_si256(a.v, _mm256_cmpeq_epi32(a.v, a.v))}; } // AVX2
    template <class T> ARKXMM_API operator &(const XMM<T> a, const XMM<T> b) -> XMM<T> { return {_mm_and_si128(a.v, b.v)}; }             // SSE2
    template <class T> ARKXMM_API operator &(const YMM<T> a, const YMM<T> b) -> YMM<T> { return {_mm256_and_si256(a.v, b.v)}; }          // AVX2
    template <class T> ARKXMM_API operator |(const XMM<T> a, const XMM<T> b) -> XMM<T> { return {_mm_or_si128(a.v, b.v)}; }              // SSE2
    template <class T> ARKXMM_API operator |(const YMM<T> a, const YMM<T> b) -> YMM<T> { return {_mm256_or_si256(a.v, b.v)}; }           // AVX2
    template <class T> ARKXMM_API operator ^(const XMM<T> a, const XMM<T> b) -> XMM<T> { return {_mm_xor_si128(a.v, b.v)}; }             // SSE2
    template <class T> ARKXMM_API operator ^(const YMM<T> a, const YMM<T> b) -> YMM<T> { return {_mm256_xor_si256(a.v, b.v)}; }          // AVX2
    template <class T> ARKXMM_API andnot(const XMM<T> a, const XMM<T> mask) -> XMM<T> { return {_mm_andnot_si128(mask.v, a.v)}; }        // SSE2 a & ~mask
    template <class T> ARKXMM_API andnot(const YMM<T> a, const YMM<T> mask) -> YMM<T> { return {_mm256_andnot_si256(mask.v, a.v)}; }     // AVX2 a & ~mask

    template <int bytes, class T> ARKXMM_API byte_shift_l_128(XMM<T> reg) -> XMM<T> { return {_mm_slli_si128(reg.v, bytes)}; }                          // SSE2
    template <int bytes, class T> ARKXMM_API byte_shift_l_128(YMM<T> reg) -> YMM<T> { return {_mm256_slli_si256(reg.v, bytes)}; }                       // AVX2
    template <int bytes, class T> ARKXMM_API byte_shift_r_128(XMM<T> reg) -> XMM<T> { return {_mm_srli_si128(reg.v, bytes)}; }                          // SSE2
    template <int bytes, class T> ARKXMM_API byte_shift_r_128(YMM<T> reg) -> YMM<T> { return {_mm256_srli_si256(reg.v, bytes)}; }                       // AVX2
    template <int bytes, class T> ARKXMM_API byte_align_r_128(XMM<T> lo, XMM<T> hi) -> XMM<T> { return {_mm_alignr_epi8(hi.v, lo.v, bytes)}; }          // SSSE3
    template <int bytes, class T> ARKXMM_API byte_align_r_128(YMM<T> lo, YMM<T> hi) -> YMM<T> { return {_mm256_alignr_epi8(hi.v, lo.v, bytes)}; }       // AVX2
    template <class T> ARKXMM_API byte_shuffle_128(XMM<T> val, XMM<int8_t> index) -> XMM<T> { return {_mm_shuffle_epi8(val.v, index.v)}; }              // SSSE3
    template <class T> ARKXMM_API byte_shuffle_128(YMM<T> val, YMM<int8_t> index) -> YMM<T> { return {_mm256_shuffle_epi8(val.v, index.v)}; }           // AVX2
    template <class T> ARKXMM_API byte_blend(XMM<T> a, XMM<T> b, XMM<int8_t> selector) -> XMM<T> { return {_mm_blendv_epi8(a.v, b.v, selector.v)}; }    // SSSE4.1
    template <class T> ARKXMM_API byte_blend(YMM<T> a, YMM<T> b, YMM<int8_t> selector) -> YMM<T> { return {_mm256_blendv_epi8(a.v, b.v, selector.v)}; } // AVX2

    // broadcast shortcut
    ARKXMM_API i8x16(int8_t v) -> vi8x16 { return broadcast<vi8x16>(v); }
    ARKXMM_API u8x16(uint8_t v) -> vu8x16 { return broadcast<vu8x16>(v); }
    ARKXMM_API i8x32(int8_t v) -> vi8x32 { return broadcast<vi8x32>(v); }
    ARKXMM_API u8x32(uint8_t v) -> vu8x32 { return broadcast<vu8x32>(v); }
    ARKXMM_API i16x8(int16_t v) -> vi16x8 { return broadcast<vi16x8>(v); }
    ARKXMM_API u16x8(uint16_t v) -> vu16x8 { return broadcast<vu16x8>(v); }
    ARKXMM_API i16x16(int16_t v) -> vi16x16 { return broadcast<vi16x16>(v); }
    ARKXMM_API u16x16(uint16_t v) -> vu16x16 { return broadcast<vu16x16>(v); }
    ARKXMM_API i32x4(int32_t v) -> vi32x4 { return broadcast<vi32x4>(v); }
    ARKXMM_API u32x4(uint32_t v) -> vu32x4 { return broadcast<vu32x4>(v); }
    ARKXMM_API i32x8(int32_t v) -> vi32x8 { return broadcast<vi32x8>(v); }
    ARKXMM_API u32x8(uint32_t v) -> vu32x8 { return broadcast<vu32x8>(v); }
    ARKXMM_API i64x2(int64_t v) -> vi64x2 { return broadcast<vi64x2>(v); }
    ARKXMM_API u64x2(uint64_t v) -> vu64x2 { return broadcast<vu64x2>(v); }
    ARKXMM_API i64x4(int64_t v) -> vi64x4 { return broadcast<vi64x4>(v); }
    ARKXMM_API u64x4(uint64_t v) -> vu64x4 { return broadcast<vu64x4>(v); }

    // from values shortcut
    ARKXMM_API i8x16(int8_t x0, int8_t x1, int8_t x2, int8_t x3, int8_t x4, int8_t x5, int8_t x6, int8_t x7, int8_t x8, int8_t x9, int8_t xA, int8_t xB, int8_t xC, int8_t xD, int8_t xE, int8_t xF) -> vi8x16 { return from_values<vi8x16>(x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, xA, xB, xC, xD, xE, xF); }
    ARKXMM_API u8x16(uint8_t x0, uint8_t x1, uint8_t x2, uint8_t x3, uint8_t x4, uint8_t x5, uint8_t x6, uint8_t x7, uint8_t x8, uint8_t x9, uint8_t xA, uint8_t xB, uint8_t xC, uint8_t xD, uint8_t xE, uint8_t xF) -> vu8x16 { return from_values<vu8x16>(x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, xA, xB, xC, xD, xE, xF); }
    ARKXMM_API i8x32(int8_t x00, int8_t x01, int8_t x02, int8_t x03, int8_t x04, int8_t x05, int8_t x06, int8_t x07, int8_t x08, int8_t x09, int8_t x0A, int8_t x0B, int8_t x0C, int8_t x0D, int8_t x0E, int8_t x0F, int8_t x10, int8_t x11, int8_t x12, int8_t x13, int8_t x14, int8_t x15, int8_t x16, int8_t x17, int8_t x18, int8_t x19, int8_t x1A, int8_t x1B, int8_t x1C, int8_t x1D, int8_t x1E, int8_t x1F) -> vi8x32 { return from_values<vi8x32>(x00, x01, x02, x03, x04, x05, x06, x07, x08, x09, x0A, x0B, x0C, x0D, x0E, x0F, x10, x11, x12, x13, x14, x15, x16, x17, x18, x19, x1A, x1B, x1C, x1D, x1E, x1F); }
    ARKXMM_API u8x32(uint8_t x00, uint8_t x01, uint8_t x02, uint8_t x03, uint8_t x04, uint8_t x05, uint8_t x06, uint8_t x07, uint8_t x08, uint8_t x09, uint8_t x0A, uint8_t x0B, uint8_t x0C, uint8_t x0D, uint8_t x0E, uint8_t x0F, uint8_t x10, uint8_t x11, uint8_t x12, uint8_t x13, uint8_t x14, uint8_t x15, uint8_t x16, uint8_t x17, uint8_t x18, uint8_t x19, uint8_t x1A, uint8_t x1B, uint8_t x1C, uint8_t x1D, uint8_t x1E, uint8_t x1F) -> vu8x32 { return from_values<vu8x32>(x00, x01, x02, x03, x04, x05, x06, x07, x08, x09, x0A, x0B, x0C, x0D, x0E, x0F, x10, x11, x12, x13, x14, x15, x16, x17, x18, x19, x1A, x1B, x1C, x1D, x1E, x1F); }
    ARKXMM_API i16x8(int16_t x0, int16_t x1, int16_t x2, int16_t x3, int16_t x4, int16_t x5, int16_t x6, int16_t x7) -> vi16x8 { return from_values<vi16x8>(x0, x1, x2, x3, x4, x5, x6, x7); }
    ARKXMM_API u16x8(uint16_t x0, uint16_t x1, uint16_t x2, uint16_t x3, uint16_t x4, uint16_t x5, uint16_t x6, uint16_t x7) -> vu16x8 { return from_values<vu16x8>(x0, x1, x2, x3, x4, x5, x6, x7); }
    ARKXMM_API i16x16(int16_t x0, int16_t x1, int16_t x2, int16_t x3, int16_t x4, int16_t x5, int16_t x6, int16_t x7, int16_t x8, int16_t x9, int16_t xA, int16_t xB, int16_t xC, int16_t xD, int16_t xE, int16_t xF) -> vi16x16 { return from_values<vi16x16>(x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, xA, xB, xC, xD, xE, xF); }
    ARKXMM_API u16x16(uint16_t x0, uint16_t x1, uint16_t x2, uint16_t x3, uint16_t x4, uint16_t x5, uint16_t x6, uint16_t x7, uint16_t x8, uint16_t x9, uint16_t xA, uint16_t xB, uint16_t xC, uint16_t xD, uint16_t xE, uint16_t xF) -> vu16x16 { return from_values<vu16x16>(x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, xA, xB, xC, xD, xE, xF); }
    ARKXMM_API i32x4(int32_t x0, int32_t x1, int32_t x2, int32_t x3) -> vi32x4 { return from_values<vi32x4>(x0, x1, x2, x3); }
    ARKXMM_API u32x4(uint32_t x0, uint32_t x1, uint32_t x2, uint32_t x3) -> vu32x4 { return from_values<vu32x4>(x0, x1, x2, x3); }
    ARKXMM_API i32x8(int32_t x0, int32_t x1, int32_t x2, int32_t x3, int32_t x4, int32_t x5, int32_t x6, int32_t x7) -> vi32x8 { return from_values<vi32x8>(x0, x1, x2, x3, x4, x5, x6, x7); }
    ARKXMM_API u32x8(uint32_t x0, uint32_t x1, uint32_t x2, uint32_t x3, uint32_t x4, uint32_t x5, uint32_t x6, uint32_t x7) -> vu32x8 { return from_values<vu32x8>(x0, x1, x2, x3, x4, x5, x6, x7); }
    ARKXMM_API i64x2(int64_t x0, int64_t x1) -> vi64x2 { return from_values<vi64x2>(x0, x1); }
    ARKXMM_API u64x2(uint64_t x0, uint64_t x1) -> vu64x2 { return from_values<vu64x2>(x0, x1); }
    ARKXMM_API i64x4(int64_t x0, int64_t x1, int64_t x2, int64_t x3) -> vi64x4 { return from_values<vi64x4>(x0, x1, x2, x3); }
    ARKXMM_API u64x4(uint64_t x0, uint64_t x1, uint64_t x2, uint64_t x3) -> vu64x4 { return from_values<vu64x4>(x0, x1, x2, x3); }


    // insert single element into vector
    template <uint8_t index_4bit> ARKXMM_API insert_element(vi8x16 v, int8_t x) -> vi8x16 { return {_mm_insert_epi8(v.v, x, index_4bit)}; }        // SSE4.1
    template <uint8_t index_4bit> ARKXMM_API insert_element(vu8x16 v, int8_t x) -> vu8x16 { return {_mm_insert_epi8(v.v, x, index_4bit)}; }        // SSE4.1
    template <uint8_t index_5bit> ARKXMM_API insert_element(vi8x32 v, int8_t x) -> vi8x32 { return {_mm256_insert_epi8(v.v, x, index_5bit)}; }     // AVX
    template <uint8_t index_5bit> ARKXMM_API insert_element(vu8x32 v, int8_t x) -> vu8x32 { return {_mm256_insert_epi8(v.v, x, index_5bit)}; }     // AVX
    template <uint8_t index_3bit> ARKXMM_API insert_element(vi16x8 v, int16_t x) -> vi16x8 { return {_mm_insert_epi16(v.v, x, index_3bit)}; }      // SSE2
    template <uint8_t index_3bit> ARKXMM_API insert_element(vu16x8 v, int16_t x) -> vu16x8 { return {_mm_insert_epi16(v.v, x, index_3bit)}; }      // SSE2
    template <uint8_t index_4bit> ARKXMM_API insert_element(vi16x16 v, int16_t x) -> vi16x16 { return {_mm256_insert_epi16(v.v, x, index_4bit)}; } // AVX
    template <uint8_t index_4bit> ARKXMM_API insert_element(vu16x16 v, int16_t x) -> vu16x16 { return {_mm256_insert_epi16(v.v, x, index_4bit)}; } // AVX
    template <uint8_t index_2bit> ARKXMM_API insert_element(vi32x4 v, int32_t x) -> vi32x4 { return {_mm_insert_epi32(v.v, x, index_2bit)}; }      // SSE4.1
    template <uint8_t index_2bit> ARKXMM_API insert_element(vu32x4 v, int32_t x) -> vu32x4 { return {_mm_insert_epi32(v.v, x, index_2bit)}; }      // SSE4.1
    template <uint8_t index_3bit> ARKXMM_API insert_element(vi32x8 v, int32_t x) -> vi32x8 { return {_mm256_insert_epi32(v.v, x, index_3bit)}; }   // AVX
    template <uint8_t index_3bit> ARKXMM_API insert_element(vu32x8 v, int32_t x) -> vu32x8 { return {_mm256_insert_epi32(v.v, x, index_3bit)}; }   // AVX
    template <uint8_t index_1bit> ARKXMM_API insert_element(vi64x2 v, int64_t x) -> vi64x2 { return {_mm_insert_epi64(v.v, x, index_1bit)}; }      // SSE4.1
    template <uint8_t index_1bit> ARKXMM_API insert_element(vu64x2 v, int64_t x) -> vu64x2 { return {_mm_insert_epi64(v.v, x, index_1bit)}; }      // SSE4.1
    template <uint8_t index_2bit> ARKXMM_API insert_element(vi64x4 v, int64_t x) -> vi64x4 { return {_mm256_insert_epi64(v.v, x, index_2bit)}; }   // AVX
    template <uint8_t index_2bit> ARKXMM_API insert_element(vu64x4 v, int64_t x) -> vu64x4 { return {_mm256_insert_epi64(v.v, x, index_2bit)}; }   // AVX

    // extract single element from vector
    template <uint8_t index_4bit> ARKXMM_API extract_element(vi8x16 v) { return _mm_extract_epi8(v.v, index_4bit); }      // SSE4.1
    template <uint8_t index_4bit> ARKXMM_API extract_element(vu8x16 v) { return _mm_extract_epi8(v.v, index_4bit); }      // SSE4.1
    template <uint8_t index_5bit> ARKXMM_API extract_element(vi8x32 v) { return _mm256_extract_epi8(v.v, index_5bit); }   // AVX2
    template <uint8_t index_5bit> ARKXMM_API extract_element(vu8x32 v) { return _mm256_extract_epi8(v.v, index_5bit); }   // AVX2
    template <uint8_t index_3bit> ARKXMM_API extract_element(vi16x8 v) { return _mm_extract_epi16(v.v, index_3bit); }     // SSE2
    template <uint8_t index_3bit> ARKXMM_API extract_element(vu16x8 v) { return _mm_extract_epi16(v.v, index_3bit); }     // SSE2
    template <uint8_t index_4bit> ARKXMM_API extract_element(vi16x16 v) { return _mm256_extract_epi16(v.v, index_4bit); } // AVX2
    template <uint8_t index_4bit> ARKXMM_API extract_element(vu16x16 v) { return _mm256_extract_epi16(v.v, index_4bit); } // AVX2
    template <uint8_t index_2bit> ARKXMM_API extract_element(vi32x4 v) { return _mm_extract_epi32(v.v, index_2bit); }     // SSE4.1
    template <uint8_t index_2bit> ARKXMM_API extract_element(vu32x4 v) { return _mm_extract_epi32(v.v, index_2bit); }     // SSE4.1
    template <uint8_t index_3bit> ARKXMM_API extract_element(vi32x8 v) { return _mm256_extract_epi32(v.v, index_3bit); }  // AVX
    template <uint8_t index_3bit> ARKXMM_API extract_element(vu32x8 v) { return _mm256_extract_epi32(v.v, index_3bit); }  // AVX
    template <uint8_t index_1bit> ARKXMM_API extract_element(vi64x2 v) { return _mm_extract_epi64(v.v, index_1bit); }     // SSE4.1
    template <uint8_t index_1bit> ARKXMM_API extract_element(vu64x2 v) { return _mm_extract_epi64(v.v, index_1bit); }     // SSE4.1
    template <uint8_t index_2bit> ARKXMM_API extract_element(vi64x4 v) { return _mm256_extract_epi64(v.v, index_2bit); }  // AVX
    template <uint8_t index_2bit> ARKXMM_API extract_element(vu64x4 v) { return _mm256_extract_epi64(v.v, index_2bit); }  // AVX

    // blend
    ARKXMM_API blend(vi8x16 a, vi8x16 b, vi8x16 control) -> vi8x16 { return {_mm_blendv_epi8(a.v, b.v, control.v)}; }                              // SSE 4.1
    ARKXMM_API blend(vu8x16 a, vu8x16 b, vu8x16 control) -> vu8x16 { return {_mm_blendv_epi8(a.v, b.v, control.v)}; }                              // SSE 4.1
    ARKXMM_API blend(vi8x32 a, vi8x32 b, vi8x32 control) -> vi8x32 { return {_mm256_blendv_epi8(a.v, b.v, control.v)}; }                           // AVX2
    ARKXMM_API blend(vu8x32 a, vu8x32 b, vu8x32 control) -> vu8x32 { return {_mm256_blendv_epi8(a.v, b.v, control.v)}; }                           // AVX2
    template <int selector_1bit_x8> ARKXMM_API blend(vi16x8 a, vi16x8 b) -> vi16x8 { return {_mm_blend_epi16(a.v, b.v, selector_1bit_x8)}; }       // SSE 4.1
    template <int selector_1bit_x8> ARKXMM_API blend(vu16x8 a, vu16x8 b) -> vu16x8 { return {_mm_blend_epi16(a.v, b.v, selector_1bit_x8)}; }       // SSE 4.1
    template <int selector_1bit_x8> ARKXMM_API blend(vi16x16 a, vi16x16 b) -> vi16x16 { return {_mm256_blend_epi16(a.v, b.v, selector_1bit_x8)}; } // AVX2
    template <int selector_1bit_x8> ARKXMM_API blend(vu16x16 a, vu16x16 b) -> vu16x16 { return {_mm256_blend_epi16(a.v, b.v, selector_1bit_x8)}; } // AVX2
    template <int selector_1bit_x4> ARKXMM_API blend(vi32x4 a, vi32x4 b) -> vi32x4 { return {_mm_blend_epi32(a.v, b.v, selector_1bit_x4)}; }       // AVX2
    template <int selector_1bit_x4> ARKXMM_API blend(vu32x4 a, vu32x4 b) -> vu32x4 { return {_mm_blend_epi32(a.v, b.v, selector_1bit_x4)}; }       // AVX2
    template <int selector_1bit_x8> ARKXMM_API blend(vi32x8 a, vi32x8 b) -> vi32x8 { return {_mm256_blend_epi32(a.v, b.v, selector_1bit_x8)}; }    // AVX2
    template <int selector_1bit_x8> ARKXMM_API blend(vu32x8 a, vu32x8 b) -> vu32x8 { return {_mm256_blend_epi32(a.v, b.v, selector_1bit_x8)}; }    // AVX2
    template <int selector_2bit_x4> ARKXMM_API shuffle(vi32x4 v) -> vi32x4 { return {_mm_shuffle_epi32(v.v, selector_2bit_x4)}; }                  // SSE2
    template <int selector_2bit_x4> ARKXMM_API shuffle(vu32x4 v) -> vu32x4 { return {_mm_shuffle_epi32(v.v, selector_2bit_x4)}; }                  // SSE2
    template <int selector_2bit_x4> ARKXMM_API shuffle(vi32x8 v) -> vi32x8 { return {_mm256_shuffle_epi32(v.v, selector_2bit_x4)}; }               // AVX2
    template <int selector_2bit_x4> ARKXMM_API shuffle(vu32x8 v) -> vu32x8 { return {_mm256_shuffle_epi32(v.v, selector_2bit_x4)}; }               // AVX2

    ARKXMM_API abs(vi8x16 a) -> vi8x16 { return {_mm_abs_epi8(a.v)}; }       // SSSE3
    ARKXMM_API abs(vi8x32 a) -> vi8x32 { return {_mm256_abs_epi8(a.v)}; }    // AVX2
    ARKXMM_API abs(vi16x8 a) -> vi16x8 { return {_mm_abs_epi16(a.v)}; }      // SSSE3
    ARKXMM_API abs(vi16x16 a) -> vi16x16 { return {_mm256_abs_epi16(a.v)}; } // AVX2
    ARKXMM_API abs(vi32x4 a) -> vi32x4 { return {_mm_abs_epi32(a.v)}; }      // SSSE3
    ARKXMM_API abs(vi32x8 a) -> vi32x8 { return {_mm256_abs_epi32(a.v)}; }   // AVX2

    ARKXMM_API operator +(vi8x16 a, vi8x16 b) -> vi8x16 { return {_mm_add_epi8(a.v, b.v)}; }        // SSE2
    ARKXMM_API operator +(vu8x16 a, vu8x16 b) -> vu8x16 { return {_mm_add_epi8(a.v, b.v)}; }        // SSE2
    ARKXMM_API operator +(vi8x32 a, vi8x32 b) -> vi8x32 { return {_mm256_add_epi8(a.v, b.v)}; }     // SSE2
    ARKXMM_API operator +(vu8x32 a, vu8x32 b) -> vu8x32 { return {_mm256_add_epi8(a.v, b.v)}; }     // SSE2
    ARKXMM_API operator +(vi16x8 a, vi16x8 b) -> vi16x8 { return {_mm_add_epi16(a.v, b.v)}; }       // SSE2
    ARKXMM_API operator +(vu16x8 a, vu16x8 b) -> vu16x8 { return {_mm_add_epi16(a.v, b.v)}; }       // SSE2
    ARKXMM_API operator +(vi16x16 a, vi16x16 b) -> vi16x16 { return {_mm256_add_epi16(a.v, b.v)}; } // AVX2
    ARKXMM_API operator +(vu16x16 a, vu16x16 b) -> vu16x16 { return {_mm256_add_epi16(a.v, b.v)}; } // AVX2
    ARKXMM_API operator +(vi32x4 a, vi32x4 b) -> vi32x4 { return {_mm_add_epi32(a.v, b.v)}; }       // SSE2
    ARKXMM_API operator +(vu32x4 a, vu32x4 b) -> vu32x4 { return {_mm_add_epi32(a.v, b.v)}; }       // SSE2
    ARKXMM_API operator +(vi32x8 a, vi32x8 b) -> vi32x8 { return {_mm256_add_epi32(a.v, b.v)}; }    // AVX2
    ARKXMM_API operator +(vu32x8 a, vu32x8 b) -> vu32x8 { return {_mm256_add_epi32(a.v, b.v)}; }    // AVX2
    ARKXMM_API operator +(vi64x2 a, vi64x2 b) -> vi64x2 { return {_mm_add_epi64(a.v, b.v)}; }       // SSE2
    ARKXMM_API operator +(vu64x2 a, vu64x2 b) -> vu64x2 { return {_mm_add_epi64(a.v, b.v)}; }       // SSE2
    ARKXMM_API operator +(vi64x4 a, vi64x4 b) -> vi64x4 { return {_mm256_add_epi64(a.v, b.v)}; }    // AVX2
    ARKXMM_API operator +(vu64x4 a, vu64x4 b) -> vu64x4 { return {_mm256_add_epi64(a.v, b.v)}; }    // AVX2

    ARKXMM_API add_sat(vi8x16 a, vi8x16 b) -> vi8x16 { return {_mm_adds_epi8(a.v, b.v)}; }        // SSE2
    ARKXMM_API add_sat(vu8x16 a, vu8x16 b) -> vu8x16 { return {_mm_adds_epu8(a.v, b.v)}; }        // SSE2
    ARKXMM_API add_sat(vi16x8 a, vi16x8 b) -> vi16x8 { return {_mm_adds_epi16(a.v, b.v)}; }       // SSE2
    ARKXMM_API add_sat(vu16x8 a, vu16x8 b) -> vu16x8 { return {_mm_adds_epu16(a.v, b.v)}; }       // SSE2
    ARKXMM_API add_sat(vi8x32 a, vi8x32 b) -> vi8x32 { return {_mm256_adds_epi8(a.v, b.v)}; }     // AVX2
    ARKXMM_API add_sat(vu8x32 a, vu8x32 b) -> vu8x32 { return {_mm256_adds_epu8(a.v, b.v)}; }     // AVX2
    ARKXMM_API add_sat(vi16x16 a, vi16x16 b) -> vi16x16 { return {_mm256_adds_epi16(a.v, b.v)}; } // AVX2
    ARKXMM_API add_sat(vu16x16 a, vu16x16 b) -> vu16x16 { return {_mm256_adds_epu16(a.v, b.v)}; } // AVX2

    ARKXMM_API operator -(vi8x16 a, vi8x16 b) -> vi8x16 { return {_mm_sub_epi8(a.v, b.v)}; }        // SSE2
    ARKXMM_API operator -(vu8x16 a, vu8x16 b) -> vu8x16 { return {_mm_sub_epi8(a.v, b.v)}; }        // SSE2
    ARKXMM_API operator -(vi8x32 a, vi8x32 b) -> vi8x32 { return {_mm256_sub_epi8(a.v, b.v)}; }     // SSE2
    ARKXMM_API operator -(vu8x32 a, vu8x32 b) -> vu8x32 { return {_mm256_sub_epi8(a.v, b.v)}; }     // SSE2
    ARKXMM_API operator -(vi16x8 a, vi16x8 b) -> vi16x8 { return {_mm_sub_epi16(a.v, b.v)}; }       // SSE2
    ARKXMM_API operator -(vu16x8 a, vu16x8 b) -> vu16x8 { return {_mm_sub_epi16(a.v, b.v)}; }       // SSE2
    ARKXMM_API operator -(vi16x16 a, vi16x16 b) -> vi16x16 { return {_mm256_sub_epi16(a.v, b.v)}; } // AVX2
    ARKXMM_API operator -(vu16x16 a, vu16x16 b) -> vu16x16 { return {_mm256_sub_epi16(a.v, b.v)}; } // AVX2
    ARKXMM_API operator -(vi32x4 a, vi32x4 b) -> vi32x4 { return {_mm_sub_epi32(a.v, b.v)}; }       // SSE2
    ARKXMM_API operator -(vu32x4 a, vu32x4 b) -> vu32x4 { return {_mm_sub_epi32(a.v, b.v)}; }       // SSE2
    ARKXMM_API operator -(vi32x8 a, vi32x8 b) -> vi32x8 { return {_mm256_sub_epi32(a.v, b.v)}; }    // AVX2
    ARKXMM_API operator -(vu32x8 a, vu32x8 b) -> vu32x8 { return {_mm256_sub_epi32(a.v, b.v)}; }    // AVX2
    ARKXMM_API operator -(vi64x2 a, vi64x2 b) -> vi64x2 { return {_mm_sub_epi64(a.v, b.v)}; }       // SSE2
    ARKXMM_API operator -(vu64x2 a, vu64x2 b) -> vu64x2 { return {_mm_sub_epi64(a.v, b.v)}; }       // SSE2
    ARKXMM_API operator -(vi64x4 a, vi64x4 b) -> vi64x4 { return {_mm256_sub_epi64(a.v, b.v)}; }    // AVX2
    ARKXMM_API operator -(vu64x4 a, vu64x4 b) -> vu64x4 { return {_mm256_sub_epi64(a.v, b.v)}; }    // AVX2

    ARKXMM_API sub_sat(vi8x16 a, vi8x16 b) -> vi8x16 { return {_mm_subs_epi8(a.v, b.v)}; }        // SSE2
    ARKXMM_API sub_sat(vu8x16 a, vu8x16 b) -> vu8x16 { return {_mm_subs_epu8(a.v, b.v)}; }        // SSE2
    ARKXMM_API sub_sat(vi8x32 a, vi8x32 b) -> vi8x32 { return {_mm256_subs_epi8(a.v, b.v)}; }     // AVX2
    ARKXMM_API sub_sat(vu8x32 a, vu8x32 b) -> vu8x32 { return {_mm256_subs_epu8(a.v, b.v)}; }     // AVX2
    ARKXMM_API sub_sat(vi16x8 a, vi16x8 b) -> vi16x8 { return {_mm_subs_epi16(a.v, b.v)}; }       // SSE2
    ARKXMM_API sub_sat(vu16x8 a, vu16x8 b) -> vu16x8 { return {_mm_subs_epu16(a.v, b.v)}; }       // SSE2
    ARKXMM_API sub_sat(vi16x16 a, vi16x16 b) -> vi16x16 { return {_mm256_subs_epi16(a.v, b.v)}; } // AVX2
    ARKXMM_API sub_sat(vu16x16 a, vu16x16 b) -> vu16x16 { return {_mm256_subs_epu16(a.v, b.v)}; } // AVX2

    ARKXMM_API horizontal_add(vi16x8 a, vi16x8 b) -> vi16x8 { return {_mm_hadd_epi16(a.v, b.v)}; }            // SSSE3 -> [a0+a1, a2+a3, ..., b0+b1, b2+b3, ...]
    ARKXMM_API horizontal_add(vu16x8 a, vu16x8 b) -> vu16x8 { return {_mm_hadd_epi16(a.v, b.v)}; }            // SSSE3 -> [a0+a1, a2+a3, ..., b0+b1, b2+b3, ...]
    ARKXMM_API horizontal_add(vi16x16 a, vi16x16 b) -> vi16x16 { return {_mm256_hadd_epi16(a.v, b.v)}; }      // AVX2 -> [a0+a1, a2+a3, ..., b0+b1, b2+b3, ...]
    ARKXMM_API horizontal_add(vu16x16 a, vu16x16 b) -> vu16x16 { return {_mm256_hadd_epi16(a.v, b.v)}; }      // AVX2 -> [a0+a1, a2+a3, ..., b0+b1, b2+b3, ...]
    ARKXMM_API horizontal_add(vi32x4 a, vi32x4 b) -> vi32x4 { return {_mm_hadd_epi32(a.v, b.v)}; }            // SSSE3 -> [a0+a1, a2+a3, b0+b1, b2+b3]
    ARKXMM_API horizontal_add(vu32x4 a, vu32x4 b) -> vu32x4 { return {_mm_hadd_epi32(a.v, b.v)}; }            // SSSE3 -> [a0+a1, a2+a3, b0+b1, b2+b3]
    ARKXMM_API horizontal_add(vi32x8 a, vi32x8 b) -> vi32x8 { return {_mm256_hadd_epi32(a.v, b.v)}; }         // AVX2 -> [a0+a1, a2+a3, b0+b1, b2+b3]
    ARKXMM_API horizontal_add(vu32x8 a, vu32x8 b) -> vu32x8 { return {_mm256_hadd_epi32(a.v, b.v)}; }         // AVX2 -> [a0+a1, a2+a3, b0+b1, b2+b3]
    ARKXMM_API horizontal_add_sat(vu16x8 a, vu16x8 b) -> vu16x8 { return {_mm_hadds_epi16(a.v, b.v)}; }       // SSSE3 -> [a0+a1, a2+a3, ..., b0+b1, b2+b3, ...]
    ARKXMM_API horizontal_add_sat(vu16x16 a, vu16x16 b) -> vu16x16 { return {_mm256_hadds_epi16(a.v, b.v)}; } // AVX2 -> [a0+a1, a2+a3, ..., b0+b1, b2+b3, ...]

    ARKXMM_API horizontal_sub(vi16x8 a, vi16x8 b) -> vi16x8 { return {_mm_hsub_epi16(a.v, b.v)}; }            // SSSE3 -> [a0-a1, a2-a3, ..., b0-b1, b2-b3, ...]
    ARKXMM_API horizontal_sub(vu16x8 a, vu16x8 b) -> vu16x8 { return {_mm_hsub_epi16(a.v, b.v)}; }            // SSSE3 -> [a0-a1, a2-a3, ..., b0-b1, b2-b3, ...]
    ARKXMM_API horizontal_sub(vi16x16 a, vi16x16 b) -> vi16x16 { return {_mm256_hsub_epi16(a.v, b.v)}; }      // AVX2 -> [a0-a1, a2-a3, ..., b0-b1, b2-b3, ...]
    ARKXMM_API horizontal_sub(vu16x16 a, vu16x16 b) -> vu16x16 { return {_mm256_hsub_epi16(a.v, b.v)}; }      // AVX2 -> [a0-a1, a2-a3, ..., b0-b1, b2-b3, ...]
    ARKXMM_API horizontal_sub(vi32x4 a, vi32x4 b) -> vi32x4 { return {_mm_hsub_epi32(a.v, b.v)}; }            // SSSE3 -> [a0-a1, a2-a3, b0-b1, b2-b3]
    ARKXMM_API horizontal_sub(vu32x4 a, vu32x4 b) -> vu32x4 { return {_mm_hsub_epi32(a.v, b.v)}; }            // SSSE3 -> [a0-a1, a2-a3, b0-b1, b2-b3]
    ARKXMM_API horizontal_sub(vi32x8 a, vi32x8 b) -> vi32x8 { return {_mm256_hsub_epi32(a.v, b.v)}; }         // AVX2 -> [a0-a1, a2-a3, b0-b1, b2-b3]
    ARKXMM_API horizontal_sub(vu32x8 a, vu32x8 b) -> vu32x8 { return {_mm256_hsub_epi32(a.v, b.v)}; }         // AVX2 -> [a0-a1, a2-a3, b0-b1, b2-b3]
    ARKXMM_API horizontal_sub_sat(vu16x8 a, vu16x8 b) -> vu16x8 { return {_mm_hsubs_epi16(a.v, b.v)}; }       // SSSE3 -> [a0-a1, a2-a3, ..., b0-b1, b2-b3, ...]
    ARKXMM_API horizontal_sub_sat(vu16x16 a, vu16x16 b) -> vu16x16 { return {_mm256_hsubs_epi16(a.v, b.v)}; } // AVX2 -> [a0-a1, a2-a3, ..., b0-b1, b2-b3, ...]

    ARKXMM_API average(vu8x16 a, vu8x16 b) -> vu8x16 { return {_mm_avg_epu8(a.v, b.v)}; }        // SSE2
    ARKXMM_API average(vu8x32 a, vu8x32 b) -> vu8x32 { return {_mm256_avg_epu8(a.v, b.v)}; }     // AVX2
    ARKXMM_API average(vu16x8 a, vu16x8 b) -> vu16x8 { return {_mm_avg_epu16(a.v, b.v)}; }       // SSE2
    ARKXMM_API average(vu16x16 a, vu16x16 b) -> vu16x16 { return {_mm256_avg_epu16(a.v, b.v)}; } // AVX2

    ARKXMM_API operator *(vi16x8 a, vi16x8 b) -> vi16x8 { return {_mm_mullo_epi16(a.v, b.v)}; }            // SSE2
    ARKXMM_API operator *(vu16x8 a, vu16x8 b) -> vu16x8 { return {_mm_mullo_epi16(a.v, b.v)}; }            // SSE2
    ARKXMM_API operator *(vi16x16 a, vi16x16 b) -> vi16x16 { return {_mm256_mullo_epi16(a.v, b.v)}; }      // AVX2
    ARKXMM_API operator *(vu16x16 a, vu16x16 b) -> vu16x16 { return {_mm256_mullo_epi16(a.v, b.v)}; }      // AVX2
    ARKXMM_API operator *(vi32x4 a, vi32x4 b) -> vi32x4 { return {_mm_mullo_epi32(a.v, b.v)}; }            // SSE4.1
    ARKXMM_API operator *(vu32x4 a, vu32x4 b) -> vu32x4 { return {_mm_mullo_epi32(a.v, b.v)}; }            // SSE4.1
    ARKXMM_API operator *(vi32x8 a, vi32x8 b) -> vi32x8 { return {_mm256_mullo_epi32(a.v, b.v)}; }         // AVX2
    ARKXMM_API operator *(vu32x8 a, vu32x8 b) -> vu32x8 { return {_mm256_mullo_epi32(a.v, b.v)}; }         // AVX2
    /* ARKXMM_API operator *(vi64x2 a, vi64x2 b) -> vi64x2 { return { _mm_mullo_epi64(a.v, b.v) }; } */    // AVX512VL + AVX512DQ
    /* ARKXMM_API operator *(vu64x2 a, vu64x2 b) -> vu64x2 { return { _mm_mullo_epi64(a.v, b.v) }; } */    // AVX512VL + AVX512DQ
    /* ARKXMM_API operator *(vi64x4 a, vi64x4 b) -> vi64x4 { return { _mm256_mullo_epi64(a.v, b.v) }; } */ // AVX512VL + AVX512DQ
    /* ARKXMM_API operator *(vu64x4 a, vu64x4 b) -> vu64x4 { return { _mm256_mullo_epi64(a.v, b.v) }; } */ // AVX512VL + AVX512DQ

    ARKXMM_API mul_lo(vi16x8 a, vi16x8 b) -> vi16x8 { return {_mm_mullo_epi16(a.v, b.v)}; }         // SSE2
    ARKXMM_API mul_lo(vu16x8 a, vu16x8 b) -> vu16x8 { return {_mm_mullo_epi16(a.v, b.v)}; }         // SSE2
    ARKXMM_API mul_lo(vi16x16 a, vi16x16 b) -> vi16x16 { return {_mm256_mullo_epi16(a.v, b.v)}; }   // AVX2
    ARKXMM_API mul_lo(vu16x16 a, vu16x16 b) -> vu16x16 { return {_mm256_mullo_epi16(a.v, b.v)}; }   // AVX2
    ARKXMM_API mul_hi(vi16x8 a, vi16x8 b) -> vi16x8 { return {_mm_mulhi_epi16(a.v, b.v)}; }         // SSE2
    ARKXMM_API mul_hi(vu16x8 a, vu16x8 b) -> vu16x8 { return {_mm_mulhi_epu16(a.v, b.v)}; }         // SSE2
    ARKXMM_API mul_hi(vi16x16 a, vi16x16 b) -> vi16x16 { return {_mm256_mulhi_epi16(a.v, b.v)}; }   // AVX2
    ARKXMM_API mul_hi(vu16x16 a, vu16x16 b) -> vu16x16 { return {_mm256_mulhi_epu16(a.v, b.v)}; }   // AVX2 
    ARKXMM_API mul_hrs(vi16x8 a, vi16x8 b) -> vi16x8 { return {_mm_mulhrs_epi16(a.v, b.v)}; }       // SSSE 3 - with scale [-32768..32767]*[-32768..32767] -> [-32768..32767]
    ARKXMM_API mul_hrs(vi16x16 a, vi16x16 b) -> vi16x16 { return {_mm256_mulhrs_epi16(a.v, b.v)}; } // AVX2 - with scale [-32768..32767]*[-32768..32767] -> [-32768..32767]
    ARKXMM_API mul32x32to64(vi32x4 a, vi32x4 b) -> vi64x2 { return {_mm_mul_epi32(a.v, b.v)}; }     // SSE4.1 -> [a0*b0, a2*b2]
    ARKXMM_API mul32x32to64(vu32x4 a, vu32x4 b) -> vu64x2 { return {_mm_mul_epu32(a.v, b.v)}; }     // SSE2 -> [a0*b0, a2*b2]
    ARKXMM_API mul32x32to64(vi32x8 a, vi32x8 b) -> vi64x4 { return {_mm256_mul_epi32(a.v, b.v)}; }  // AVX2 -> [a0*b0, a2*b2, a4*b4, a6*b6]
    ARKXMM_API mul32x32to64(vu32x8 a, vu32x8 b) -> vu64x4 { return {_mm256_mul_epu32(a.v, b.v)}; }  // AVX2 -> [a0*b0, a2*b2, a4*b4, a6*b6]

    ARKXMM_API operator <<(vi16x8 a, int i) -> vi16x8 { return {_mm_slli_epi16(a.v, i)}; }           // SSE2
    ARKXMM_API operator <<(vu16x8 a, int i) -> vu16x8 { return {_mm_slli_epi16(a.v, i)}; }           // SSE2
    ARKXMM_API operator <<(vi16x16 a, int i) -> vi16x16 { return {_mm256_slli_epi16(a.v, i)}; }      // AVX2
    ARKXMM_API operator <<(vu16x16 a, int i) -> vu16x16 { return {_mm256_slli_epi16(a.v, i)}; }      // AVX2
    ARKXMM_API operator <<(vi32x4 a, int i) -> vi32x4 { return {_mm_slli_epi32(a.v, i)}; }           // SSE2
    ARKXMM_API operator <<(vu32x4 a, int i) -> vu32x4 { return {_mm_slli_epi32(a.v, i)}; }           // SSE2
    ARKXMM_API operator <<(vi32x8 a, int i) -> vi32x8 { return {_mm256_slli_epi32(a.v, i)}; }        // AVX2
    ARKXMM_API operator <<(vu32x8 a, int i) -> vu32x8 { return {_mm256_slli_epi32(a.v, i)}; }        // AVX2
    /*ARKXMM_API operator <<(vi64x2 a, int i) -> vi64x2 { return {_mm_slli_epi64(a.v, i)}; } */      // SSE2
    ARKXMM_API operator <<(vu64x2 a, int i) -> vu64x2 { return {_mm_slli_epi64(a.v, i)}; }           // SSE2
    /* ARKXMM_API operator <<(vi64x4 a, int i) -> vi64x4 { return {_mm256_slli_epi64(a.v, i)}; }  */ // AVX2
    ARKXMM_API operator <<(vu64x4 a, int i) -> vu64x4 { return {_mm256_slli_epi64(a.v, i)}; }        // AVX2

    ARKXMM_API operator >>(vi16x8 a, int i) -> vi16x8 { return {_mm_srai_epi16(a.v, i)}; }            // SSE2
    ARKXMM_API operator >>(vu16x8 a, int i) -> vu16x8 { return {_mm_srli_epi16(a.v, i)}; }            // SSE2
    ARKXMM_API operator >>(vi16x16 a, int i) -> vi16x16 { return {_mm256_srai_epi16(a.v, i)}; }       // AVX2
    ARKXMM_API operator >>(vu16x16 a, int i) -> vu16x16 { return {_mm256_srli_epi16(a.v, i)}; }       // AVX2
    ARKXMM_API operator >>(vi32x4 a, int i) -> vi32x4 { return {_mm_srai_epi32(a.v, i)}; }            // SSE2
    ARKXMM_API operator >>(vu32x4 a, int i) -> vu32x4 { return {_mm_srli_epi32(a.v, i)}; }            // SSE2
    ARKXMM_API operator >>(vi32x8 a, int i) -> vi32x8 { return {_mm256_srai_epi32(a.v, i)}; }         // AVX2
    ARKXMM_API operator >>(vu32x8 a, int i) -> vu32x8 { return {_mm256_srli_epi32(a.v, i)}; }         // AVX2
    /* ARKXMM_API operator >>(vi64x2 a, int i) -> vi64x2 { return { _mm_srai_epi64(a.v, i) }; } */    // AVX512VL + AVX512F
    ARKXMM_API operator >>(vu64x2 a, int i) -> vu64x2 { return {_mm_srli_epi64(a.v, i)}; }            // SSE2
    /* ARKXMM_API operator >>(vi64x4 a, int i) -> vi64x4 { return { _mm256_srai_epi64(a.v, i) }; } */ // AVX512VL + AVX512F
    ARKXMM_API operator >>(vu64x4 a, int i) -> vu64x4 { return {_mm256_srli_epi64(a.v, i)}; }         // AVX2

    ARKXMM_API operator <<(vi16x8 a, SHIFT i) -> vi16x8 { return {_mm_sll_epi16(a.v, i)}; }            // SSE2
    ARKXMM_API operator <<(vu16x8 a, SHIFT i) -> vu16x8 { return {_mm_sll_epi16(a.v, i)}; }            // SSE2
    ARKXMM_API operator <<(vi16x16 a, SHIFT i) -> vi16x16 { return {_mm256_sll_epi16(a.v, i)}; }       // AVX2
    ARKXMM_API operator <<(vu16x16 a, SHIFT i) -> vu16x16 { return {_mm256_sll_epi16(a.v, i)}; }       // AVX2
    ARKXMM_API operator <<(vi32x4 a, SHIFT i) -> vi32x4 { return {_mm_sll_epi32(a.v, i)}; }            // SSE2
    ARKXMM_API operator <<(vu32x4 a, SHIFT i) -> vu32x4 { return {_mm_sll_epi32(a.v, i)}; }            // SSE2
    ARKXMM_API operator <<(vi32x8 a, SHIFT i) -> vi32x8 { return {_mm256_sll_epi32(a.v, i)}; }         // AVX2
    ARKXMM_API operator <<(vu32x8 a, SHIFT i) -> vu32x8 { return {_mm256_sll_epi32(a.v, i)}; }         // AVX2
    /* ARKXMM_API operator <<(vi64x2 a, SHIFT i) -> vi64x2 { return { _mm_sll_epi64(a.v, i) }; } */    // SSE2
    ARKXMM_API operator <<(vu64x2 a, SHIFT i) -> vu64x2 { return {_mm_sll_epi64(a.v, i)}; }            // SSE2
    /* ARKXMM_API operator <<(vi64x4 a, SHIFT i) -> vi64x4 { return { _mm256_sll_epi64(a.v, i) }; } */ // AVX2
    ARKXMM_API operator <<(vu64x4 a, SHIFT i) -> vu64x4 { return {_mm256_sll_epi64(a.v, i)}; }         // AVX2

    ARKXMM_API operator >>(vi16x8 a, SHIFT i) -> vi16x8 { return {_mm_sra_epi16(a.v, i)}; }            // SSE2
    ARKXMM_API operator >>(vu16x8 a, SHIFT i) -> vu16x8 { return {_mm_srl_epi16(a.v, i)}; }            // SSE2
    ARKXMM_API operator >>(vi16x16 a, SHIFT i) -> vi16x16 { return {_mm256_sra_epi16(a.v, i)}; }       // AVX2
    ARKXMM_API operator >>(vu16x16 a, SHIFT i) -> vu16x16 { return {_mm256_srl_epi16(a.v, i)}; }       // AVX2
    ARKXMM_API operator >>(vi32x4 a, SHIFT i) -> vi32x4 { return {_mm_sra_epi32(a.v, i)}; }            // SSE2
    ARKXMM_API operator >>(vu32x4 a, SHIFT i) -> vu32x4 { return {_mm_srl_epi32(a.v, i)}; }            // SSE2
    ARKXMM_API operator >>(vi32x8 a, SHIFT i) -> vi32x8 { return {_mm256_sra_epi32(a.v, i)}; }         // AVX2
    ARKXMM_API operator >>(vu32x8 a, SHIFT i) -> vu32x8 { return {_mm256_srl_epi32(a.v, i)}; }         // AVX2
    /* ARKXMM_API operator >>(vi64x2 a, SHIFT i) -> vi64x2 { return { _mm_sra_epi64(a.v, i) }; } */    // AVX512VL + AVX512F
    ARKXMM_API operator >>(vu64x2 a, SHIFT i) -> vu64x2 { return {_mm_srl_epi64(a.v, i)}; }            // SSE2
    /* ARKXMM_API operator >>(vi64x4 a, SHIFT i) -> vi64x4 { return { _mm256_sra_epi64(a.v, i) }; } */ // AVX512VL + AVX512F
    ARKXMM_API operator >>(vu64x4 a, SHIFT i) -> vu64x4 { return {_mm256_srl_epi64(a.v, i)}; }         // AVX2

    /* ARKXMM_API operator <<(vi16x8 a, vi16x8 i) -> vi16x8 { return { _mm_sllv_epi16(a.v, i.v) }; } */       // AVX512VL + AVX512BW
    /* ARKXMM_API operator <<(vu16x8 a, vi16x8 i) -> vu16x8 { return { _mm_sllv_epi16(a.v, i.v) }; } */       // AVX512VL + AVX512BW
    /* ARKXMM_API operator <<(vi16x16 a, vi16x16 i) -> vi16x16 { return { _mm256_sllv_epi16(a.v, i.v) }; } */ // AVX512VL + AVX512BW
    /* ARKXMM_API operator <<(vu16x16 a, vi16x16 i) -> vu16x16 { return { _mm256_sllv_epi16(a.v, i.v) }; } */ // AVX512VL + AVX512BW
    ARKXMM_API operator <<(vi32x4 a, vi32x4 i) -> vi32x4 { return {_mm_sllv_epi32(a.v, i.v)}; }               // AVX2
    ARKXMM_API operator <<(vu32x4 a, vi32x4 i) -> vu32x4 { return {_mm_sllv_epi32(a.v, i.v)}; }               // AVX2
    ARKXMM_API operator <<(vi32x8 a, vi32x8 i) -> vi32x8 { return {_mm256_sllv_epi32(a.v, i.v)}; }            // AVX2
    ARKXMM_API operator <<(vu32x8 a, vi32x8 i) -> vu32x8 { return {_mm256_sllv_epi32(a.v, i.v)}; }            // AVX2
    /* ARKXMM_API operator <<(vi64x2 a, vi64x2 i) -> vi64x2 { return { _mm_sllv_epi64(a.v, i.v) }; } */       // AVX2
    ARKXMM_API operator <<(vu64x2 a, vi64x2 i) -> vu64x2 { return {_mm_sllv_epi64(a.v, i.v)}; }               // AVX2
    /* ARKXMM_API operator <<(vi64x4 a, vi64x4 i) -> vi64x4 { return { _mm256_sllv_epi64(a.v, i.v) }; } */    // AVX2
    ARKXMM_API operator <<(vu64x4 a, vi64x4 i) -> vu64x4 { return {_mm256_sllv_epi64(a.v, i.v)}; }            // AVX2

    /* ARKXMM_API operator >>(vi16x8 a, vi16x8 i) -> vi16x8 { return { _mm_srav_epi16(a.v, i.v) }; } */       // AVX512VL + AVX512BW
    /* ARKXMM_API operator >>(vu16x8 a, vi16x8 i) -> vu16x8 { return { _mm_srlv_epi16(a.v, i.v) }; } */       // AVX512VL + AVX512BW
    /* ARKXMM_API operator >>(vi16x16 a, vi16x16 i) -> vi16x16 { return { _mm256_srav_epi16(a.v, i.v) }; } */ // AVX512VL + AVX512BW
    /* ARKXMM_API operator >>(vu16x16 a, vi16x16 i) -> vu16x16 { return { _mm256_srlv_epi16(a.v, i.v) }; } */ // AVX512VL + AVX512BW
    ARKXMM_API operator >>(vi32x4 a, vi32x4 i) -> vi32x4 { return {_mm_srav_epi32(a.v, i.v)}; }               // AVX2
    ARKXMM_API operator >>(vu32x4 a, vi32x4 i) -> vu32x4 { return {_mm_srlv_epi32(a.v, i.v)}; }               // AVX2
    ARKXMM_API operator >>(vi32x8 a, vi32x8 i) -> vi32x8 { return {_mm256_srav_epi32(a.v, i.v)}; }            // AVX2
    ARKXMM_API operator >>(vu32x8 a, vi32x8 i) -> vu32x8 { return {_mm256_srlv_epi32(a.v, i.v)}; }            // AVX2
    /* ARKXMM_API operator >>(vi64x2 a, vi64x2 i) -> vi64x2 { return { _mm_srav_epi64(a.v, i.v) }; } */       // AVX512VL + AVX512F
    ARKXMM_API operator >>(vu64x2 a, vi64x2 i) -> vu64x2 { return {_mm_srlv_epi64(a.v, i.v)}; }               // AVX2
    /* ARKXMM_API operator >>(vi64x4 a, vi64x4 i) -> vi64x4 { return { _mm256_srav_epi64(a.v, i.v) }; } */    // AVX512VL + AVX512F
    ARKXMM_API operator >>(vu64x4 a, vi64x4 i) -> vu64x4 { return {_mm256_srlv_epi64(a.v, i.v)}; }            // AVX2

    ARKXMM_API max(vi8x16 a, vi8x16 b) -> vi8x16 { return {_mm_max_epi8(a.v, b.v)}; }                  // SSE4.1
    ARKXMM_API max(vu8x16 a, vu8x16 b) -> vu8x16 { return {_mm_max_epu8(a.v, b.v)}; }                  // SSE2
    ARKXMM_API max(vi8x32 a, vi8x32 b) -> vi8x32 { return {_mm256_max_epi8(a.v, b.v)}; }               // AVX2
    ARKXMM_API max(vu8x32 a, vu8x32 b) -> vu8x32 { return {_mm256_max_epu8(a.v, b.v)}; }               // AVX2
    ARKXMM_API max(vi16x8 a, vi16x8 b) -> vi16x8 { return {_mm_max_epi16(a.v, b.v)}; }                 // SSE2
    ARKXMM_API max(vu16x8 a, vu16x8 b) -> vu16x8 { return {_mm_max_epu16(a.v, b.v)}; }                 // SSE4.1
    ARKXMM_API max(vi16x16 a, vi16x16 b) -> vi16x16 { return {_mm256_max_epi16(a.v, b.v)}; }           // AVX2
    ARKXMM_API max(vu16x16 a, vu16x16 b) -> vu16x16 { return {_mm256_max_epu16(a.v, b.v)}; }           // AVX2
    ARKXMM_API max(vi32x4 a, vi32x4 b) -> vi32x4 { return {_mm_max_epi32(a.v, b.v)}; }                 // SSE4.1
    ARKXMM_API max(vu32x4 a, vu32x4 b) -> vu32x4 { return {_mm_max_epu32(a.v, b.v)}; }                 // SSE4.1
    ARKXMM_API max(vi32x8 a, vi32x8 b) -> vi32x8 { return {_mm256_max_epi32(a.v, b.v)}; }              // AVX2
    ARKXMM_API max(vu32x8 a, vu32x8 b) -> vu32x8 { return {_mm256_max_epu32(a.v, b.v)}; }              // AVX2
    /* ARKXMM_API max(vi64x2 a, vi64x2 b) -> vi64x2 { return { _mm_max_epi64(a.v, b.v) }; }         */ // AVX512F + AVX512VL
    /* ARKXMM_API max(vu64x2 a, vu64x2 b) -> vu64x2 { return { _mm_max_epu64(a.v, b.v) }; }         */ // AVX512F + AVX512VL
    /* ARKXMM_API max(vi64x4 a, vi64x4 b) -> vi64x4 { return { _mm256_max_epi64(a.v, b.v) }; }      */ // AVX512F + AVX512VL
    /* ARKXMM_API max(vu64x4 a, vu64x4 b) -> vu64x4 { return { _mm256_max_epu64(a.v, b.v) }; }      */ // AVX512F + AVX512VL

    ARKXMM_API min(vi8x16 a, vi8x16 b) -> vi8x16 { return {_mm_min_epi8(a.v, b.v)}; }                  // SSE4.1
    ARKXMM_API min(vu8x16 a, vu8x16 b) -> vu8x16 { return {_mm_min_epu8(a.v, b.v)}; }                  // SSE2
    ARKXMM_API min(vi8x32 a, vi8x32 b) -> vi8x32 { return {_mm256_min_epi8(a.v, b.v)}; }               // AVX2
    ARKXMM_API min(vu8x32 a, vu8x32 b) -> vu8x32 { return {_mm256_min_epu8(a.v, b.v)}; }               // AVX2
    ARKXMM_API min(vi16x8 a, vi16x8 b) -> vi16x8 { return {_mm_min_epi16(a.v, b.v)}; }                 // SSE2
    ARKXMM_API min(vu16x8 a, vu16x8 b) -> vu16x8 { return {_mm_min_epu16(a.v, b.v)}; }                 // SSE4.1
    ARKXMM_API min(vi16x16 a, vi16x16 b) -> vi16x16 { return {_mm256_min_epi16(a.v, b.v)}; }           // AVX2
    ARKXMM_API min(vu16x16 a, vu16x16 b) -> vu16x16 { return {_mm256_min_epu16(a.v, b.v)}; }           // AVX2
    ARKXMM_API min(vi32x4 a, vi32x4 b) -> vi32x4 { return {_mm_min_epi32(a.v, b.v)}; }                 // SSE4.1
    ARKXMM_API min(vu32x4 a, vu32x4 b) -> vu32x4 { return {_mm_min_epu32(a.v, b.v)}; }                 // SSE4.1
    ARKXMM_API min(vi32x8 a, vi32x8 b) -> vi32x8 { return {_mm256_min_epi32(a.v, b.v)}; }              // AVX2
    ARKXMM_API min(vu32x8 a, vu32x8 b) -> vu32x8 { return {_mm256_min_epu32(a.v, b.v)}; }              // AVX2
    /* ARKXMM_API min(vi64x2 a, vi64x2 b) -> vi64x2 { return { _mm_min_epi64(a.v, b.v) }; }         */ // AVX512F + AVX512VL
    /* ARKXMM_API min(vu64x2 a, vu64x2 b) -> vu64x2 { return { _mm_min_epu64(a.v, b.v) }; }         */ // AVX512F + AVX512VL
    /* ARKXMM_API min(vi64x4 a, vi64x4 b) -> vi64x4 { return { _mm256_min_epi64(a.v, b.v) }; }      */ // AVX512F + AVX512VL
    /* ARKXMM_API min(vu64x4 a, vu64x4 b) -> vu64x4 { return { _mm256_min_epu64(a.v, b.v) }; }      */ // AVX512F + AVX512VL

    ARKXMM_API operator ==(vu8x16 a, vu8x16 b) -> vu8x16 { return {_mm_cmpeq_epi8(a.v, b.v)}; }        // SSE2
    ARKXMM_API operator ==(vi8x16 a, vi8x16 b) -> vi8x16 { return {_mm_cmpeq_epi8(a.v, b.v)}; }        // SSE2
    ARKXMM_API operator ==(vu8x32 a, vu8x32 b) -> vu8x32 { return {_mm256_cmpeq_epi8(a.v, b.v)}; }     // AVX2
    ARKXMM_API operator ==(vi8x32 a, vi8x32 b) -> vi8x32 { return {_mm256_cmpeq_epi8(a.v, b.v)}; }     // AVX2
    ARKXMM_API operator ==(vu16x8 a, vu16x8 b) -> vu16x8 { return {_mm_cmpeq_epi16(a.v, b.v)}; }       // SSE2
    ARKXMM_API operator ==(vi16x8 a, vi16x8 b) -> vi16x8 { return {_mm_cmpeq_epi16(a.v, b.v)}; }       // SSE2
    ARKXMM_API operator ==(vu16x16 a, vu16x16 b) -> vu16x16 { return {_mm256_cmpeq_epi16(a.v, b.v)}; } // AVX2
    ARKXMM_API operator ==(vi16x16 a, vi16x16 b) -> vi16x16 { return {_mm256_cmpeq_epi16(a.v, b.v)}; } // AVX2
    ARKXMM_API operator ==(vu32x4 a, vu32x4 b) -> vu32x4 { return {_mm_cmpeq_epi32(a.v, b.v)}; }       // SSE2
    ARKXMM_API operator ==(vi32x4 a, vi32x4 b) -> vi32x4 { return {_mm_cmpeq_epi32(a.v, b.v)}; }       // SSE2
    ARKXMM_API operator ==(vu32x8 a, vu32x8 b) -> vu32x8 { return {_mm256_cmpeq_epi32(a.v, b.v)}; }    // AVX2
    ARKXMM_API operator ==(vi32x8 a, vi32x8 b) -> vi32x8 { return {_mm256_cmpeq_epi32(a.v, b.v)}; }    // AVX2
    ARKXMM_API operator ==(vu64x2 a, vu64x2 b) -> vu64x2 { return {_mm_cmpeq_epi64(a.v, b.v)}; }       // SSE4.1
    ARKXMM_API operator ==(vi64x2 a, vi64x2 b) -> vi64x2 { return {_mm_cmpeq_epi64(a.v, b.v)}; }       // SSE4.1
    ARKXMM_API operator ==(vu64x4 a, vu64x4 b) -> vu64x4 { return {_mm256_cmpeq_epi64(a.v, b.v)}; }    // SSE4.1
    ARKXMM_API operator ==(vi64x4 a, vi64x4 b) -> vi64x4 { return {_mm256_cmpeq_epi64(a.v, b.v)}; }    // SSE4.1

    ARKXMM_API operator <(vi8x16 a, vi8x16 b) -> vi8x16 { return {_mm_cmplt_epi8(a.v, b.v)}; }        // SSE2
    ARKXMM_API operator <(vi8x32 a, vi8x32 b) -> vi8x32 { return {_mm256_cmpgt_epi8(b.v, a.v)}; }     // AVX2
    ARKXMM_API operator <(vi16x8 a, vi16x8 b) -> vi16x8 { return {_mm_cmplt_epi16(a.v, b.v)}; }       // SSE2
    ARKXMM_API operator <(vi16x16 a, vi16x16 b) -> vi16x16 { return {_mm256_cmpgt_epi16(b.v, a.v)}; } // AVX2
    ARKXMM_API operator <(vi32x4 a, vi32x4 b) -> vi32x4 { return {_mm_cmplt_epi32(a.v, b.v)}; }       // SSE2
    ARKXMM_API operator <(vi32x8 a, vi32x8 b) -> vi32x8 { return {_mm256_cmpgt_epi32(b.v, a.v)}; }    // AVX2
    ARKXMM_API operator <(vi64x2 a, vi64x2 b) -> vi64x2 { return {_mm_cmpgt_epi64(b.v, a.v)}; }       // SSE4.2
    ARKXMM_API operator <(vi64x4 a, vi64x4 b) -> vi64x4 { return {_mm256_cmpgt_epi64(b.v, a.v)}; }    // AVX2

    ARKXMM_API operator >(vi8x16 a, vi8x16 b) -> vi8x16 { return {_mm_cmpgt_epi8(a.v, b.v)}; }        // SSE2
    ARKXMM_API operator >(vi8x32 a, vi8x32 b) -> vi8x32 { return {_mm256_cmpgt_epi8(a.v, b.v)}; }     // AVX2
    ARKXMM_API operator >(vi16x8 a, vi16x8 b) -> vi16x8 { return {_mm_cmpgt_epi16(a.v, b.v)}; }       // SSE2
    ARKXMM_API operator >(vi16x16 a, vi16x16 b) -> vi16x16 { return {_mm256_cmpgt_epi16(a.v, b.v)}; } // AVX2
    ARKXMM_API operator >(vi32x4 a, vi32x4 b) -> vi32x4 { return {_mm_cmpgt_epi32(a.v, b.v)}; }       // SSE2
    ARKXMM_API operator >(vi32x8 a, vi32x8 b) -> vi32x8 { return {_mm256_cmpgt_epi32(a.v, b.v)}; }    // AVX2
    ARKXMM_API operator >(vi64x2 a, vi64x2 b) -> vi64x2 { return {_mm_cmpgt_epi64(a.v, b.v)}; }       // SSE4.2
    ARKXMM_API operator >(vi64x4 a, vi64x4 b) -> vi64x4 { return {_mm256_cmpgt_epi64(a.v, b.v)}; }    // SSE4.2

    // pack 2 vector {aaa...a}, {bbb...b} to {aaa..abbb..b}
    ARKXMM_API pack_sat_i(vi16x8 a, vi16x8 b) -> vi8x16 { return {_mm_packs_epi16(a.v, b.v)}; }       // SSE2 - clamp to [-128..127]
    ARKXMM_API pack_sat_i(vi16x16 a, vi16x16 b) -> vi8x32 { return {_mm256_packs_epi16(a.v, b.v)}; }  // AVX2 - clamp to [-128..127]
    ARKXMM_API pack_sat_i(vi32x4 a, vi32x4 b) -> vi16x8 { return {_mm_packs_epi32(a.v, b.v)}; }       // SSE2 - clamp to [-32768..32767]
    ARKXMM_API pack_sat_i(vi32x8 a, vi32x8 b) -> vi16x16 { return {_mm256_packs_epi32(a.v, b.v)}; }   // AVX2 - clamp to [-32768..32767]
    ARKXMM_API pack_sat_u(vi16x8 a, vi16x8 b) -> vu8x16 { return {_mm_packus_epi16(a.v, b.v)}; }      // SSE2 - clamp to [0..255]
    ARKXMM_API pack_sat_u(vi16x16 a, vi16x16 b) -> vu8x32 { return {_mm256_packus_epi16(a.v, b.v)}; } // AVX2 - clamp to [0..255]
    ARKXMM_API pack_sat_u(vi32x4 a, vi32x4 b) -> vu16x8 { return {_mm_packus_epi32(a.v, b.v)}; }      // SSE4.1 - clamp to [0..65535]
    ARKXMM_API pack_sat_u(vi32x8 a, vi32x8 b) -> vu16x16 { return {_mm256_packus_epi32(a.v, b.v)}; }  // AVX2 - clamp to [0..65535]

    // unpack 2 vector {lll...lLLL..L}, {hhh..hHHH..H} to {lhlhlh...lh}
    template <class XMM> ARKXMM_API unpack_lo(XMM l, XMM h = {}) -> enable::if_8x16<XMM> { return {_mm_unpacklo_epi8(l.v, h.v)}; }      // SSE2 {l0...l15}, {h0...h15} -> {l0,h0,...,l7,h7}
    template <class YMM> ARKXMM_API unpack_lo(YMM l, YMM h = {}) -> enable::if_8x32<YMM> { return {_mm256_unpacklo_epi8(l.v, h.v)}; }   // AVX2 {l0...l15|l16...l31}, {h0...h15|h16...h31} -> {l0,h0,...,l7,h7 | l16,h16,...,l23,h23}
    template <class XMM> ARKXMM_API unpack_lo(XMM l, XMM h = {}) -> enable::if_16x8<XMM> { return {_mm_unpacklo_epi16(l.v, h.v)}; }     // SSE2 {l0... l7}, {h0... h7} -> {l0,h0,...,l3,h3}
    template <class YMM> ARKXMM_API unpack_lo(YMM l, YMM h = {}) -> enable::if_16x16<YMM> { return {_mm256_unpacklo_epi16(l.v, h.v)}; } // AVX2 {l0... l7| l8...l15}, {h0... h7| h8...h15} -> {l0,h0,...,l3,h3 | l8,h8,...,l11,h11}
    template <class XMM> ARKXMM_API unpack_lo(XMM l, XMM h = {}) -> enable::if_32x4<XMM> { return {_mm_unpacklo_epi32(l.v, h.v)}; }     // SSE2 {l0... l3}, {h0... h3} -> {l0,h0,l1,h1}
    template <class YMM> ARKXMM_API unpack_lo(YMM l, YMM h = {}) -> enable::if_32x8<YMM> { return {_mm256_unpacklo_epi32(l.v, h.v)}; }  // AVX2 {l0... l3| l4... l7}, {h0... h3| h4... h7} -> {l0,h0,l1,h1 | l4,h4,l5,h5}
    template <class XMM> ARKXMM_API unpack_lo(XMM l, XMM h = {}) -> enable::if_64x2<XMM> { return {_mm_unpacklo_epi64(l.v, h.v)}; }     // SSE2 {l0... l1}, {h0... h1} -> {l0,h0}
    template <class YMM> ARKXMM_API unpack_lo(YMM l, YMM h = {}) -> enable::if_64x4<YMM> { return {_mm256_unpacklo_epi64(l.v, h.v)}; }  // AVX2 {l0... l1| l2... l3}, {h0... h1| h2... h3} -> {l0,h0|l2,h2}
    template <class XMM> ARKXMM_API unpack_hi(XMM l, XMM h = {}) -> enable::if_8x16<XMM> { return {_mm_unpackhi_epi8(l.v, h.v)}; }      // SSE2 {l0...l15}, {h0...h15} -> {l8,h8,...,l15,h15}
    template <class YMM> ARKXMM_API unpack_hi(YMM l, YMM h = {}) -> enable::if_8x32<YMM> { return {_mm256_unpackhi_epi8(l.v, h.v)}; }   // AVX2 {l0...l15|l16...l31}, {h0...h15|h16...h31} -> {l7,h7,...,l15,h15 | l24,h24,...,l31,h31}
    template <class XMM> ARKXMM_API unpack_hi(XMM l, XMM h = {}) -> enable::if_16x8<XMM> { return {_mm_unpackhi_epi16(l.v, h.v)}; }     // SSE2 {l0... l7}, {h0... h7} -> {l4,h4,...,l7,h7}
    template <class YMM> ARKXMM_API unpack_hi(YMM l, YMM h = {}) -> enable::if_16x16<YMM> { return {_mm256_unpackhi_epi16(l.v, h.v)}; } // AVX2 {l0... l7| l8...l15}, {h0... h7| h8...h15} -> {l4,h4,...,l7,h7 | l12,h12,...,l15,h15}
    template <class XMM> ARKXMM_API unpack_hi(XMM l, XMM h = {}) -> enable::if_32x4<XMM> { return {_mm_unpackhi_epi32(l.v, h.v)}; }     // SSE2 {l0... l3}, {h0... h3} -> {l2,h2,l3,h3}
    template <class YMM> ARKXMM_API unpack_hi(YMM l, YMM h = {}) -> enable::if_32x8<YMM> { return {_mm256_unpackhi_epi32(l.v, h.v)}; }  // AVX2 {l0... l3| l4... l7}, {h0... h3| h4... h7} -> {l2,h2,l3,h3 | l6,h6,l7,h7}
    template <class XMM> ARKXMM_API unpack_hi(XMM l, XMM h = {}) -> enable::if_64x2<XMM> { return {_mm_unpackhi_epi64(l.v, h.v)}; }     // SSE2 {l0... l1}, {h0... h1} -> {l1,h1}
    template <class YMM> ARKXMM_API unpack_hi(YMM l, YMM h = {}) -> enable::if_64x4<YMM> { return {_mm256_unpackhi_epi64(l.v, h.v)}; }  // AVX2 {l0... l1| l2... l3}, {h0... h1| h2... h3} -> {l1,h1 | l3,h3}

    // avx2 permute
    template <class YMM> ARKXMM_API permute32(YMM v, vi32x8 idx) -> enable::if_32x8<YMM> { return {_mm256_permutevar8x32_epi32(v.v, idx.v)}; }                                                                                                             // AVX2  idx = 0..7
    template <uint8_t i0, uint8_t i1, uint8_t i2, uint8_t i3, uint8_t i4, uint8_t i5, uint8_t i6, uint8_t i7, class YMM> ARKXMM_API permute32(YMM v) -> enable::if_32x8<YMM> { return permute32(v, from_values<vi32x8>(i0, i1, i2, i3, i4, i5, i6, i7)); } // AVX2  idx = 0..7
    template <uint8_t i0, uint8_t i1, uint8_t i2, uint8_t i3, class YMM> ARKXMM_API permute64(YMM v) -> enable::if_64x4<YMM> { return {_mm256_permute4x64_epi64(v.v, (i0 & 3) | (i1 & 3) << 2 | (i2 & 3) << 4 | (i3 & 3) << 6)}; }                         // AVX2  idx = 0..3
    template <int8_t i0, int8_t i1, class YMM> ARKXMM_API permute128(YMM a, YMM b) -> enable::if_64x4<YMM> { return {_mm256_permute2x128_si256(a.v, b.v, (i0 & 15) | (i1 & 15) << 4)}; }                                                                   // AVX2  idx = 0..3 or -1

    // avx2 gather
    template <class XMM> ARKXMM_API gather(const typename XMM::elem_t* table, vu32x4 idx) -> enable::if_32x4<XMM> { return {_mm_i32gather_epi32(reinterpret_cast<const int32_t*>(table), idx.v, 4)}; }    // returns 4 elements idx{i,j,k,l}->{xi,xj,xk,xl}
    template <class XMM> ARKXMM_API gather(const typename XMM::elem_t* table, vu64x2 idx) -> enable::if_32x4<XMM> { return {_mm_i64gather_epi32(reinterpret_cast<const int32_t*>(table), idx.v, 4)}; }    // returns 2 elements idx{i,j} -> {xi,xj,0,0}
    template <class XMM> ARKXMM_API gather(const typename XMM::elem_t* table, vu32x4 idx) -> enable::if_64x2<XMM> { return {_mm_i32gather_epi64(reinterpret_cast<const int64_t*>(table), idx.v, 8)}; }    // returns 2 elements idx{i,j,_,_} -> {xi,xj}
    template <class XMM> ARKXMM_API gather(const typename XMM::elem_t* table, vu64x2 idx) -> enable::if_64x2<XMM> { return {_mm_i64gather_epi64(reinterpret_cast<const int64_t*>(table), idx.v, 8)}; }    // returns 2 elements idx{i,j} -> {xi,xj}
    template <class YMM> ARKXMM_API gather(const typename YMM::elem_t* table, vu32x8 idx) -> enable::if_32x8<YMM> { return {_mm256_i32gather_epi32(reinterpret_cast<const int32_t*>(table), idx.v, 4)}; } // returns 8 elements idx{i,j,k,l,m,n,o,p} -> {xi,xj,xk,xl,xm,xn,xo,xp}
    template <class YMM> ARKXMM_API gather(const typename YMM::elem_t* table, vu64x4 idx) -> enable::if_32x8<YMM> { return {_mm256_i64gather_epi32(reinterpret_cast<const int32_t*>(table), idx.v, 4)}; } // returns 4 elements idx{i,j,k,l} -> {xi,xj,xk,xl,0,0,0,0}
    template <class YMM> ARKXMM_API gather(const typename YMM::elem_t* table, vu32x4 idx) -> enable::if_64x4<YMM> { return {_mm256_i32gather_epi64(reinterpret_cast<const int64_t*>(table), idx.v, 8)}; } // returns 4 elements idx{i,j,k,l,_,_,_,_} -> {xi,xj,xk,xl}
    template <class YMM> ARKXMM_API gather(const typename YMM::elem_t* table, vu64x4 idx) -> enable::if_64x4<YMM> { return {_mm256_i64gather_epi64(reinterpret_cast<const int64_t*>(table), idx.v, 8)}; } // returns 4 elements idx{i,j,k,l} -> {xi,xj,xk,xl}

    // NTA prefetch
    ARKXMM_API prefetch_nta(const void* p) -> void { _mm_prefetch(static_cast<const char*>(p), _MM_HINT_NTA); }

    // PCLMULQDQ carry-less integer multiplication
    template <int i0, int i1> ARKXMM_API clmul(vu64x2 a, vu64x2 b) -> vx128x1 { return {_mm_clmulepi64_si128(a.v, b.v, (i0 & 1) | (i1 & 1) << 4)}; } // PCLMULQDQ carry-less integer multiplication

    // immediate value extensions
    template <class NMM> ARKXMM_API operator &(NMM a, typename NMM::elem_t b) -> ARKXMM_DEFINE_EXTENSION(a & xmm::broadcast<NMM>(b));
    template <class NMM> ARKXMM_API operator |(NMM a, typename NMM::elem_t b) -> ARKXMM_DEFINE_EXTENSION(a | xmm::broadcast<NMM>(b));
    template <class NMM> ARKXMM_API operator ^(NMM a, typename NMM::elem_t b) -> ARKXMM_DEFINE_EXTENSION(a ^ xmm::broadcast<NMM>(b));
    template <class NMM> ARKXMM_API operator +(NMM a, typename NMM::elem_t b) -> ARKXMM_DEFINE_EXTENSION(a + xmm::broadcast<NMM>(b));
    template <class NMM> ARKXMM_API operator -(NMM a, typename NMM::elem_t b) -> ARKXMM_DEFINE_EXTENSION(a - xmm::broadcast<NMM>(b));
    template <class NMM> ARKXMM_API operator *(NMM a, typename NMM::elem_t b) -> ARKXMM_DEFINE_EXTENSION(a * xmm::broadcast<NMM>(b));
    template <class NMM> ARKXMM_API operator ==(NMM a, typename NMM::elem_t b) -> ARKXMM_DEFINE_EXTENSION(a == xmm::broadcast<NMM>(b));
    template <class NMM> ARKXMM_API operator <(NMM a, typename NMM::elem_t b) -> ARKXMM_DEFINE_EXTENSION(a < xmm::broadcast<NMM>(b));
    template <class NMM> ARKXMM_API operator >(NMM a, typename NMM::elem_t b) -> ARKXMM_DEFINE_EXTENSION(a > xmm::broadcast<NMM>(b));
    template <class NMM> ARKXMM_API max(NMM a, typename NMM::elem_t b) -> ARKXMM_DEFINE_EXTENSION(max(a, xmm::broadcast<NMM>(b)));
    template <class NMM> ARKXMM_API min(NMM a, typename NMM::elem_t b) -> ARKXMM_DEFINE_EXTENSION(min(a, xmm::broadcast<NMM>(b)));

    // operator extensions
    template <class T, class U> ARKXMM_API operator &=(T& lhs, U rhs) -> ARKXMM_DEFINE_EXTENSION(lhs = lhs & rhs);
    template <class T, class U> ARKXMM_API operator |=(T& lhs, U rhs) -> ARKXMM_DEFINE_EXTENSION(lhs = lhs | rhs);
    template <class T, class U> ARKXMM_API operator ^=(T& lhs, U rhs) -> ARKXMM_DEFINE_EXTENSION(lhs = lhs ^ rhs);
    template <class T, class U> ARKXMM_API operator +=(T& lhs, U rhs) -> ARKXMM_DEFINE_EXTENSION(lhs = lhs + rhs);
    template <class T, class U> ARKXMM_API operator -=(T& lhs, U rhs) -> ARKXMM_DEFINE_EXTENSION(lhs = lhs - rhs);
    template <class T, class U> ARKXMM_API operator *=(T& lhs, U rhs) -> ARKXMM_DEFINE_EXTENSION(lhs = lhs * rhs);
    template <class T, class U> ARKXMM_API operator <<=(T& lhs, U rhs) -> ARKXMM_DEFINE_EXTENSION(lhs = lhs << rhs);
    template <class T, class U> ARKXMM_API operator >>=(T& lhs, U rhs) -> ARKXMM_DEFINE_EXTENSION(lhs = lhs >> rhs);
    template <class T, class U> ARKXMM_API clamp(T lhs, U mi, U ma) -> ARKXMM_DEFINE_EXTENSION(min(max(lhs, mi), ma));
    template <class T, class U> ARKXMM_API operator !=(T lhs, U rhs) -> ARKXMM_DEFINE_EXTENSION(~(lhs == rhs));
    template <class T, class U> ARKXMM_API operator <=(T lhs, U rhs) -> ARKXMM_DEFINE_EXTENSION(min(lhs, rhs) == lhs);
    template <class T, class U> ARKXMM_API operator >=(T lhs, U rhs) -> ARKXMM_DEFINE_EXTENSION(max(lhs, rhs) == lhs);
    template <class T, class U> ARKXMM_API operator <(T lhs, U rhs) -> ARKXMM_DEFINE_EXTENSION(andnot(lhs <= rhs, lhs == rhs));
    template <class T, class U> ARKXMM_API operator >(T lhs, U rhs) -> ARKXMM_DEFINE_EXTENSION(andnot(lhs >= rhs, lhs == rhs));

    // pair operator extensions (recursive)
    template <class T> ARKXMM_API load_u(const PAIR<T>* src) -> ARKXMM_DEFINE_EXTENSION(PAIR<T>{load_u(&src->l), load_u(&src->r)});
    template <class T> ARKXMM_API load_a(const PAIR<T>* src) -> ARKXMM_DEFINE_EXTENSION(PAIR<T>{load_a(&src->l), load_a(&src->r)});
    template <class T> ARKXMM_API load_s(const PAIR<T>* src) -> ARKXMM_DEFINE_EXTENSION(PAIR<T>{load_s(&src->l), load_s(&src->r)});
    template <class T> ARKXMM_API store_u(PAIR<T>* dst, PAIR<T> v) -> ARKXMM_DEFINE_EXTENSION(store_u(&dst->l, v.l), store_u(&dst->r, v.r));
    template <class T> ARKXMM_API store_a(PAIR<T>* dst, PAIR<T> v) -> ARKXMM_DEFINE_EXTENSION(store_a(&dst->l, v.l), store_a(&dst->r, v.r));
    template <class T> ARKXMM_API store_s(PAIR<T>* dst, PAIR<T> v) -> ARKXMM_DEFINE_EXTENSION(store_s(&dst->l, v.l), store_s(&dst->r, v.r));
    template <class T> ARKXMM_API operator ~(PAIR<T> a) -> ARKXMM_DEFINE_EXTENSION(PAIR<T>{~a.l, ~a.r});
    template <class T> ARKXMM_API operator &(PAIR<T> a, PAIR<T> b) -> ARKXMM_DEFINE_EXTENSION(PAIR<T>{a.l & b.l, a.r & b.r});
    template <class T> ARKXMM_API operator |(PAIR<T> a, PAIR<T> b) -> ARKXMM_DEFINE_EXTENSION(PAIR<T>{a.l | b.l, a.r | b.r});
    template <class T> ARKXMM_API operator ^(PAIR<T> a, PAIR<T> b) -> ARKXMM_DEFINE_EXTENSION(PAIR<T>{a.l ^ b.l, a.r ^ b.r});
    template <class T> ARKXMM_API operator +(PAIR<T> a, PAIR<T> b) -> ARKXMM_DEFINE_EXTENSION(PAIR<T>{a.l + b.l, a.r + b.r});
    template <class T> ARKXMM_API operator -(PAIR<T> a, PAIR<T> b) -> ARKXMM_DEFINE_EXTENSION(PAIR<T>{a.l - b.l, a.r - b.r});
    template <class T> ARKXMM_API operator *(PAIR<T> a, PAIR<T> b) -> ARKXMM_DEFINE_EXTENSION(PAIR<T>{a.l * b.l, a.r * b.r});
    template <class T> ARKXMM_API operator <<(PAIR<T> a, PAIR<T> b) -> ARKXMM_DEFINE_EXTENSION(PAIR<T>{a.l << b.l, a.r << b.r});
    template <class T> ARKXMM_API operator >>(PAIR<T> a, PAIR<T> b) -> ARKXMM_DEFINE_EXTENSION(PAIR<T>{a.l >> b.l, a.r >> b.r});
    template <class T, class U> ARKXMM_API operator <<(PAIR<T> a, U b) -> ARKXMM_DEFINE_EXTENSION(PAIR<T>{a.l << b, a.r << b});
    template <class T, class U> ARKXMM_API operator >>(PAIR<T> a, U b) -> ARKXMM_DEFINE_EXTENSION(PAIR<T>{a.l >> b, a.r >> b});
    template <class T> ARKXMM_API operator ==(PAIR<T> a, PAIR<T> b) -> ARKXMM_DEFINE_EXTENSION(PAIR<T>{a.l == b.l, a.r == b.r});
    template <class T> ARKXMM_API operator <(PAIR<T> a, PAIR<T> b) -> ARKXMM_DEFINE_EXTENSION(PAIR<T>{a.l < b.l, a.r < b.r});
    template <class T> ARKXMM_API operator >(PAIR<T> a, PAIR<T> b) -> ARKXMM_DEFINE_EXTENSION(PAIR<T>{a.l > b.l, a.r > b.r});


#if __has_include("./hex-int-literals.h")
    inline namespace literals
    {
        namespace literal_parser
        {
            template <class VectorType, char...cs>
            static inline constexpr VectorType parse()
            {
                using namespace hex_int_literals::parser;
                using elem_t = typename VectorType::elem_t;
                constexpr size_t size = VectorType::size;
                constexpr size_t elemSizeInNibble = sizeof(elem_t) * 2;
                constexpr size_t vectorSizeInNibble = sizeof(VectorType) * 2;
                constexpr std::array<elem_t, size> values = parse_hexint_array<elem_t, cs...>(
                    is<vectorSizeInNibble>,
                    isXn<elemSizeInNibble>,
                    isXn<elemSizeInNibble>);
                return std::apply([](auto ...vals) constexpr { return from_values<VectorType>(vals...); }, values);
            }
        }

        template <char...cs> static inline vi8x16 operator ""_vi8x16() { return literal_parser::parse<vi8x16, cs...>(); }
        template <char...cs> static inline vu8x16 operator ""_vu8x16() { return literal_parser::parse<vu8x16, cs...>(); }
        template <char...cs> static inline vi16x8 operator ""_vi16x8() { return literal_parser::parse<vi16x8, cs...>(); }
        template <char...cs> static inline vu16x8 operator ""_vu16x8() { return literal_parser::parse<vu16x8, cs...>(); }
        template <char...cs> static inline vi32x4 operator ""_vi32x4() { return literal_parser::parse<vi32x4, cs...>(); }
        template <char...cs> static inline vu32x4 operator ""_vu32x4() { return literal_parser::parse<vu32x4, cs...>(); }
        template <char...cs> static inline vi64x2 operator ""_vi64x2() { return literal_parser::parse<vi64x2, cs...>(); }
        template <char...cs> static inline vu64x2 operator ""_vu64x2() { return literal_parser::parse<vu64x2, cs...>(); }

        template <char...cs> static inline vi8x32 operator ""_vi8x32() { return literal_parser::parse<vi8x32, cs...>(); }
        template <char...cs> static inline vu8x32 operator ""_vu8x32() { return literal_parser::parse<vu8x32, cs...>(); }
        template <char...cs> static inline vi16x16 operator ""_vi16x16() { return literal_parser::parse<vi16x16, cs...>(); }
        template <char...cs> static inline vu16x16 operator ""_vu16x16() { return literal_parser::parse<vu16x16, cs...>(); }
        template <char...cs> static inline vi32x8 operator ""_vi32x8() { return literal_parser::parse<vi32x8, cs...>(); }
        template <char...cs> static inline vu32x8 operator ""_vu32x8() { return literal_parser::parse<vu32x8, cs...>(); }
        template <char...cs> static inline vi64x4 operator ""_vi64x4() { return literal_parser::parse<vi64x4, cs...>(); }
        template <char...cs> static inline vu64x4 operator ""_vu64x4() { return literal_parser::parse<vu64x4, cs...>(); }
    }
#endif

    // Extract single byte from each element
    template <uint8_t index> ARKXMM_API extract_byte(vu32x4 v) -> std::enable_if_t<index < 4, vu32x4> { return reinterpret<vu32x4>(byte_shuffle_128(v, reinterpret<vi8x16>(from_values<vu32x4>(0xFFFFFF00 | index, 0xFFFFFF04 | index, 0xFFFFFF08 | index, 0xFFFFFF0C | index)))); }
    template <uint8_t index> ARKXMM_API extract_byte(vu32x8 v) -> std::enable_if_t<index < 4, vu32x8> { return reinterpret<vu32x8>(byte_shuffle_128(v, reinterpret<vi8x32>(from_values<vu32x8>(0xFFFFFF00 | index, 0xFFFFFF04 | index, 0xFFFFFF08 | index, 0xFFFFFF0C | index, 0xFFFFFF00 | index, 0xFFFFFF04 | index, 0xFFFFFF08 | index, 0xFFFFFF0C | index)))); }
    template <uint8_t index> ARKXMM_API extract_byte(vu64x2 v) -> std::enable_if_t<index < 8, vu64x2> { return reinterpret<vu64x2>(byte_shuffle_128(v, reinterpret<vi8x16>(from_values<vu64x2>(0xFFFFFFFFFFFFFF00 | index, 0xFFFFFFFFFFFFFF08 | index)))); }
    template <uint8_t index> ARKXMM_API extract_byte(vu64x4 v) -> std::enable_if_t<index < 8, vu64x4> { return reinterpret<vu64x4>(byte_shuffle_128(v, reinterpret<vi8x32>(from_values<vu64x4>(0xFFFFFFFFFFFFFF00 | index, 0xFFFFFFFFFFFFFF08 | index, 0xFFFFFFFFFFFFFF00 | index, 0xFFFFFFFFFFFFFF08 | index)))); }

    ARKXMM_API rotl(vu16x8 v, int i) -> vu16x8 { return v << (i & 15) | v >> (-i & 15); }
    ARKXMM_API rotl(vu16x16 v, int i) -> vu16x16 { return v << (i & 15) | v >> (-i & 15); }
    ARKXMM_API rotl(vu32x4 v, int i) -> vu32x4 { return v << (i & 31) | v >> (-i & 31); }
    ARKXMM_API rotl(vu32x8 v, int i) -> vu32x8 { return v << (i & 31) | v >> (-i & 31); }
    ARKXMM_API rotl(vu64x4 v, int i) -> vu64x4 { return v << (i & 63) | v >> (-i & 63); }
    ARKXMM_API rotl(vu64x2 v, int i) -> vu64x2 { return v << (i & 63) | v >> (-i & 63); }
    ARKXMM_API rotr(vu16x8 v, int i) -> vu16x8 { return v >> (i & 15) | v << (-i & 15); }
    ARKXMM_API rotr(vu16x16 v, int i) -> vu16x16 { return v >> (i & 15) | v << (-i & 15); }
    ARKXMM_API rotr(vu32x4 v, int i) -> vu32x4 { return v >> (i & 31) | v << (-i & 31); }
    ARKXMM_API rotr(vu32x8 v, int i) -> vu32x8 { return v >> (i & 31) | v << (-i & 31); }
    ARKXMM_API rotr(vu64x2 v, int i) -> vu64x2 { return v >> (i & 63) | v << (-i & 63); }
    ARKXMM_API rotr(vu64x4 v, int i) -> vu64x4 { return v >> (i & 63) | v << (-i & 63); }
    ARKXMM_API byteswap(vu16x8 v) -> vu16x8 { return byte_shuffle_128(v, from_values<XMM<int8_t>>(1, 0, 3, 2, 5, 4, 7, 6, 9, 8, 11, 10, 13, 12, 15, 14)); }
    ARKXMM_API byteswap(vu16x16 v) -> vu16x16 { return byte_shuffle_128(v, from_values<YMM<int8_t>>(1, 0, 3, 2, 5, 4, 7, 6, 9, 8, 11, 10, 13, 12, 15, 14, 1, 0, 3, 2, 5, 4, 7, 6, 9, 8, 11, 10, 13, 12, 15, 14)); }
    ARKXMM_API byteswap(vu32x4 v) -> vu32x4 { return byte_shuffle_128(v, from_values<XMM<int8_t>>(3, 2, 1, 0, 7, 6, 5, 4, 11, 10, 9, 8, 15, 14, 13, 12)); }
    ARKXMM_API byteswap(vu32x8 v) -> vu32x8 { return byte_shuffle_128(v, from_values<YMM<int8_t>>(3, 2, 1, 0, 7, 6, 5, 4, 11, 10, 9, 8, 15, 14, 13, 12, 3, 2, 1, 0, 7, 6, 5, 4, 11, 10, 9, 8, 15, 14, 13, 12)); }
    ARKXMM_API byteswap(vu64x2 v) -> vu64x2 { return byte_shuffle_128(v, from_values<XMM<int8_t>>(7, 6, 5, 4, 3, 2, 1, 0, 15, 14, 13, 12, 11, 10, 9, 8)); }
    ARKXMM_API byteswap(vu64x4 v) -> vu64x4 { return byte_shuffle_128(v, from_values<YMM<int8_t>>(7, 6, 5, 4, 3, 2, 1, 0, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 15, 14, 13, 12, 11, 10, 9, 8)); }

    // x00 x01 x02 x03 | x04 x05 x06 x07
    // x10 x11 x12 x13 | x14 x15 x16 x17
    // x20 x21 x22 x23 | x24 x25 x26 x27
    // x30 x31 x32 x33 | x34 x35 x36 x37
    // to 
    // x00 x10 x20 x30 | x04 x14 x24 x34
    // x01 x11 x21 x31 | x05 x15 x25 x35
    // x02 x12 x22 x32 | x06 x16 x26 x36
    // x03 x13 x23 x33 | x07 x17 x27 x37
    template <template <class T> class NMM, class T>
    ARKXMM_API transpose_32x4x4(PAIR<PAIR<NMM<T>>> reg) -> PAIR<PAIR<enable::if_32xN<NMM<T>>>>
    {
        auto i0 = reg.l.l;                                       // x00 x01 x02 x03 | x04 x05 x06 x07
        auto i1 = reg.l.r;                                       // x10 x11 x12 x13 | x14 x15 x16 x17
        auto i2 = reg.r.l;                                       // x20 x21 x22 x23 | x24 x25 x26 x27
        auto i3 = reg.r.r;                                       // x30 x31 x32 x33 | x34 x35 x36 x37
        auto t0 = reinterpret<NMM<uint64_t>>(unpack_lo(i0, i1)); // x00_x10 x01_x11 | x04_x14 x05_x15
        auto t1 = reinterpret<NMM<uint64_t>>(unpack_hi(i0, i1)); // x02_x12 x03_x13 | x06_x16 x07_x17
        auto t2 = reinterpret<NMM<uint64_t>>(unpack_lo(i2, i3)); // x20_x30 x21_x31 | x24_x34 x25_x35
        auto t3 = reinterpret<NMM<uint64_t>>(unpack_hi(i2, i3)); // x22_x32 x23_x33 | x26_x36 x27_x37
        auto o0 = reinterpret<NMM<T>>(unpack_lo(t0, t2));        // x00 x10 x20 x30 | x04 x14 x24 x34
        auto o1 = reinterpret<NMM<T>>(unpack_hi(t0, t2));        // x01 x11 x21 x31 | x05 x15 x25 x35
        auto o2 = reinterpret<NMM<T>>(unpack_lo(t1, t3));        // x02 x12 x22 x32 | x06 x16 x26 x36
        auto o3 = reinterpret<NMM<T>>(unpack_hi(t1, t3));        // x03 x13 x23 x33 | x07 x17 x27 x37
        return {{o0, o1}, {o2, o3}};
    }

    // x00 x01 x02 x03 | x04 x05 x06 x07
    // to
    // x00 x00 x00 x00 | x04 x04 x04 x04
    // x01 x01 x01 x01 | x05 x05 x05 x05
    // x02 x02 x02 x02 | x06 x06 x06 x06
    // x03 x03 x03 x03 | x07 x07 x07 x07
    template <class NMM>
    ARKXMM_API transpose_broadcast_i32x4x4(NMM v) -> PAIR<PAIR<enable::if_32xN<NMM>>>
    {
        NMM a = shuffle<0b00000000>(v);
        NMM b = shuffle<0b01010101>(v);
        NMM c = shuffle<0b10101010>(v);
        NMM d = shuffle<0b11111111>(v);
        return PAIR<PAIR<NMM>>{PAIR<NMM>{a, b}, PAIR<NMM>{c, d}};
    }
}
