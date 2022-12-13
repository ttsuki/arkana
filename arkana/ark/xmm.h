/// @file
/// @brief	arkana::xmm - x86 SIMD Operation wrappers
/// @author Copyright(c) 2020-2022 ttsuki
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

#if defined(_MSC_VER)
#ifdef NDEBUG
#define ARKXMM_INLINE inline __forceinline
#define ARKXMM_VECTORCALL __vectorcall
#else
#define ARKXMM_INLINE inline
#define ARKXMM_VECTORCALL
#endif
#else
#define ARKXMM_INLINE inline __attribute__((always_inline))
#define ARKXMM_VECTORCALL
#endif

#define ARKXMM_API  static ARKXMM_INLINE auto ARKXMM_VECTORCALL
#define ARKXMM_DEFINE_EXTENSION(...) decltype(__VA_ARGS__) { return (__VA_ARGS__); }

namespace arkana::xmm
{
    using std::int8_t;
    using std::int16_t;
    using std::int32_t;
    using std::int64_t;
    using std::uint8_t;
    using std::uint16_t;
    using std::uint32_t;
    using std::uint64_t;
    using float32_t = float;
    using float64_t = double;

#ifdef __SIZEOF_INT128__ // if compiler has __int128
    using xint128_t = unsigned __int128;
#else
    using xint128_t = __m128i;
#endif

    /// SSE __m128i
    template <class T>
    struct alignas(16) XMM
    {
        using vector_t = __m128i;
        using element_t = T;
        static constexpr inline size_t element_bits = sizeof(element_t) * 8;
        static constexpr inline size_t size = sizeof(vector_t) / sizeof(element_t);
        using array_t = std::array<element_t, size>;
        vector_t v;
    };

    /// SSE __m128
    template <>
    struct alignas(16) XMM<float32_t>
    {
        using vector_t = __m128;
        using element_t = float32_t;
        static constexpr inline size_t element_bits = sizeof(element_t) * 8;
        static constexpr inline size_t size = sizeof(vector_t) / sizeof(element_t);
        using array_t = std::array<element_t, size>;
        vector_t v;
    };

    /// SSE __m128d
    template <>
    struct alignas(16) XMM<float64_t>
    {
        using vector_t = __m128d;
        using element_t = float64_t;
        static constexpr inline size_t element_bits = sizeof(element_t) * 8;
        static constexpr inline size_t size = sizeof(vector_t) / sizeof(element_t);
        using array_t = std::array<element_t, size>;
        vector_t v;
    };

    using vi8x16 = XMM<int8_t>;
    using vu8x16 = XMM<uint8_t>;
    using vi16x8 = XMM<int16_t>;
    using vu16x8 = XMM<uint16_t>;
    using vi32x4 = XMM<int32_t>;
    using vu32x4 = XMM<uint32_t>;
    using vf32x4 = XMM<float32_t>;
    using vi64x2 = XMM<int64_t>;
    using vu64x2 = XMM<uint64_t>;
    using vf64x2 = XMM<float64_t>;
    using vx128x1 = XMM<xint128_t>;

    /// AVX __m256i
    template <class T>
    struct alignas(32) YMM
    {
        using vector_t = __m256i;
        using element_t = T;
        static constexpr inline size_t element_bits = sizeof(element_t) * 8;
        static constexpr inline size_t size = sizeof(vector_t) / sizeof(element_t);
        using array_t = std::array<element_t, size>;
        vector_t v;
    };

    /// AVX __m256
    template <>
    struct alignas(32) YMM<float32_t>
    {
        using vector_t = __m256;
        using element_t = float32_t;
        static constexpr inline size_t element_bits = sizeof(element_t) * 8;
        static constexpr inline size_t size = sizeof(vector_t) / sizeof(element_t);
        using array_t = std::array<element_t, size>;
        vector_t v;
    };

    /// AVX __m256d
    template <>
    struct alignas(32) YMM<float64_t>
    {
        using vector_t = __m256d;
        using element_t = float64_t;
        static constexpr inline size_t element_bits = sizeof(element_t) * 8;
        static constexpr inline size_t size = sizeof(vector_t) / sizeof(element_t);
        using array_t = std::array<element_t, size>;
        vector_t v;
    };

    using vi8x32 = YMM<int8_t>;
    using vu8x32 = YMM<uint8_t>;
    using vi16x16 = YMM<int16_t>;
    using vu16x16 = YMM<uint16_t>;
    using vi32x8 = YMM<int32_t>;
    using vu32x8 = YMM<uint32_t>;
    using vf32x8 = YMM<float32_t>;
    using vi64x4 = YMM<int64_t>;
    using vu64x4 = YMM<uint64_t>;
    using vf64x4 = YMM<float64_t>;
    using vx128x2 = YMM<xint128_t>;

    struct SHIFT
    {
        int64_t i;

        explicit SHIFT(int64_t i) : i(i) { }

        ARKXMM_INLINE ARKXMM_VECTORCALL operator XMM<int64_t>() const { return {_mm_set1_epi64x(i)}; }
        ARKXMM_INLINE ARKXMM_VECTORCALL operator YMM<int64_t>() const { return {_mm256_set1_epi64x(i)}; }
        ARKXMM_INLINE ARKXMM_VECTORCALL operator __m128i() const { return _mm_set1_epi64x(i); }
        ARKXMM_INLINE ARKXMM_VECTORCALL operator __m256i() const { return _mm256_set1_epi64x(i); }
    };

    namespace enable
    {
        template <class TMM, class UMM, class VMM = TMM> using if_ = std::enable_if_t<std::is_same_v<TMM, UMM>, VMM>;

        template <class XMM, class T = XMM> using if_iXMM = std::enable_if_t<!std::is_floating_point_v<typename XMM::element_t> && XMM::element_bits * XMM::size == 128, T>;
        template <class XMM, class T = XMM> using if_8x16 = std::enable_if_t<!std::is_floating_point_v<typename XMM::element_t> && XMM::element_bits * XMM::size == 128 && XMM::element_bits == 8, T>;
        template <class XMM, class T = XMM> using if_16x8 = std::enable_if_t<!std::is_floating_point_v<typename XMM::element_t> && XMM::element_bits * XMM::size == 128 && XMM::element_bits == 16, T>;
        template <class XMM, class T = XMM> using if_32x4 = std::enable_if_t<!std::is_floating_point_v<typename XMM::element_t> && XMM::element_bits * XMM::size == 128 && XMM::element_bits == 32, T>;
        template <class XMM, class T = XMM> using if_64x2 = std::enable_if_t<!std::is_floating_point_v<typename XMM::element_t> && XMM::element_bits * XMM::size == 128 && XMM::element_bits == 64, T>;
        template <class XMM, class T = XMM> using if_f32x4 = std::enable_if_t<std::is_floating_point_v<typename XMM::element_t> && XMM::element_bits * XMM::size == 128 && XMM::element_bits == 32, T>;
        template <class XMM, class T = XMM> using if_f64x2 = std::enable_if_t<std::is_floating_point_v<typename XMM::element_t> && XMM::element_bits * XMM::size == 128 && XMM::element_bits == 64, T>;

        template <class YMM, class T = YMM> using if_iYMM = std::enable_if_t<!std::is_floating_point_v<typename YMM::element_t> && YMM::element_bits * YMM::size == 256, T>;
        template <class YMM, class T = YMM> using if_8x32 = std::enable_if_t<!std::is_floating_point_v<typename YMM::element_t> && YMM::element_bits * YMM::size == 256 && YMM::element_bits == 8, T>;
        template <class YMM, class T = YMM> using if_16x16 = std::enable_if_t<!std::is_floating_point_v<typename YMM::element_t> && YMM::element_bits * YMM::size == 256 && YMM::element_bits == 16, T>;
        template <class YMM, class T = YMM> using if_32x8 = std::enable_if_t<!std::is_floating_point_v<typename YMM::element_t> && YMM::element_bits * YMM::size == 256 && YMM::element_bits == 32, T>;
        template <class YMM, class T = YMM> using if_64x4 = std::enable_if_t<!std::is_floating_point_v<typename YMM::element_t> && YMM::element_bits * YMM::size == 256 && YMM::element_bits == 64, T>;
        template <class YMM, class T = YMM> using if_f32x8 = std::enable_if_t<std::is_floating_point_v<typename YMM::element_t> && YMM::element_bits * YMM::size == 256 && YMM::element_bits == 32, T>;
        template <class YMM, class T = YMM> using if_f64x4 = std::enable_if_t<std::is_floating_point_v<typename YMM::element_t> && YMM::element_bits * YMM::size == 256 && YMM::element_bits == 64, T>;

        template <class NMM, class T = NMM> using if_iNMM = std::enable_if_t<!std::is_floating_point_v<typename NMM::element_t> && (NMM::element_bits * NMM::size == 128 || NMM::element_bits * NMM::size == 256), T>;
        template <class NMM, class T = NMM> using if_8xN = std::enable_if_t<!std::is_floating_point_v<typename NMM::element_t> && (NMM::element_bits * NMM::size == 128 || NMM::element_bits * NMM::size == 256) && NMM::element_bits == 8, T>;
        template <class NMM, class T = NMM> using if_16xN = std::enable_if_t<!std::is_floating_point_v<typename NMM::element_t> && (NMM::element_bits * NMM::size == 128 || NMM::element_bits * NMM::size == 256) && NMM::element_bits == 16, T>;
        template <class NMM, class T = NMM> using if_32xN = std::enable_if_t<!std::is_floating_point_v<typename NMM::element_t> && (NMM::element_bits * NMM::size == 128 || NMM::element_bits * NMM::size == 256) && NMM::element_bits == 32, T>;
        template <class NMM, class T = NMM> using if_64xN = std::enable_if_t<!std::is_floating_point_v<typename NMM::element_t> && (NMM::element_bits * NMM::size == 128 || NMM::element_bits * NMM::size == 256) && NMM::element_bits == 64, T>;
        template <class NMM, class T = NMM> using if_f32xN = std::enable_if_t<std::is_floating_point_v<typename NMM::element_t> && (NMM::element_bits * NMM::size == 128 || NMM::element_bits * NMM::size == 256) && NMM::element_bits == 32, T>;
        template <class NMM, class T = NMM> using if_f64xN = std::enable_if_t<std::is_floating_point_v<typename NMM::element_t> && (NMM::element_bits * NMM::size == 128 || NMM::element_bits * NMM::size == 256) && NMM::element_bits == 64, T>;
    }

    template <class XMM> ARKXMM_API load_u(const void* src) -> enable::if_iXMM<XMM> { return XMM{_mm_lddqu_si128(&static_cast<const XMM*>(src)->v)}; }                         // SSE3
    template <class XMM> ARKXMM_API load_u(const void* src) -> enable::if_f32x4<XMM> { return XMM{_mm_loadu_ps(static_cast<const vf32x4::element_t*>(src))}; }                 // SSE
    template <class XMM> ARKXMM_API load_u(const void* src) -> enable::if_f64x2<XMM> { return XMM{_mm_loadu_pd(static_cast<const vf64x2::element_t*>(src))}; }                 // SSE2
    template <class XMM> ARKXMM_API load_a(const void* src) -> enable::if_iXMM<XMM> { return XMM{_mm_load_si128(&static_cast<const XMM*>(src)->v)}; }                          // SSE2
    template <class XMM> ARKXMM_API load_a(const void* src) -> enable::if_f32x4<XMM> { return XMM{_mm_load_ps(static_cast<const vf32x4::element_t*>(src))}; }                  // SSE
    template <class XMM> ARKXMM_API load_a(const void* src) -> enable::if_f64x2<XMM> { return XMM{_mm_load_pd(static_cast<const vf64x2::element_t*>(src))}; }                  // SSE2
    template <class XMM> ARKXMM_API load_s(const void* src) -> enable::if_iXMM<XMM> { return XMM{_mm_stream_load_si128(&const_cast<XMM*>(static_cast<const XMM*>(src))->v)}; } // SSE4.1
    template <class XMM> ARKXMM_API load_s(const void* src) -> enable::if_f32x4<XMM> { return XMM{_mm_load_ps(static_cast<const vf32x4::element_t*>(src))}; }                  // SSE
    template <class XMM> ARKXMM_API load_s(const void* src) -> enable::if_f64x2<XMM> { return XMM{_mm_load_pd(static_cast<const vf64x2::element_t*>(src))}; }                  // SSE2
    template <class YMM> ARKXMM_API load_u(const void* src) -> enable::if_iYMM<YMM> { return YMM{_mm256_lddqu_si256(&static_cast<const YMM*>(src)->v)}; }                      // AVX
    template <class YMM> ARKXMM_API load_u(const void* src) -> enable::if_f32x8<YMM> { return YMM{_mm256_loadu_ps(static_cast<const vf32x4::element_t*>(src))}; }              // SSE
    template <class YMM> ARKXMM_API load_u(const void* src) -> enable::if_f64x4<YMM> { return YMM{_mm256_loadu_pd(static_cast<const vf64x2::element_t*>(src))}; }              // SSE2
    template <class YMM> ARKXMM_API load_a(const void* src) -> enable::if_iYMM<YMM> { return YMM{_mm256_load_si256(&static_cast<const YMM*>(src)->v)}; }                       // AVX
    template <class YMM> ARKXMM_API load_a(const void* src) -> enable::if_f32x8<YMM> { return YMM{_mm256_load_ps(static_cast<const vf32x4::element_t*>(src))}; }               // SSE
    template <class YMM> ARKXMM_API load_a(const void* src) -> enable::if_f64x4<YMM> { return YMM{_mm256_load_pd(static_cast<const vf64x2::element_t*>(src))}; }               // SSE2
    template <class YMM> ARKXMM_API load_s(const void* src) -> enable::if_iYMM<YMM> { return YMM{_mm256_stream_load_si256(&static_cast<const YMM*>(src)->v)}; }                // AVX2
    template <class YMM> ARKXMM_API load_s(const void* src) -> enable::if_f32x8<YMM> { return YMM{_mm256_load_ps(static_cast<const vf32x4::element_t*>(src))}; }               // AVX
    template <class YMM> ARKXMM_API load_s(const void* src) -> enable::if_f64x4<YMM> { return YMM{_mm256_load_pd(static_cast<const vf64x2::element_t*>(src))}; }               // AVX

    template <class XMM> ARKXMM_API store_u(void* dst, const std::decay_t<XMM> v) -> enable::if_iXMM<XMM> { return _mm_storeu_si128(&static_cast<XMM*>(dst)->v, v.v), v; }            // SSE2
    template <class XMM> ARKXMM_API store_u(void* dst, const std::decay_t<XMM> v) -> enable::if_f32x4<XMM> { return _mm_storeu_ps(static_cast<vf32x4::element_t*>(dst), v.v), v; }    // SSE
    template <class XMM> ARKXMM_API store_u(void* dst, const std::decay_t<XMM> v) -> enable::if_f64x2<XMM> { return _mm_storeu_pd(static_cast<vf64x2::element_t*>(dst), v.v), v; }    // SSE2
    template <class XMM> ARKXMM_API store_a(void* dst, const std::decay_t<XMM> v) -> enable::if_iXMM<XMM> { return _mm_store_si128(&static_cast<XMM*>(dst)->v, v.v), v; }             // SSE2
    template <class XMM> ARKXMM_API store_a(void* dst, const std::decay_t<XMM> v) -> enable::if_f32x4<XMM> { return _mm_store_ps(static_cast<vf32x4::element_t*>(dst), v.v), v; }     // SSE
    template <class XMM> ARKXMM_API store_a(void* dst, const std::decay_t<XMM> v) -> enable::if_f64x2<XMM> { return _mm_store_pd(static_cast<vf64x2::element_t*>(dst), v.v), v; }     // SSE2
    template <class XMM> ARKXMM_API store_s(void* dst, const std::decay_t<XMM> v) -> enable::if_iXMM<XMM> { return _mm_stream_si128(&static_cast<XMM*>(dst)->v, v.v), v; }            // SSE2
    template <class XMM> ARKXMM_API store_s(void* dst, const std::decay_t<XMM> v) -> enable::if_f32x4<XMM> { return _mm_stream_ps(static_cast<vf32x4::element_t*>(dst), v.v), v; }    // SSE
    template <class XMM> ARKXMM_API store_s(void* dst, const std::decay_t<XMM> v) -> enable::if_f64x2<XMM> { return _mm_stream_pd(static_cast<vf64x2::element_t*>(dst), v.v), v; }    // SSE2
    template <class YMM> ARKXMM_API store_u(void* dst, const std::decay_t<YMM> v) -> enable::if_iYMM<YMM> { return _mm256_storeu_si256(&static_cast<YMM*>(dst)->v, v.v), v; }         // AVX
    template <class YMM> ARKXMM_API store_u(void* dst, const std::decay_t<YMM> v) -> enable::if_f32x8<YMM> { return _mm256_storeu_ps(static_cast<vf32x4::element_t*>(dst), v.v), v; } // AVX
    template <class YMM> ARKXMM_API store_u(void* dst, const std::decay_t<YMM> v) -> enable::if_f64x4<YMM> { return _mm256_storeu_pd(static_cast<vf64x2::element_t*>(dst), v.v), v; } // AVX
    template <class YMM> ARKXMM_API store_a(void* dst, const std::decay_t<YMM> v) -> enable::if_iYMM<YMM> { return _mm256_store_si256(&static_cast<YMM*>(dst)->v, v.v), v; }          // AVX
    template <class YMM> ARKXMM_API store_a(void* dst, const std::decay_t<YMM> v) -> enable::if_f32x8<YMM> { return _mm256_store_ps(static_cast<vf32x4::element_t*>(dst), v.v), v; }  // AVX
    template <class YMM> ARKXMM_API store_a(void* dst, const std::decay_t<YMM> v) -> enable::if_f64x4<YMM> { return _mm256_store_pd(static_cast<vf64x2::element_t*>(dst), v.v), v; }  // AVX
    template <class YMM> ARKXMM_API store_s(void* dst, const std::decay_t<YMM> v) -> enable::if_iYMM<YMM> { return _mm256_stream_si256(&static_cast<YMM*>(dst)->v, v.v), v; }         // AVX
    template <class YMM> ARKXMM_API store_s(void* dst, const std::decay_t<YMM> v) -> enable::if_f32x8<YMM> { return _mm256_stream_ps(static_cast<vf32x4::element_t*>(dst), v.v), v; } // AVX
    template <class YMM> ARKXMM_API store_s(void* dst, const std::decay_t<YMM> v) -> enable::if_f64x4<YMM> { return _mm256_stream_pd(static_cast<vf64x2::element_t*>(dst), v.v), v; } // AVX

    /// to array
    template <class NMM> ARKXMM_API to_array(NMM v) -> typename NMM::array_t
    {
        typename NMM::array_t a;
        store_u<NMM>(a.data(), v);
        return a;
    }

    template <class To, class T> ARKXMM_API reinterpret(XMM<T> v) -> enable::if_iXMM<To> { return To{v.v}; } // cast XMM to another XMM
    template <class To, class T> ARKXMM_API reinterpret(YMM<T> v) -> enable::if_iYMM<To> { return To{v.v}; } // cast YMM to another YMM

    template <class XMM> ARKXMM_API zero() -> enable::if_iXMM<XMM> { return {_mm_setzero_si128()}; }    // SSE2
    template <class YMM> ARKXMM_API zero() -> enable::if_f32x4<YMM> { return {_mm_setzero_ps()}; }      // SSE
    template <class YMM> ARKXMM_API zero() -> enable::if_f64x2<YMM> { return {_mm_setzero_pd()}; }      // SSE2
    template <class YMM> ARKXMM_API zero() -> enable::if_iYMM<YMM> { return {_mm256_setzero_si256()}; } // AVX
    template <class YMM> ARKXMM_API zero() -> enable::if_f32x8<YMM> { return {_mm256_setzero_ps()}; }   // AVX
    template <class YMM> ARKXMM_API zero() -> enable::if_f64x4<YMM> { return {_mm256_setzero_pd()}; }   // AVX

    // broadcast - use as `broadcast<vu32x4>(123)`
    template <class XMM> ARKXMM_API broadcast(typename XMM::element_t val) -> enable::if_8x16<XMM> { return {_mm_set1_epi8(static_cast<int8_t>(val))}; }                               // SSE2
    template <class XMM> ARKXMM_API broadcast(typename XMM::element_t val) -> enable::if_16x8<XMM> { return {_mm_set1_epi16(static_cast<int16_t>(val))}; }                             // SSE2
    template <class XMM> ARKXMM_API broadcast(typename XMM::element_t val) -> enable::if_32x4<XMM> { return {_mm_set1_epi32(static_cast<int32_t>(val))}; }                             // SSE2
    template <class XMM> ARKXMM_API broadcast(typename XMM::element_t val) -> enable::if_64x2<XMM> { return {_mm_set1_epi64x(static_cast<int64_t>(val))}; }                            // SSE2
    template <class XMM> ARKXMM_API broadcast(typename XMM::element_t val) -> enable::if_f32x4<XMM> { return {_mm_set1_ps(static_cast<float32_t>(val))}; }                             // SSE2
    template <class XMM> ARKXMM_API broadcast(typename XMM::element_t val) -> enable::if_f64x2<XMM> { return {_mm_set1_pd(static_cast<float64_t>(val))}; }                             // SSE2
    template <class YMM> ARKXMM_API broadcast(typename YMM::element_t val) -> enable::if_8x32<YMM> { return {_mm256_set1_epi8(static_cast<int8_t>(val))}; }                            // AVX
    template <class YMM> ARKXMM_API broadcast(typename YMM::element_t val) -> enable::if_16x16<YMM> { return {_mm256_set1_epi16(static_cast<int16_t>(val))}; }                         // AVX
    template <class YMM> ARKXMM_API broadcast(typename YMM::element_t val) -> enable::if_32x8<YMM> { return {_mm256_set1_epi32(static_cast<int32_t>(val))}; }                          // AVX
    template <class YMM> ARKXMM_API broadcast(typename YMM::element_t val) -> enable::if_64x4<YMM> { return {_mm256_set1_epi64x(static_cast<int64_t>(val))}; }                         // AVX
    template <class YMM> ARKXMM_API broadcast(typename YMM::element_t val) -> enable::if_f32x8<YMM> { return {_mm256_set1_ps(static_cast<float32_t>(val))}; }                          // AVX
    template <class YMM> ARKXMM_API broadcast(typename YMM::element_t val) -> enable::if_f64x4<YMM> { return {_mm256_set1_pd(static_cast<float64_t>(val))}; }                          // AVX
    template <class YMM> ARKXMM_API broadcast(XMM<typename YMM::element_t> val) -> enable::if_iYMM<YMM> { return {_mm256_insertf128_si256(_mm256_castsi128_si256(val.v), val.v, 1)}; } // AVX2
    template <class YMM> ARKXMM_API broadcast(XMM<typename YMM::element_t> val) -> enable::if_f32x8<YMM> { return {_mm256_insertf128_ps(_mm256_castps128_ps256(val.v), val.v, 1)}; }   // AVX
    template <class YMM> ARKXMM_API broadcast(XMM<typename YMM::element_t> val) -> enable::if_f64x4<YMM> { return {_mm256_insertf128_pd(_mm256_castpd128_pd256(val.v), val.v, 1)}; }   // AVX

    // XMM from_values - use as `from_values<vu32x4>(1, 2, 3, 4)`
    template <class XMM> ARKXMM_API from_values(typename XMM::element_t x0, typename XMM::element_t x1, typename XMM::element_t x2, typename XMM::element_t x3, typename XMM::element_t x4, typename XMM::element_t x5, typename XMM::element_t x6, typename XMM::element_t x7, typename XMM::element_t x8, typename XMM::element_t x9, typename XMM::element_t xA, typename XMM::element_t xB, typename XMM::element_t xC, typename XMM::element_t xD, typename XMM::element_t xE, typename XMM::element_t xF) -> enable::if_8x16<XMM> { return {_mm_setr_epi8(static_cast<int8_t>(x0), static_cast<int8_t>(x1), static_cast<int8_t>(x2), static_cast<int8_t>(x3), static_cast<int8_t>(x4), static_cast<int8_t>(x5), static_cast<int8_t>(x6), static_cast<int8_t>(x7), static_cast<int8_t>(x8), static_cast<int8_t>(x9), static_cast<int8_t>(xA), static_cast<int8_t>(xB), static_cast<int8_t>(xC), static_cast<int8_t>(xD), static_cast<int8_t>(xE), static_cast<int8_t>(xF))}; }
    template <class XMM> ARKXMM_API from_values(typename XMM::element_t x0, typename XMM::element_t x1, typename XMM::element_t x2, typename XMM::element_t x3, typename XMM::element_t x4, typename XMM::element_t x5, typename XMM::element_t x6, typename XMM::element_t x7) -> enable::if_16x8<XMM> { return {_mm_setr_epi16(static_cast<int16_t>(x0), static_cast<int16_t>(x1), static_cast<int16_t>(x2), static_cast<int16_t>(x3), static_cast<int16_t>(x4), static_cast<int16_t>(x5), static_cast<int16_t>(x6), static_cast<int16_t>(x7))}; }
    template <class XMM> ARKXMM_API from_values(typename XMM::element_t x0, typename XMM::element_t x1, typename XMM::element_t x2, typename XMM::element_t x3) -> enable::if_32x4<XMM> { return {_mm_setr_epi32(static_cast<int32_t>(x0), static_cast<int32_t>(x1), static_cast<int32_t>(x2), static_cast<int32_t>(x3))}; }
    template <class XMM> ARKXMM_API from_values(typename XMM::element_t x0, typename XMM::element_t x1) -> enable::if_64x2<XMM> { return {_mm_set_epi64x(static_cast<int64_t>(x1), static_cast<int64_t>(x0))}; }
    template <class XMM> ARKXMM_API from_values(typename XMM::element_t x0, typename XMM::element_t x1, typename XMM::element_t x2, typename XMM::element_t x3) -> enable::if_f32x4<XMM> { return {_mm_setr_ps(x0, x1, x2, x3)}; }
    template <class XMM> ARKXMM_API from_values(typename XMM::element_t x0, typename XMM::element_t x1) -> enable::if_f64x2<XMM> { return {_mm_setr_pd(x0, x1)}; }

    // YMM from_values - use as `from_values<vu32x8>(1, 2, 3, 4, 5, 6, 7, 8)`
    template <class YMM> ARKXMM_API from_values(typename YMM::element_t x00, typename YMM::element_t x01, typename YMM::element_t x02, typename YMM::element_t x03, typename YMM::element_t x04, typename YMM::element_t x05, typename YMM::element_t x06, typename YMM::element_t x07, typename YMM::element_t x08, typename YMM::element_t x09, typename YMM::element_t x0A, typename YMM::element_t x0B, typename YMM::element_t x0C, typename YMM::element_t x0D, typename YMM::element_t x0E, typename YMM::element_t x0F, typename YMM::element_t x10, typename YMM::element_t x11, typename YMM::element_t x12, typename YMM::element_t x13, typename YMM::element_t x14, typename YMM::element_t x15, typename YMM::element_t x16, typename YMM::element_t x17, typename YMM::element_t x18, typename YMM::element_t x19, typename YMM::element_t x1A, typename YMM::element_t x1B, typename YMM::element_t x1C, typename YMM::element_t x1D, typename YMM::element_t x1E, typename YMM::element_t x1F) -> enable::if_8x32<YMM> { return {_mm256_setr_epi8(static_cast<int8_t>(x00), static_cast<int8_t>(x01), static_cast<int8_t>(x02), static_cast<int8_t>(x03), static_cast<int8_t>(x04), static_cast<int8_t>(x05), static_cast<int8_t>(x06), static_cast<int8_t>(x07), static_cast<int8_t>(x08), static_cast<int8_t>(x09), static_cast<int8_t>(x0A), static_cast<int8_t>(x0B), static_cast<int8_t>(x0C), static_cast<int8_t>(x0D), static_cast<int8_t>(x0E), static_cast<int8_t>(x0F), static_cast<int8_t>(x10), static_cast<int8_t>(x11), static_cast<int8_t>(x12), static_cast<int8_t>(x13), static_cast<int8_t>(x14), static_cast<int8_t>(x15), static_cast<int8_t>(x16), static_cast<int8_t>(x17), static_cast<int8_t>(x18), static_cast<int8_t>(x19), static_cast<int8_t>(x1A), static_cast<int8_t>(x1B), static_cast<int8_t>(x1C), static_cast<int8_t>(x1D), static_cast<int8_t>(x1E), static_cast<int8_t>(x1F))}; }
    template <class YMM> ARKXMM_API from_values(typename YMM::element_t x0, typename YMM::element_t x1, typename YMM::element_t x2, typename YMM::element_t x3, typename YMM::element_t x4, typename YMM::element_t x5, typename YMM::element_t x6, typename YMM::element_t x7, typename YMM::element_t x8, typename YMM::element_t x9, typename YMM::element_t xA, typename YMM::element_t xB, typename YMM::element_t xC, typename YMM::element_t xD, typename YMM::element_t xE, typename YMM::element_t xF) -> enable::if_16x16<YMM> { return {_mm256_setr_epi16(static_cast<int16_t>(x0), static_cast<int16_t>(x1), static_cast<int16_t>(x2), static_cast<int16_t>(x3), static_cast<int16_t>(x4), static_cast<int16_t>(x5), static_cast<int16_t>(x6), static_cast<int16_t>(x7), static_cast<int16_t>(x8), static_cast<int16_t>(x9), static_cast<int16_t>(xA), static_cast<int16_t>(xB), static_cast<int16_t>(xC), static_cast<int16_t>(xD), static_cast<int16_t>(xE), static_cast<int16_t>(xF))}; }
    template <class YMM> ARKXMM_API from_values(typename YMM::element_t x0, typename YMM::element_t x1, typename YMM::element_t x2, typename YMM::element_t x3, typename YMM::element_t x4, typename YMM::element_t x5, typename YMM::element_t x6, typename YMM::element_t x7) -> enable::if_32x8<YMM> { return {_mm256_setr_epi32(static_cast<int32_t>(x0), static_cast<int32_t>(x1), static_cast<int32_t>(x2), static_cast<int32_t>(x3), static_cast<int32_t>(x4), static_cast<int32_t>(x5), static_cast<int32_t>(x6), static_cast<int32_t>(x7))}; }
    template <class YMM> ARKXMM_API from_values(typename YMM::element_t x0, typename YMM::element_t x1, typename YMM::element_t x2, typename YMM::element_t x3) -> enable::if_64x4<YMM> { return {_mm256_setr_epi64x(static_cast<int64_t>(x0), static_cast<int64_t>(x1), static_cast<int64_t>(x2), static_cast<int64_t>(x3))}; }
    template <class YMM> ARKXMM_API from_values(typename YMM::element_t x0, typename YMM::element_t x1, typename YMM::element_t x2, typename YMM::element_t x3, typename YMM::element_t x4, typename YMM::element_t x5, typename YMM::element_t x6, typename YMM::element_t x7) -> enable::if_f32x8<YMM> { return {_mm256_setr_ps(static_cast<float32_t>(x0), static_cast<float32_t>(x1), static_cast<float32_t>(x2), static_cast<float32_t>(x3), static_cast<float32_t>(x4), static_cast<float32_t>(x5), static_cast<float32_t>(x6), static_cast<float32_t>(x7))}; }
    template <class YMM> ARKXMM_API from_values(typename YMM::element_t x0, typename YMM::element_t x1, typename YMM::element_t x2, typename YMM::element_t x3) -> enable::if_f64x4<YMM> { return {_mm256_setr_pd(static_cast<float64_t>(x0), static_cast<float64_t>(x1), static_cast<float64_t>(x2), static_cast<float64_t>(x3))}; }
    template <class YMM> ARKXMM_API from_values(XMM<typename YMM::element_t> x0, XMM<typename YMM::element_t> x1) -> enable::if_iYMM<YMM> { return {_mm256_inserti128_si256(_mm256_castsi128_si256(x0.v), x1.v, 1)}; }
    template <class YMM> ARKXMM_API from_values(XMM<typename YMM::element_t> x0, XMM<typename YMM::element_t> x1) -> enable::if_f32x8<YMM> { return {_mm256_insertf128_ps(_mm256_castps128_ps256(x0.v), x1.v, 1)}; }
    template <class YMM> ARKXMM_API from_values(XMM<typename YMM::element_t> x0, XMM<typename YMM::element_t> x1) -> enable::if_f64x4<YMM> { return {_mm256_insertf128_pd(_mm256_castpd128_pd256(x0.v), x1.v, 1)}; }

    // YMM from_values xmm x2 - use as `from_values<vu32x8>(1, 2, 3, 4)`
    template <class YMM> ARKXMM_API from_values(typename YMM::element_t x0, typename YMM::element_t x1, typename YMM::element_t x2, typename YMM::element_t x3, typename YMM::element_t x4, typename YMM::element_t x5, typename YMM::element_t x6, typename YMM::element_t x7, typename YMM::element_t x8, typename YMM::element_t x9, typename YMM::element_t xA, typename YMM::element_t xB, typename YMM::element_t xC, typename YMM::element_t xD, typename YMM::element_t xE, typename YMM::element_t xF) -> enable::if_8x32<YMM> { return {_mm256_setr_epi8(static_cast<int8_t>(x0), static_cast<int8_t>(x1), static_cast<int8_t>(x2), static_cast<int8_t>(x3), static_cast<int8_t>(x4), static_cast<int8_t>(x5), static_cast<int8_t>(x6), static_cast<int8_t>(x7), static_cast<int8_t>(x8), static_cast<int8_t>(x9), static_cast<int8_t>(xA), static_cast<int8_t>(xB), static_cast<int8_t>(xC), static_cast<int8_t>(xD), static_cast<int8_t>(xE), static_cast<int8_t>(xF), static_cast<int8_t>(x0), static_cast<int8_t>(x1), static_cast<int8_t>(x2), static_cast<int8_t>(x3), static_cast<int8_t>(x4), static_cast<int8_t>(x5), static_cast<int8_t>(x6), static_cast<int8_t>(x7), static_cast<int8_t>(x8), static_cast<int8_t>(x9), static_cast<int8_t>(xA), static_cast<int8_t>(xB), static_cast<int8_t>(xC), static_cast<int8_t>(xD), static_cast<int8_t>(xE), static_cast<int8_t>(xF))}; }
    template <class YMM> ARKXMM_API from_values(typename YMM::element_t x0, typename YMM::element_t x1, typename YMM::element_t x2, typename YMM::element_t x3, typename YMM::element_t x4, typename YMM::element_t x5, typename YMM::element_t x6, typename YMM::element_t x7) -> enable::if_16x16<YMM> { return {_mm256_setr_epi16(static_cast<int16_t>(x0), static_cast<int16_t>(x1), static_cast<int16_t>(x2), static_cast<int16_t>(x3), static_cast<int16_t>(x4), static_cast<int16_t>(x5), static_cast<int16_t>(x6), static_cast<int16_t>(x7), static_cast<int16_t>(x0), static_cast<int16_t>(x1), static_cast<int16_t>(x2), static_cast<int16_t>(x3), static_cast<int16_t>(x4), static_cast<int16_t>(x5), static_cast<int16_t>(x6), static_cast<int16_t>(x7))}; }
    template <class YMM> ARKXMM_API from_values(typename YMM::element_t x0, typename YMM::element_t x1, typename YMM::element_t x2, typename YMM::element_t x3) -> enable::if_32x8<YMM> { return {_mm256_setr_epi32(static_cast<int32_t>(x0), static_cast<int32_t>(x1), static_cast<int32_t>(x2), static_cast<int32_t>(x3), static_cast<int32_t>(x0), static_cast<int32_t>(x1), static_cast<int32_t>(x2), static_cast<int32_t>(x3))}; }
    template <class YMM> ARKXMM_API from_values(typename YMM::element_t x0, typename YMM::element_t x1) -> enable::if_64x4<YMM> { return {_mm256_set_epi64x(static_cast<int64_t>(x1), static_cast<int64_t>(x0), static_cast<int64_t>(x1), static_cast<int64_t>(x0))}; }
    template <class YMM> ARKXMM_API from_values(typename YMM::element_t x0, typename YMM::element_t x1, typename YMM::element_t x2, typename YMM::element_t x3) -> enable::if_f32x8<YMM> { return {_mm256_setr_ps(static_cast<float32_t>(x0), static_cast<float32_t>(x1), static_cast<float32_t>(x2), static_cast<float32_t>(x3), static_cast<float32_t>(x0), static_cast<float32_t>(x1), static_cast<float32_t>(x2), static_cast<float32_t>(x3))}; }
    template <class YMM> ARKXMM_API from_values(typename YMM::element_t x0, typename YMM::element_t x1) -> enable::if_f64x4<YMM> { return {_mm256_setr_pd(static_cast<float64_t>(x0), static_cast<float64_t>(x1), static_cast<float64_t>(x0), static_cast<float64_t>(x1))}; }
    template <class YMM> ARKXMM_API from_values(XMM<typename YMM::element_t> x0) -> enable::if_iYMM<YMM> { return broadcast<YMM>(x0); }  // AVX2
    template <class YMM> ARKXMM_API from_values(XMM<typename YMM::element_t> x0) -> enable::if_f32x8<YMM> { return broadcast<YMM>(x0); } // AVX
    template <class YMM> ARKXMM_API from_values(XMM<typename YMM::element_t> x0) -> enable::if_f64x4<YMM> { return broadcast<YMM>(x0); } // AVX

    // bitwise operators
    template <class T> ARKXMM_API operator ~(XMM<T> a) -> XMM<T> { return {_mm_xor_si128(a.v, _mm_cmpeq_epi32(a.v, a.v))}; }       // SSE2
    template <class T> ARKXMM_API operator ~(YMM<T> a) -> YMM<T> { return {_mm256_xor_si256(a.v, _mm256_cmpeq_epi32(a.v, a.v))}; } // AVX2
    template <class T> ARKXMM_API operator &(XMM<T> a, XMM<T> b) -> XMM<T> { return {_mm_and_si128(a.v, b.v)}; }                   // SSE2
    template <class T> ARKXMM_API operator &(YMM<T> a, YMM<T> b) -> YMM<T> { return {_mm256_and_si256(a.v, b.v)}; }                // AVX2
    template <class T> ARKXMM_API operator |(XMM<T> a, XMM<T> b) -> XMM<T> { return {_mm_or_si128(a.v, b.v)}; }                    // SSE2
    template <class T> ARKXMM_API operator |(YMM<T> a, YMM<T> b) -> YMM<T> { return {_mm256_or_si256(a.v, b.v)}; }                 // AVX2
    template <class T> ARKXMM_API operator ^(XMM<T> a, XMM<T> b) -> XMM<T> { return {_mm_xor_si128(a.v, b.v)}; }                   // SSE2
    template <class T> ARKXMM_API operator ^(YMM<T> a, YMM<T> b) -> YMM<T> { return {_mm256_xor_si256(a.v, b.v)}; }                // AVX2
    template <class T> ARKXMM_API andnot(XMM<T> a, XMM<T> mask) -> XMM<T> { return {_mm_andnot_si128(mask.v, a.v)}; }              // SSE2 a & ~mask
    template <class T> ARKXMM_API andnot(YMM<T> a, YMM<T> mask) -> YMM<T> { return {_mm256_andnot_si256(mask.v, a.v)}; }           // AVX2 a & ~mask

    template <class T, class U> ARKXMM_API testz(XMM<T> v, XMM<U> mask) -> int { return _mm_testz_si128(mask.v, v.v); }     // SSE4.1
    template <class T, class U> ARKXMM_API testc(XMM<T> v, XMM<U> mask) -> int { return _mm_testc_si128(mask.v, v.v); }     // SSE4.1
    template <class T, class U> ARKXMM_API testnzc(XMM<T> v, XMM<U> mask) -> int { return _mm_testnzc_si128(mask.v, v.v); } // SSE4.1

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
    ARKXMM_API i8x32(vi8x16 v) -> vi8x32 { return broadcast<vi8x32>(v); }
    ARKXMM_API u8x32(vu8x16 v) -> vu8x32 { return broadcast<vu8x32>(v); }
    ARKXMM_API i16x8(int16_t v) -> vi16x8 { return broadcast<vi16x8>(v); }
    ARKXMM_API u16x8(uint16_t v) -> vu16x8 { return broadcast<vu16x8>(v); }
    ARKXMM_API i16x16(int16_t v) -> vi16x16 { return broadcast<vi16x16>(v); }
    ARKXMM_API u16x16(uint16_t v) -> vu16x16 { return broadcast<vu16x16>(v); }
    ARKXMM_API i16x16(vi16x8 v) -> vi16x16 { return broadcast<vi16x16>(v); }
    ARKXMM_API u16x16(vu16x8 v) -> vu16x16 { return broadcast<vu16x16>(v); }
    ARKXMM_API i32x4(int32_t v) -> vi32x4 { return broadcast<vi32x4>(v); }
    ARKXMM_API u32x4(uint32_t v) -> vu32x4 { return broadcast<vu32x4>(v); }
    ARKXMM_API f32x4(float32_t v) -> vf32x4 { return broadcast<vf32x4>(v); }
    ARKXMM_API i32x8(int32_t v) -> vi32x8 { return broadcast<vi32x8>(v); }
    ARKXMM_API u32x8(uint32_t v) -> vu32x8 { return broadcast<vu32x8>(v); }
    ARKXMM_API f32x8(float32_t v) -> vf32x8 { return broadcast<vf32x8>(v); }
    ARKXMM_API i32x8(vi32x4 v) -> vi32x8 { return broadcast<vi32x8>(v); }
    ARKXMM_API u32x8(vu32x4 v) -> vu32x8 { return broadcast<vu32x8>(v); }
    ARKXMM_API f32x8(vf32x4 v) -> vf32x8 { return broadcast<vf32x8>(v); }
    ARKXMM_API i64x2(int64_t v) -> vi64x2 { return broadcast<vi64x2>(v); }
    ARKXMM_API u64x2(uint64_t v) -> vu64x2 { return broadcast<vu64x2>(v); }
    ARKXMM_API f64x2(float64_t v) -> vf64x2 { return broadcast<vf64x2>(v); }
    ARKXMM_API i64x4(int64_t v) -> vi64x4 { return broadcast<vi64x4>(v); }
    ARKXMM_API u64x4(uint64_t v) -> vu64x4 { return broadcast<vu64x4>(v); }
    ARKXMM_API f64x4(float64_t v) -> vf64x4 { return broadcast<vf64x4>(v); }
    ARKXMM_API i64x4(vi64x2 v) -> vi64x4 { return broadcast<vi64x4>(v); }
    ARKXMM_API u64x4(vu64x2 v) -> vu64x4 { return broadcast<vu64x4>(v); }
    ARKXMM_API f64x4(vf64x2 v) -> vf64x4 { return broadcast<vf64x4>(v); }

    // from values shortcut
    ARKXMM_API i8x16(int8_t x0, int8_t x1, int8_t x2, int8_t x3, int8_t x4, int8_t x5, int8_t x6, int8_t x7, int8_t x8, int8_t x9, int8_t xA, int8_t xB, int8_t xC, int8_t xD, int8_t xE, int8_t xF) -> vi8x16 { return from_values<vi8x16>(x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, xA, xB, xC, xD, xE, xF); }
    ARKXMM_API u8x16(uint8_t x0, uint8_t x1, uint8_t x2, uint8_t x3, uint8_t x4, uint8_t x5, uint8_t x6, uint8_t x7, uint8_t x8, uint8_t x9, uint8_t xA, uint8_t xB, uint8_t xC, uint8_t xD, uint8_t xE, uint8_t xF) -> vu8x16 { return from_values<vu8x16>(x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, xA, xB, xC, xD, xE, xF); }
    ARKXMM_API i8x32(int8_t x0, int8_t x1, int8_t x2, int8_t x3, int8_t x4, int8_t x5, int8_t x6, int8_t x7, int8_t x8, int8_t x9, int8_t xA, int8_t xB, int8_t xC, int8_t xD, int8_t xE, int8_t xF) -> vi8x32 { return from_values<vi8x32>(x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, xA, xB, xC, xD, xE, xF); }
    ARKXMM_API u8x32(uint8_t x0, uint8_t x1, uint8_t x2, uint8_t x3, uint8_t x4, uint8_t x5, uint8_t x6, uint8_t x7, uint8_t x8, uint8_t x9, uint8_t xA, uint8_t xB, uint8_t xC, uint8_t xD, uint8_t xE, uint8_t xF) -> vu8x32 { return from_values<vu8x32>(x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, xA, xB, xC, xD, xE, xF); }
    ARKXMM_API i8x32(int8_t x00, int8_t x01, int8_t x02, int8_t x03, int8_t x04, int8_t x05, int8_t x06, int8_t x07, int8_t x08, int8_t x09, int8_t x0A, int8_t x0B, int8_t x0C, int8_t x0D, int8_t x0E, int8_t x0F, int8_t x10, int8_t x11, int8_t x12, int8_t x13, int8_t x14, int8_t x15, int8_t x16, int8_t x17, int8_t x18, int8_t x19, int8_t x1A, int8_t x1B, int8_t x1C, int8_t x1D, int8_t x1E, int8_t x1F) -> vi8x32 { return from_values<vi8x32>(x00, x01, x02, x03, x04, x05, x06, x07, x08, x09, x0A, x0B, x0C, x0D, x0E, x0F, x10, x11, x12, x13, x14, x15, x16, x17, x18, x19, x1A, x1B, x1C, x1D, x1E, x1F); }
    ARKXMM_API u8x32(uint8_t x00, uint8_t x01, uint8_t x02, uint8_t x03, uint8_t x04, uint8_t x05, uint8_t x06, uint8_t x07, uint8_t x08, uint8_t x09, uint8_t x0A, uint8_t x0B, uint8_t x0C, uint8_t x0D, uint8_t x0E, uint8_t x0F, uint8_t x10, uint8_t x11, uint8_t x12, uint8_t x13, uint8_t x14, uint8_t x15, uint8_t x16, uint8_t x17, uint8_t x18, uint8_t x19, uint8_t x1A, uint8_t x1B, uint8_t x1C, uint8_t x1D, uint8_t x1E, uint8_t x1F) -> vu8x32 { return from_values<vu8x32>(x00, x01, x02, x03, x04, x05, x06, x07, x08, x09, x0A, x0B, x0C, x0D, x0E, x0F, x10, x11, x12, x13, x14, x15, x16, x17, x18, x19, x1A, x1B, x1C, x1D, x1E, x1F); }
    ARKXMM_API i16x8(int16_t x0, int16_t x1, int16_t x2, int16_t x3, int16_t x4, int16_t x5, int16_t x6, int16_t x7) -> vi16x8 { return from_values<vi16x8>(x0, x1, x2, x3, x4, x5, x6, x7); }
    ARKXMM_API u16x8(uint16_t x0, uint16_t x1, uint16_t x2, uint16_t x3, uint16_t x4, uint16_t x5, uint16_t x6, uint16_t x7) -> vu16x8 { return from_values<vu16x8>(x0, x1, x2, x3, x4, x5, x6, x7); }
    ARKXMM_API i16x16(int16_t x0, int16_t x1, int16_t x2, int16_t x3, int16_t x4, int16_t x5, int16_t x6, int16_t x7) -> vi16x16 { return from_values<vi16x16>(x0, x1, x2, x3, x4, x5, x6, x7); }
    ARKXMM_API u16x16(uint16_t x0, uint16_t x1, uint16_t x2, uint16_t x3, uint16_t x4, uint16_t x5, uint16_t x6, uint16_t x7) -> vu16x16 { return from_values<vu16x16>(x0, x1, x2, x3, x4, x5, x6, x7); }
    ARKXMM_API i16x16(int16_t x0, int16_t x1, int16_t x2, int16_t x3, int16_t x4, int16_t x5, int16_t x6, int16_t x7, int16_t x8, int16_t x9, int16_t xA, int16_t xB, int16_t xC, int16_t xD, int16_t xE, int16_t xF) -> vi16x16 { return from_values<vi16x16>(x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, xA, xB, xC, xD, xE, xF); }
    ARKXMM_API u16x16(uint16_t x0, uint16_t x1, uint16_t x2, uint16_t x3, uint16_t x4, uint16_t x5, uint16_t x6, uint16_t x7, uint16_t x8, uint16_t x9, uint16_t xA, uint16_t xB, uint16_t xC, uint16_t xD, uint16_t xE, uint16_t xF) -> vu16x16 { return from_values<vu16x16>(x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, xA, xB, xC, xD, xE, xF); }
    ARKXMM_API i32x4(int32_t x0, int32_t x1, int32_t x2, int32_t x3) -> vi32x4 { return from_values<vi32x4>(x0, x1, x2, x3); }
    ARKXMM_API u32x4(uint32_t x0, uint32_t x1, uint32_t x2, uint32_t x3) -> vu32x4 { return from_values<vu32x4>(x0, x1, x2, x3); }
    ARKXMM_API f32x4(float32_t x0, float32_t x1, float32_t x2, float32_t x3) -> vf32x4 { return from_values<vf32x4>(x0, x1, x2, x3); }
    ARKXMM_API i32x8(int32_t x0, int32_t x1, int32_t x2, int32_t x3) -> vi32x8 { return from_values<vi32x8>(x0, x1, x2, x3); }
    ARKXMM_API u32x8(uint32_t x0, uint32_t x1, uint32_t x2, uint32_t x3) -> vu32x8 { return from_values<vu32x8>(x0, x1, x2, x3); }
    ARKXMM_API f32x8(float32_t x0, float32_t x1, float32_t x2, float32_t x3) -> vf32x8 { return from_values<vf32x8>(x0, x1, x2, x3); }
    ARKXMM_API i32x8(int32_t x0, int32_t x1, int32_t x2, int32_t x3, int32_t x4, int32_t x5, int32_t x6, int32_t x7) -> vi32x8 { return from_values<vi32x8>(x0, x1, x2, x3, x4, x5, x6, x7); }
    ARKXMM_API u32x8(uint32_t x0, uint32_t x1, uint32_t x2, uint32_t x3, uint32_t x4, uint32_t x5, uint32_t x6, uint32_t x7) -> vu32x8 { return from_values<vu32x8>(x0, x1, x2, x3, x4, x5, x6, x7); }
    ARKXMM_API f32x8(float32_t x0, float32_t x1, float32_t x2, float32_t x3, float32_t x4, float32_t x5, float32_t x6, float32_t x7) -> vf32x8 { return from_values<vf32x8>(x0, x1, x2, x3, x4, x5, x6, x7); }
    ARKXMM_API i64x2(int64_t x0, int64_t x1) -> vi64x2 { return from_values<vi64x2>(x0, x1); }
    ARKXMM_API u64x2(uint64_t x0, uint64_t x1) -> vu64x2 { return from_values<vu64x2>(x0, x1); }
    ARKXMM_API f64x2(float64_t x0, float64_t x1) -> vf64x2 { return from_values<vf64x2>(x0, x1); }
    ARKXMM_API i64x4(int64_t x0, int64_t x1) -> vi64x4 { return from_values<vi64x4>(x0, x1); }
    ARKXMM_API u64x4(uint64_t x0, uint64_t x1) -> vu64x4 { return from_values<vu64x4>(x0, x1); }
    ARKXMM_API f64x4(float64_t x0, float64_t x1) -> vf64x2 { return from_values<vf64x2>(x0, x1); }
    ARKXMM_API i64x4(int64_t x0, int64_t x1, int64_t x2, int64_t x3) -> vi64x4 { return from_values<vi64x4>(x0, x1, x2, x3); }
    ARKXMM_API u64x4(uint64_t x0, uint64_t x1, uint64_t x2, uint64_t x3) -> vu64x4 { return from_values<vu64x4>(x0, x1, x2, x3); }
    ARKXMM_API f64x4(float64_t x0, float64_t x1, float64_t x2, float64_t x3) -> vf64x4 { return from_values<vf64x4>(x0, x1, x2, x3); }

    ARKXMM_API i8x32(vi8x16 x0, vi8x16 x1) -> vi8x32 { return from_values<vi8x32>(x0, x1); }
    ARKXMM_API u8x32(vu8x16 x0, vu8x16 x1) -> vu8x32 { return from_values<vu8x32>(x0, x1); }
    ARKXMM_API i16x16(vi16x8 x0, vi16x8 x1) -> vi16x16 { return from_values<vi16x16>(x0, x1); }
    ARKXMM_API u16x16(vu16x8 x0, vu16x8 x1) -> vu16x16 { return from_values<vu16x16>(x0, x1); }
    ARKXMM_API i32x8(vi32x4 x0, vi32x4 x1) -> vi32x8 { return from_values<vi32x8>(x0, x1); }
    ARKXMM_API u32x8(vu32x4 x0, vu32x4 x1) -> vu32x8 { return from_values<vu32x8>(x0, x1); }
    ARKXMM_API f32x8(vf32x4 x0, vf32x4 x1) -> vf32x8 { return from_values<vf32x8>(x0, x1); }
    ARKXMM_API i64x4(vi64x2 x0, vi64x2 x1) -> vi64x4 { return from_values<vi64x4>(x0, x1); }
    ARKXMM_API u64x4(vu64x2 x0, vu64x2 x1) -> vu64x4 { return from_values<vu64x4>(x0, x1); }
    ARKXMM_API f64x4(vf64x2 x0, vf64x2 x1) -> vf64x4 { return from_values<vf64x4>(x0, x1); }

    // insert single element into vector
    template <uint8_t index_4bit> ARKXMM_API insert_element(vi8x16 v, int8_t x) -> vi8x16 { return {_mm_insert_epi8(v.v, x, index_4bit)}; }         // SSE4.1
    template <uint8_t index_4bit> ARKXMM_API insert_element(vu8x16 v, uint8_t x) -> vu8x16 { return {_mm_insert_epi8(v.v, x, index_4bit)}; }        // SSE4.1
    template <uint8_t index_5bit> ARKXMM_API insert_element(vi8x32 v, int8_t x) -> vi8x32 { return {_mm256_insert_epi8(v.v, x, index_5bit)}; }      // AVX
    template <uint8_t index_5bit> ARKXMM_API insert_element(vu8x32 v, uint8_t x) -> vu8x32 { return {_mm256_insert_epi8(v.v, x, index_5bit)}; }     // AVX
    template <uint8_t index_3bit> ARKXMM_API insert_element(vi16x8 v, int16_t x) -> vi16x8 { return {_mm_insert_epi16(v.v, x, index_3bit)}; }       // SSE2
    template <uint8_t index_3bit> ARKXMM_API insert_element(vu16x8 v, uint16_t x) -> vu16x8 { return {_mm_insert_epi16(v.v, x, index_3bit)}; }      // SSE2
    template <uint8_t index_4bit> ARKXMM_API insert_element(vi16x16 v, int16_t x) -> vi16x16 { return {_mm256_insert_epi16(v.v, x, index_4bit)}; }  // AVX
    template <uint8_t index_4bit> ARKXMM_API insert_element(vu16x16 v, uint16_t x) -> vu16x16 { return {_mm256_insert_epi16(v.v, x, index_4bit)}; } // AVX
    template <uint8_t index_2bit> ARKXMM_API insert_element(vi32x4 v, int32_t x) -> vi32x4 { return {_mm_insert_epi32(v.v, x, index_2bit)}; }       // SSE4.1
    template <uint8_t index_2bit> ARKXMM_API insert_element(vu32x4 v, uint32_t x) -> vu32x4 { return {_mm_insert_epi32(v.v, x, index_2bit)}; }      // SSE4.1
    template <uint8_t index_3bit> ARKXMM_API insert_element(vi32x8 v, int32_t x) -> vi32x8 { return {_mm256_insert_epi32(v.v, x, index_3bit)}; }    // AVX
    template <uint8_t index_3bit> ARKXMM_API insert_element(vu32x8 v, uint32_t x) -> vu32x8 { return {_mm256_insert_epi32(v.v, x, index_3bit)}; }   // AVX
    template <uint8_t index_1bit> ARKXMM_API insert_element(vi64x2 v, int64_t x) -> vi64x2 { return {_mm_insert_epi64(v.v, x, index_1bit)}; }       // SSE4.1
    template <uint8_t index_1bit> ARKXMM_API insert_element(vu64x2 v, uint64_t x) -> vu64x2 { return {_mm_insert_epi64(v.v, x, index_1bit)}; }      // SSE4.1
    template <uint8_t index_2bit> ARKXMM_API insert_element(vi64x4 v, int64_t x) -> vi64x4 { return {_mm256_insert_epi64(v.v, x, index_2bit)}; }    // AVX
    template <uint8_t index_2bit> ARKXMM_API insert_element(vu64x4 v, uint64_t x) -> vu64x4 { return {_mm256_insert_epi64(v.v, x, index_2bit)}; }   // AVX

    template <uint8_t dst_index_2bit, uint8_t src_index_2bit = 0, uint8_t bit_mask = 0b1111> ARKXMM_API insert_element(vf32x4 dst, vf32x4 x) -> vf32x4 { return {_mm_insert_ps(dst.v, x.v, (src_index_2bit & 3) << 6 | (dst_index_2bit & 3) << 4 | (bit_mask & 7))}; } // SSE4.1
    template <uint8_t dst_index_2bit, uint8_t bit_mask = 0b1111> ARKXMM_API insert_element(vf32x4 dst, float32_t x) -> vf32x4 { return insert_element<dst_index_2bit, 0, bit_mask>(dst, vf32x4{_mm_load_ss(&x)}); }                                                    // SSE4.1

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

    template <uint8_t index_2bit> ARKXMM_API extract_element(vf32x4 v)
    {
        static_assert(index_2bit < 4);
        if constexpr (index_2bit == 0) return _mm_cvtss_f32(v.v);
        else return _mm_cvtss_f32(_mm_shuffle_ps(v.v, v.v, index_2bit));
    }

    template <uint8_t index_3bit> ARKXMM_API extract_element(vf32x8 v)
    {
        static_assert(index_3bit < 8);
        if constexpr (index_3bit == 0) return _mm_cvtss_f32(_mm256_castps256_ps128(v.v));
        else if constexpr (index_3bit < 4) return _mm_cvtss_f32(_mm_shuffle_ps(_mm256_castps256_ps128(v.v), _mm256_castps256_ps128(v.v), index_3bit));
        else return _mm_cvtss_f32(_mm256_castps256_ps128(_mm256_permutevar8x32_ps(v.v, i8x32(static_cast<int8_t>(index_3bit)).v)));
    }

    template <uint8_t index_1bit> ARKXMM_API extract_element(vf64x2 v)
    {
        static_assert(index_1bit < 2);
        if constexpr (index_1bit == 0) return _mm_cvtsd_f64(v.v);
        else return _mm_cvtsd_f64(_mm_shuffle_pd(v.v, v.v, index_1bit));
    }

    template <uint8_t index_2bit> ARKXMM_API extract_element(vf64x4 v)
    {
        static_assert(index_2bit < 4);
        if constexpr (index_2bit == 0) return _mm_cvtsd_f64(_mm256_castpd256_pd128(v.v));
        else if constexpr (index_2bit < 4) return _mm_cvtsd_f64(_mm_shuffle_pd(_mm256_castpd256_pd128(v.v), _mm256_castpd256_pd128(v.v), index_2bit));
        else return _mm_cvtsd_f64(_mm256_castpd256_pd128(_mm256_permute4x64_pd(v.v, index_2bit)));
    }


    // blend
    ARKXMM_API blend(vi8x16 a, vi8x16 b, vi8x16 control) -> vi8x16 { return {_mm_blendv_epi8(a.v, b.v, control.v)}; }                              // SSE 4.1
    ARKXMM_API blend(vu8x16 a, vu8x16 b, vu8x16 control) -> vu8x16 { return {_mm_blendv_epi8(a.v, b.v, control.v)}; }                              // SSE 4.1
    ARKXMM_API blend(vi8x32 a, vi8x32 b, vi8x32 control) -> vi8x32 { return {_mm256_blendv_epi8(a.v, b.v, control.v)}; }                           // AVX2
    ARKXMM_API blend(vu8x32 a, vu8x32 b, vu8x32 control) -> vu8x32 { return {_mm256_blendv_epi8(a.v, b.v, control.v)}; }                           // AVX2
    ARKXMM_API blend(vf32x4 a, vf32x4 b, vf32x4 control) -> vf32x4 { return {_mm_blendv_ps(a.v, b.v, control.v)}; }                                // SSE 4.1
    ARKXMM_API blend(vf32x8 a, vf32x8 b, vf32x8 control) -> vf32x8 { return {_mm256_blendv_ps(a.v, b.v, control.v)}; }                             // AVX
    ARKXMM_API blend(vf64x2 a, vf64x2 b, vf64x2 control) -> vf64x2 { return {_mm_blendv_pd(a.v, b.v, control.v)}; }                                // SSE 4.1
    ARKXMM_API blend(vf64x4 a, vf64x4 b, vf64x4 control) -> vf64x4 { return {_mm256_blendv_pd(a.v, b.v, control.v)}; }                             // AVX
    template <int selector_1bit_x8> ARKXMM_API blend(vi16x8 a, vi16x8 b) -> vi16x8 { return {_mm_blend_epi16(a.v, b.v, selector_1bit_x8)}; }       // SSE 4.1
    template <int selector_1bit_x8> ARKXMM_API blend(vu16x8 a, vu16x8 b) -> vu16x8 { return {_mm_blend_epi16(a.v, b.v, selector_1bit_x8)}; }       // SSE 4.1
    template <int selector_1bit_x8> ARKXMM_API blend(vi16x16 a, vi16x16 b) -> vi16x16 { return {_mm256_blend_epi16(a.v, b.v, selector_1bit_x8)}; } // AVX2
    template <int selector_1bit_x8> ARKXMM_API blend(vu16x16 a, vu16x16 b) -> vu16x16 { return {_mm256_blend_epi16(a.v, b.v, selector_1bit_x8)}; } // AVX2
    template <int selector_1bit_x4> ARKXMM_API blend(vi32x4 a, vi32x4 b) -> vi32x4 { return {_mm_blend_epi32(a.v, b.v, selector_1bit_x4)}; }       // AVX2
    template <int selector_1bit_x4> ARKXMM_API blend(vu32x4 a, vu32x4 b) -> vu32x4 { return {_mm_blend_epi32(a.v, b.v, selector_1bit_x4)}; }       // AVX2
    template <int selector_1bit_x4> ARKXMM_API blend(vf32x4 a, vf32x4 b) -> vf32x4 { return {_mm_blend_ps(a.v, b.v, selector_1bit_x4)}; }          // SSE4.1
    template <int selector_1bit_x8> ARKXMM_API blend(vi32x8 a, vi32x8 b) -> vi32x8 { return {_mm256_blend_epi32(a.v, b.v, selector_1bit_x8)}; }    // AVX2
    template <int selector_1bit_x8> ARKXMM_API blend(vu32x8 a, vu32x8 b) -> vu32x8 { return {_mm256_blend_epi32(a.v, b.v, selector_1bit_x8)}; }    // AVX2
    template <int selector_1bit_x8> ARKXMM_API blend(vf32x8 a, vf32x8 b) -> vf32x8 { return {_mm256_blend_ps(a.v, b.v, selector_1bit_x8)}; }       // AVX
    template <int selector_1bit_x2> ARKXMM_API blend(vf64x2 a, vf64x2 b) -> vf64x2 { return {_mm_blend_pd(a.v, b.v, selector_1bit_x2)}; }          // SSE 4.1
    template <int selector_1bit_x4> ARKXMM_API blend(vf64x4 a, vf64x4 b) -> vf64x4 { return {_mm256_blend_pd(a.v, b.v, selector_1bit_x4)}; }       // AVX

    template <uint8_t i0, uint8_t i1, uint8_t i2, uint8_t i3, class XMM> ARKXMM_API shuffle16_lo(XMM v) -> enable::if_iXMM<XMM> { return {_mm_shufflelo_epi16(v.v, (i0 & 0b11) | (i1 & 0b11) << 2 | (i2 & 0b11) << 4 | (i3 & 0b11) << 6)}; }    // SSE2
    template <uint8_t i0, uint8_t i1, uint8_t i2, uint8_t i3, class YMM> ARKXMM_API shuffle16_lo(YMM v) -> enable::if_iYMM<YMM> { return {_mm256_shufflelo_epi16(v.v, (i0 & 0b11) | (i1 & 0b11) << 2 | (i2 & 0b11) << 4 | (i3 & 0b11) << 6)}; } // SSE2
    template <uint8_t i0, uint8_t i1, uint8_t i2, uint8_t i3, class XMM> ARKXMM_API shuffle16_hi(XMM v) -> enable::if_iXMM<XMM> { return {_mm_shufflehi_epi16(v.v, (i0 & 0b11) | (i1 & 0b11) << 2 | (i2 & 0b11) << 4 | (i3 & 0b11) << 6)}; }    // SSE2
    template <uint8_t i0, uint8_t i1, uint8_t i2, uint8_t i3, class YMM> ARKXMM_API shuffle16_hi(YMM v) -> enable::if_iYMM<YMM> { return {_mm256_shufflehi_epi16(v.v, (i0 & 0b11) | (i1 & 0b11) << 2 | (i2 & 0b11) << 4 | (i3 & 0b11) << 6)}; } // SSE2
    template <uint8_t i0, uint8_t i1, uint8_t i2, uint8_t i3, class XMM> ARKXMM_API shuffle32(XMM v) -> enable::if_iXMM<XMM> { return {_mm_shuffle_epi32(v.v, (i0 & 0b11) | (i1 & 0b11) << 2 | (i2 & 0b11) << 4 | (i3 & 0b11) << 6)}; }         // SSE2
    template <uint8_t i0, uint8_t i1, uint8_t i2, uint8_t i3, class XMM> ARKXMM_API shuffle32(XMM v) -> enable::if_f32x4<XMM> { return {_mm_shuffle_ps(v.v, v.v, (i0 & 0b11) | (i1 & 0b11) << 2 | (i2 & 0b11) << 4 | (i3 & 0b11) << 6)}; }      // SSE2
    template <uint8_t i0, uint8_t i1, uint8_t i2, uint8_t i3, class YMM> ARKXMM_API shuffle32(YMM v) -> enable::if_iYMM<YMM> { return {_mm256_shuffle_epi32(v.v, (i0 & 0b11) | (i1 & 0b11) << 2 | (i2 & 0b11) << 4 | (i3 & 0b11) << 6)}; }      // AVX2
    template <uint8_t i0, uint8_t i1, uint8_t i2, uint8_t i3, class XMM> ARKXMM_API shuffle32(XMM v) -> enable::if_f32x8<XMM> { return {_mm256_shuffle_ps(v.v, v.v, (i0 & 0b11) | (i1 & 0b11) << 2 | (i2 & 0b11) << 4 | (i3 & 0b11) << 6)}; }   // SSE2
    template <uint8_t i0, uint8_t i1, class XMM> ARKXMM_API shuffle64(XMM v) -> enable::if_iXMM<XMM> { return shuffle32<i0 * 2, i0 * 2 + 1, i1 * 2, i1 * 2 + 1, XMM>(v); }                                                                      // SSE2
    template <uint8_t i0, uint8_t i1, class YMM> ARKXMM_API shuffle64(YMM v) -> enable::if_iYMM<YMM> { return shuffle32<i0 * 2, i0 * 2 + 1, i1 * 2, i1 * 2 + 1, YMM>(v); }                                                                      // AVX2
    template <uint8_t i0, uint8_t i1, class XMM> ARKXMM_API shuffle64(XMM v) -> enable::if_f64x2<XMM> { return {_mm_shuffle_pd(v.v, v.v, (i0 & 0b1) | (i1 & 0b1) << 1)}; }                                                                      // SSE2
    template <uint8_t i0, uint8_t i1, class YMM> ARKXMM_API shuffle64(YMM v) -> enable::if_f64x4<YMM> { return {_mm256_shuffle_pd(v.v, v.v, (i0 & 0b1) | (i1 & 0b1) << 1)}; }                                                                   // AVX

    template <uint8_t i0, uint8_t i1, uint8_t i2, uint8_t i3> ARKXMM_API shuffle_lo(vi16x8 v) -> vi16x8 { return shuffle16_lo<i0, i1, i2, i3>(v); }   // SSE2
    template <uint8_t i0, uint8_t i1, uint8_t i2, uint8_t i3> ARKXMM_API shuffle_lo(vu16x8 v) -> vu16x8 { return shuffle16_lo<i0, i1, i2, i3>(v); }   // SSE2
    template <uint8_t i0, uint8_t i1, uint8_t i2, uint8_t i3> ARKXMM_API shuffle_hi(vi16x8 v) -> vi16x8 { return shuffle16_hi<i0, i1, i2, i3>(v); }   // SSE2
    template <uint8_t i0, uint8_t i1, uint8_t i2, uint8_t i3> ARKXMM_API shuffle_hi(vu16x8 v) -> vu16x8 { return shuffle16_hi<i0, i1, i2, i3>(v); }   // SSE2
    template <uint8_t i0, uint8_t i1, uint8_t i2, uint8_t i3> ARKXMM_API shuffle_lo(vi16x16 v) -> vi16x16 { return shuffle16_lo<i0, i1, i2, i3>(v); } // AVX2
    template <uint8_t i0, uint8_t i1, uint8_t i2, uint8_t i3> ARKXMM_API shuffle_lo(vu16x16 v) -> vu16x16 { return shuffle16_lo<i0, i1, i2, i3>(v); } // AVX2
    template <uint8_t i0, uint8_t i1, uint8_t i2, uint8_t i3> ARKXMM_API shuffle_hi(vi16x16 v) -> vi16x16 { return shuffle16_hi<i0, i1, i2, i3>(v); } // AVX2
    template <uint8_t i0, uint8_t i1, uint8_t i2, uint8_t i3> ARKXMM_API shuffle_hi(vu16x16 v) -> vu16x16 { return shuffle16_hi<i0, i1, i2, i3>(v); } // AVX2
    template <uint8_t i0, uint8_t i1, uint8_t i2, uint8_t i3> ARKXMM_API shuffle(vi32x4 v) -> vi32x4 { return shuffle32<i0, i1, i2, i3>(v); }         // SSE2
    template <uint8_t i0, uint8_t i1, uint8_t i2, uint8_t i3> ARKXMM_API shuffle(vu32x4 v) -> vu32x4 { return shuffle32<i0, i1, i2, i3>(v); }         // SSE2
    template <uint8_t i0, uint8_t i1, uint8_t i2, uint8_t i3> ARKXMM_API shuffle(vf32x4 v) -> vf32x4 { return shuffle32<i0, i1, i2, i3>(v); }         // AVX
    template <uint8_t i0, uint8_t i1, uint8_t i2, uint8_t i3> ARKXMM_API shuffle(vi32x8 v) -> vi32x8 { return shuffle32<i0, i1, i2, i3>(v); }         // AVX2
    template <uint8_t i0, uint8_t i1, uint8_t i2, uint8_t i3> ARKXMM_API shuffle(vu32x8 v) -> vu32x8 { return shuffle32<i0, i1, i2, i3>(v); }         // AVX2
    template <uint8_t i0, uint8_t i1, uint8_t i2, uint8_t i3> ARKXMM_API shuffle(vf32x8 v) -> vf32x8 { return shuffle32<i0, i1, i2, i3>(v); }         // AVX
    template <uint8_t i0, uint8_t i1> ARKXMM_API shuffle(vf64x2 v) -> vf64x2 { return shuffle64<i0, i1>(v); }                                         // SSE2
    template <uint8_t i0, uint8_t i1> ARKXMM_API shuffle(vf64x4 v) -> vf64x4 { return shuffle64<i0, i1>(v); }                                         // AVX


    ARKXMM_API abs(vi8x16 a) -> vi8x16 { return {_mm_abs_epi8(a.v)}; }                            // SSSE3
    ARKXMM_API abs(vi8x32 a) -> vi8x32 { return {_mm256_abs_epi8(a.v)}; }                         // AVX2
    ARKXMM_API abs(vi16x8 a) -> vi16x8 { return {_mm_abs_epi16(a.v)}; }                           // SSSE3
    ARKXMM_API abs(vi16x16 a) -> vi16x16 { return {_mm256_abs_epi16(a.v)}; }                      // AVX2
    ARKXMM_API abs(vi32x4 a) -> vi32x4 { return {_mm_abs_epi32(a.v)}; }                           // SSSE3
    ARKXMM_API abs(vf32x4 a) -> vf32x4 { return {_mm_andnot_ps(_mm_set1_ps(-0.0f), a.v)}; }       // SSE
    ARKXMM_API abs(vi32x8 a) -> vi32x8 { return {_mm256_abs_epi32(a.v)}; }                        // AVX2
    ARKXMM_API abs(vf32x8 a) -> vf32x8 { return {_mm256_andnot_ps(_mm256_set1_ps(-0.0f), a.v)}; } // AVX
    ARKXMM_API abs(vf64x2 a) -> vf64x2 { return {_mm_andnot_pd(_mm_set1_pd(-0.0), a.v)}; }        // SSE2
    ARKXMM_API abs(vf64x4 a) -> vf64x4 { return {_mm256_andnot_pd(_mm256_set1_pd(-0.0), a.v)}; }  // AVX

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
    ARKXMM_API operator +(vf32x4 a, vf32x4 b) -> vf32x4 { return {_mm_add_ps(a.v, b.v)}; }          // SSE
    ARKXMM_API operator +(vi32x8 a, vi32x8 b) -> vi32x8 { return {_mm256_add_epi32(a.v, b.v)}; }    // AVX2
    ARKXMM_API operator +(vu32x8 a, vu32x8 b) -> vu32x8 { return {_mm256_add_epi32(a.v, b.v)}; }    // AVX2
    ARKXMM_API operator +(vf32x8 a, vf32x8 b) -> vf32x8 { return {_mm256_add_ps(a.v, b.v)}; }       // AVX
    ARKXMM_API operator +(vi64x2 a, vi64x2 b) -> vi64x2 { return {_mm_add_epi64(a.v, b.v)}; }       // SSE2
    ARKXMM_API operator +(vu64x2 a, vu64x2 b) -> vu64x2 { return {_mm_add_epi64(a.v, b.v)}; }       // SSE2
    ARKXMM_API operator +(vf64x2 a, vf64x2 b) -> vf64x2 { return {_mm_add_pd(a.v, b.v)}; }          // SSE2
    ARKXMM_API operator +(vi64x4 a, vi64x4 b) -> vi64x4 { return {_mm256_add_epi64(a.v, b.v)}; }    // AVX2
    ARKXMM_API operator +(vu64x4 a, vu64x4 b) -> vu64x4 { return {_mm256_add_epi64(a.v, b.v)}; }    // AVX2
    ARKXMM_API operator +(vf64x4 a, vf64x4 b) -> vf64x4 { return {_mm256_add_pd(a.v, b.v)}; }       // AVX

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
    ARKXMM_API operator -(vf32x4 a, vf32x4 b) -> vf32x4 { return {_mm_sub_ps(a.v, b.v)}; }          // SSE
    ARKXMM_API operator -(vi32x8 a, vi32x8 b) -> vi32x8 { return {_mm256_sub_epi32(a.v, b.v)}; }    // AVX2
    ARKXMM_API operator -(vu32x8 a, vu32x8 b) -> vu32x8 { return {_mm256_sub_epi32(a.v, b.v)}; }    // AVX2
    ARKXMM_API operator -(vf32x8 a, vf32x8 b) -> vf32x8 { return {_mm256_sub_ps(a.v, b.v)}; }       // AVX
    ARKXMM_API operator -(vi64x2 a, vi64x2 b) -> vi64x2 { return {_mm_sub_epi64(a.v, b.v)}; }       // SSE2
    ARKXMM_API operator -(vu64x2 a, vu64x2 b) -> vu64x2 { return {_mm_sub_epi64(a.v, b.v)}; }       // SSE2
    ARKXMM_API operator -(vf64x2 a, vf64x2 b) -> vf64x2 { return {_mm_sub_pd(a.v, b.v)}; }          // SSE2
    ARKXMM_API operator -(vi64x4 a, vi64x4 b) -> vi64x4 { return {_mm256_sub_epi64(a.v, b.v)}; }    // AVX2
    ARKXMM_API operator -(vu64x4 a, vu64x4 b) -> vu64x4 { return {_mm256_sub_epi64(a.v, b.v)}; }    // AVX2
    ARKXMM_API operator -(vf64x4 a, vf64x4 b) -> vf64x4 { return {_mm256_sub_pd(a.v, b.v)}; }       // AVX

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
    ARKXMM_API horizontal_add(vf32x4 a, vf32x4 b) -> vf32x4 { return {_mm_hadd_ps(a.v, b.v)}; }               // SSE3 -> [a0+a1, a2+a3, b0+b1, b2+b3]
    ARKXMM_API horizontal_add(vi32x8 a, vi32x8 b) -> vi32x8 { return {_mm256_hadd_epi32(a.v, b.v)}; }         // AVX2 -> [a0+a1, a2+a3, b0+b1, b2+b3]
    ARKXMM_API horizontal_add(vu32x8 a, vu32x8 b) -> vu32x8 { return {_mm256_hadd_epi32(a.v, b.v)}; }         // AVX2 -> [a0+a1, a2+a3, b0+b1, b2+b3]
    ARKXMM_API horizontal_add(vf32x8 a, vf32x8 b) -> vf32x8 { return {_mm256_hadd_ps(a.v, b.v)}; }            // AVX -> [a0+a1, a2+a3, b0+b1, b2+b3]
    ARKXMM_API horizontal_add(vf64x2 a, vf64x2 b) -> vf64x2 { return {_mm_hadd_pd(a.v, b.v)}; }               // SSE3 -> [a0+a1, a2+a3, b0+b1, b2+b3]
    ARKXMM_API horizontal_add(vf64x4 a, vf64x4 b) -> vf64x4 { return {_mm256_hadd_pd(a.v, b.v)}; }            // AVX -> [a0+a1, a2+a3, b0+b1, b2+b3]
    ARKXMM_API horizontal_add_sat(vu16x8 a, vu16x8 b) -> vu16x8 { return {_mm_hadds_epi16(a.v, b.v)}; }       // SSSE3 -> [a0+a1, a2+a3, ..., b0+b1, b2+b3, ...]
    ARKXMM_API horizontal_add_sat(vu16x16 a, vu16x16 b) -> vu16x16 { return {_mm256_hadds_epi16(a.v, b.v)}; } // AVX2 -> [a0+a1, a2+a3, ..., b0+b1, b2+b3, ...]

    ARKXMM_API horizontal_sub(vi16x8 a, vi16x8 b) -> vi16x8 { return {_mm_hsub_epi16(a.v, b.v)}; }            // SSSE3 -> [a0-a1, a2-a3, ..., b0-b1, b2-b3, ...]
    ARKXMM_API horizontal_sub(vu16x8 a, vu16x8 b) -> vu16x8 { return {_mm_hsub_epi16(a.v, b.v)}; }            // SSSE3 -> [a0-a1, a2-a3, ..., b0-b1, b2-b3, ...]
    ARKXMM_API horizontal_sub(vi16x16 a, vi16x16 b) -> vi16x16 { return {_mm256_hsub_epi16(a.v, b.v)}; }      // AVX2 -> [a0-a1, a2-a3, ..., b0-b1, b2-b3, ...]
    ARKXMM_API horizontal_sub(vu16x16 a, vu16x16 b) -> vu16x16 { return {_mm256_hsub_epi16(a.v, b.v)}; }      // AVX2 -> [a0-a1, a2-a3, ..., b0-b1, b2-b3, ...]
    ARKXMM_API horizontal_sub(vi32x4 a, vi32x4 b) -> vi32x4 { return {_mm_hsub_epi32(a.v, b.v)}; }            // SSSE3 -> [a0-a1, a2-a3, b0-b1, b2-b3]
    ARKXMM_API horizontal_sub(vu32x4 a, vu32x4 b) -> vu32x4 { return {_mm_hsub_epi32(a.v, b.v)}; }            // SSSE3 -> [a0-a1, a2-a3, b0-b1, b2-b3]
    ARKXMM_API horizontal_sub(vf32x4 a, vf32x4 b) -> vf32x4 { return {_mm_hsub_ps(a.v, b.v)}; }               // SSE3 -> [a0-a1, a2-a3, b0-b1, b2-b3]
    ARKXMM_API horizontal_sub(vi32x8 a, vi32x8 b) -> vi32x8 { return {_mm256_hsub_epi32(a.v, b.v)}; }         // AVX2 -> [a0-a1, a2-a3, b0-b1, b2-b3]
    ARKXMM_API horizontal_sub(vu32x8 a, vu32x8 b) -> vu32x8 { return {_mm256_hsub_epi32(a.v, b.v)}; }         // AVX2 -> [a0-a1, a2-a3, b0-b1, b2-b3]
    ARKXMM_API horizontal_sub(vf32x8 a, vf32x8 b) -> vf32x8 { return {_mm256_hsub_ps(a.v, b.v)}; }            // AVX -> [a0-a1, a2-a3, b0-b1, b2-b3]
    ARKXMM_API horizontal_sub(vf64x2 a, vf64x2 b) -> vf64x2 { return {_mm_hsub_pd(a.v, b.v)}; }               // SSE3 -> [a0-a1, a2-a3, b0-b1, b2-b3]
    ARKXMM_API horizontal_sub(vf64x4 a, vf64x4 b) -> vf64x4 { return {_mm256_hsub_pd(a.v, b.v)}; }            // AVX -> [a0-a1, a2-a3, b0-b1, b2-b3]
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
    ARKXMM_API operator *(vf32x4 a, vf32x4 b) -> vf32x4 { return {_mm_mul_ps(a.v, b.v)}; }                 // SSE
    ARKXMM_API operator *(vi32x8 a, vi32x8 b) -> vi32x8 { return {_mm256_mullo_epi32(a.v, b.v)}; }         // AVX2
    ARKXMM_API operator *(vu32x8 a, vu32x8 b) -> vu32x8 { return {_mm256_mullo_epi32(a.v, b.v)}; }         // AVX2
    ARKXMM_API operator *(vf32x8 a, vf32x8 b) -> vf32x8 { return {_mm256_mul_ps(a.v, b.v)}; }              // AVX
    /* ARKXMM_API operator *(vi64x2 a, vi64x2 b) -> vi64x2 { return { _mm_mullo_epi64(a.v, b.v) }; } */    // AVX512VL + AVX512DQ
    /* ARKXMM_API operator *(vu64x2 a, vu64x2 b) -> vu64x2 { return { _mm_mullo_epi64(a.v, b.v) }; } */    // AVX512VL + AVX512DQ
    ARKXMM_API operator *(vf64x2 a, vf64x2 b) -> vf64x2 { return {_mm_mul_pd(a.v, b.v)}; }                 // SSE2
    /* ARKXMM_API operator *(vi64x4 a, vi64x4 b) -> vi64x4 { return { _mm256_mullo_epi64(a.v, b.v) }; } */ // AVX512VL + AVX512DQ
    /* ARKXMM_API operator *(vu64x4 a, vu64x4 b) -> vu64x4 { return { _mm256_mullo_epi64(a.v, b.v) }; } */ // AVX512VL + AVX512DQ
    ARKXMM_API operator *(vf64x4 a, vf64x4 b) -> vf64x4 { return {_mm256_mul_pd(a.v, b.v)}; }              // AVX

    ARKXMM_API mul_lo(vi16x8 a, vi16x8 b) -> vi16x8 { return {_mm_mullo_epi16(a.v, b.v)}; }         // SSE2
    ARKXMM_API mul_lo(vu16x8 a, vu16x8 b) -> vu16x8 { return {_mm_mullo_epi16(a.v, b.v)}; }         // SSE2
    ARKXMM_API mul_lo(vi16x16 a, vi16x16 b) -> vi16x16 { return {_mm256_mullo_epi16(a.v, b.v)}; }   // AVX2
    ARKXMM_API mul_lo(vu16x16 a, vu16x16 b) -> vu16x16 { return {_mm256_mullo_epi16(a.v, b.v)}; }   // AVX2
    ARKXMM_API mul_hi(vi16x8 a, vi16x8 b) -> vi16x8 { return {_mm_mulhi_epi16(a.v, b.v)}; }         // SSE2
    ARKXMM_API mul_hi(vu16x8 a, vu16x8 b) -> vu16x8 { return {_mm_mulhi_epu16(a.v, b.v)}; }         // SSE2
    ARKXMM_API mul_hi(vi16x16 a, vi16x16 b) -> vi16x16 { return {_mm256_mulhi_epi16(a.v, b.v)}; }   // AVX2
    ARKXMM_API mul_hi(vu16x16 a, vu16x16 b) -> vu16x16 { return {_mm256_mulhi_epu16(a.v, b.v)}; }   // AVX2 
    ARKXMM_API mul_hrs(vi16x8 a, vi16x8 b) -> vi16x8 { return {_mm_mulhrs_epi16(a.v, b.v)}; }       // SSSE3 - with scale [-32768..32767]*[-32768..32767] -> [-32768..32767]
    ARKXMM_API mul_hrs(vi16x16 a, vi16x16 b) -> vi16x16 { return {_mm256_mulhrs_epi16(a.v, b.v)}; } // AVX2 - with scale [-32768..32767]*[-32768..32767] -> [-32768..32767]
    ARKXMM_API mul32x32to64(vi32x4 a, vi32x4 b) -> vi64x2 { return {_mm_mul_epi32(a.v, b.v)}; }     // SSE4.1 -> [a0*b0, a2*b2]
    ARKXMM_API mul32x32to64(vu32x4 a, vu32x4 b) -> vu64x2 { return {_mm_mul_epu32(a.v, b.v)}; }     // SSE2 -> [a0*b0, a2*b2]
    ARKXMM_API mul32x32to64(vi32x8 a, vi32x8 b) -> vi64x4 { return {_mm256_mul_epi32(a.v, b.v)}; }  // AVX2 -> [a0*b0, a2*b2, a4*b4, a6*b6]
    ARKXMM_API mul32x32to64(vu32x8 a, vu32x8 b) -> vu64x4 { return {_mm256_mul_epu32(a.v, b.v)}; }  // AVX2 -> [a0*b0, a2*b2, a4*b4, a6*b6]

    ARKXMM_API mul_hadd(vi16x8 a, vi16x8 b) -> vi32x4 { return {_mm_madd_epi16(a.v, b.v)}; }      // SSE2 -> { i32(a0*b0)+i32(a1*b1), i32(a2*b2)+i32(a3*b3), ..., i32(a6*b6)+i32(a7*b7) }
    ARKXMM_API mul_hadd(vi16x16 a, vi16x16 b) -> vi32x8 { return {_mm256_madd_epi16(a.v, b.v)}; } // AVX2 -> { i32(a0*b0)+i32(a1*b1), i32(a2*b2)+i32(a3*b3), ..., i32(a14*b14)+i32(a15*b15) }

    ARKXMM_API operator /(vf32x4 a, vf32x4 b) -> vf32x4 { return {_mm_div_ps(a.v, b.v)}; }    // SSE
    ARKXMM_API operator /(vf32x8 a, vf32x8 b) -> vf32x8 { return {_mm256_div_ps(a.v, b.v)}; } // AVX
    ARKXMM_API operator /(vf64x2 a, vf64x2 b) -> vf64x2 { return {_mm_div_pd(a.v, b.v)}; }    // SSE2
    ARKXMM_API operator /(vf64x4 a, vf64x4 b) -> vf64x4 { return {_mm256_div_pd(a.v, b.v)}; } // AVX

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
    ARKXMM_API max(vf32x4 a, vf32x4 b) -> vf32x4 { return {_mm_max_ps(a.v, b.v)}; }                    // SSE
    ARKXMM_API max(vi32x8 a, vi32x8 b) -> vi32x8 { return {_mm256_max_epi32(a.v, b.v)}; }              // AVX2
    ARKXMM_API max(vu32x8 a, vu32x8 b) -> vu32x8 { return {_mm256_max_epu32(a.v, b.v)}; }              // AVX2
    ARKXMM_API max(vf32x8 a, vf32x8 b) -> vf32x8 { return {_mm256_max_ps(a.v, b.v)}; }                 // AVX
    /* ARKXMM_API max(vi64x2 a, vi64x2 b) -> vi64x2 { return { _mm_max_epi64(a.v, b.v) }; }         */ // AVX512F + AVX512VL
    /* ARKXMM_API max(vu64x2 a, vu64x2 b) -> vu64x2 { return { _mm_max_epu64(a.v, b.v) }; }         */ // AVX512F + AVX512VL
    ARKXMM_API max(vf64x2 a, vf64x2 b) -> vf64x2 { return {_mm_max_pd(a.v, b.v)}; }                    // SSE2
    /* ARKXMM_API max(vi64x4 a, vi64x4 b) -> vi64x4 { return { _mm256_max_epi64(a.v, b.v) }; }      */ // AVX512F + AVX512VL
    /* ARKXMM_API max(vu64x4 a, vu64x4 b) -> vu64x4 { return { _mm256_max_epu64(a.v, b.v) }; }      */ // AVX512F + AVX512VL
    ARKXMM_API max(vf64x4 a, vf64x4 b) -> vf64x4 { return {_mm256_max_pd(a.v, b.v)}; }                 // AVX

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
    ARKXMM_API min(vf32x4 a, vf32x4 b) -> vf32x4 { return {_mm_min_ps(a.v, b.v)}; }                    // SSE
    ARKXMM_API min(vi32x8 a, vi32x8 b) -> vi32x8 { return {_mm256_min_epi32(a.v, b.v)}; }              // AVX2
    ARKXMM_API min(vu32x8 a, vu32x8 b) -> vu32x8 { return {_mm256_min_epu32(a.v, b.v)}; }              // AVX2
    ARKXMM_API min(vf32x8 a, vf32x8 b) -> vf32x8 { return {_mm256_min_ps(a.v, b.v)}; }                 // AVX
    /* ARKXMM_API min(vi64x2 a, vi64x2 b) -> vi64x2 { return { _mm_min_epi64(a.v, b.v) }; }         */ // AVX512F + AVX512VL
    /* ARKXMM_API min(vu64x2 a, vu64x2 b) -> vu64x2 { return { _mm_min_epu64(a.v, b.v) }; }         */ // AVX512F + AVX512VL
    ARKXMM_API min(vf64x2 a, vf64x2 b) -> vf64x2 { return {_mm_min_pd(a.v, b.v)}; }                    // SSE2
    /* ARKXMM_API min(vi64x4 a, vi64x4 b) -> vi64x4 { return { _mm256_min_epi64(a.v, b.v) }; }      */ // AVX512F + AVX512VL
    /* ARKXMM_API min(vu64x4 a, vu64x4 b) -> vu64x4 { return { _mm256_min_epu64(a.v, b.v) }; }      */ // AVX512F + AVX512VL
    ARKXMM_API min(vf64x4 a, vf64x4 b) -> vf64x4 { return {_mm256_min_pd(a.v, b.v)}; }                 // AVX

    ARKXMM_API sqrt(vf32x4 v) -> vf32x4 { return {_mm_sqrt_ps(v.v)}; }    // SSE
    ARKXMM_API sqrt(vf32x8 v) -> vf32x8 { return {_mm256_sqrt_ps(v.v)}; } // AVX
    ARKXMM_API sqrt(vf64x2 v) -> vf64x2 { return {_mm_sqrt_pd(v.v)}; }    // SSE2
    ARKXMM_API sqrt(vf64x4 v) -> vf64x4 { return {_mm256_sqrt_pd(v.v)}; } // AVX

    ARKXMM_API operator ==(vi8x16 a, vi8x16 b) -> vi8x16 { return {_mm_cmpeq_epi8(a.v, b.v)}; }        // SSE2
    ARKXMM_API operator ==(vu8x16 a, vu8x16 b) -> vu8x16 { return {_mm_cmpeq_epi8(a.v, b.v)}; }        // SSE2
    ARKXMM_API operator ==(vi8x32 a, vi8x32 b) -> vi8x32 { return {_mm256_cmpeq_epi8(a.v, b.v)}; }     // AVX2
    ARKXMM_API operator ==(vu8x32 a, vu8x32 b) -> vu8x32 { return {_mm256_cmpeq_epi8(a.v, b.v)}; }     // AVX2
    ARKXMM_API operator ==(vi16x8 a, vi16x8 b) -> vi16x8 { return {_mm_cmpeq_epi16(a.v, b.v)}; }       // SSE2
    ARKXMM_API operator ==(vu16x8 a, vu16x8 b) -> vu16x8 { return {_mm_cmpeq_epi16(a.v, b.v)}; }       // SSE2
    ARKXMM_API operator ==(vi16x16 a, vi16x16 b) -> vi16x16 { return {_mm256_cmpeq_epi16(a.v, b.v)}; } // AVX2
    ARKXMM_API operator ==(vu16x16 a, vu16x16 b) -> vu16x16 { return {_mm256_cmpeq_epi16(a.v, b.v)}; } // AVX2
    ARKXMM_API operator ==(vi32x4 a, vi32x4 b) -> vi32x4 { return {_mm_cmpeq_epi32(a.v, b.v)}; }       // SSE2
    ARKXMM_API operator ==(vu32x4 a, vu32x4 b) -> vu32x4 { return {_mm_cmpeq_epi32(a.v, b.v)}; }       // SSE2
    ARKXMM_API operator ==(vi32x8 a, vi32x8 b) -> vi32x8 { return {_mm256_cmpeq_epi32(a.v, b.v)}; }    // AVX2
    ARKXMM_API operator ==(vu32x8 a, vu32x8 b) -> vu32x8 { return {_mm256_cmpeq_epi32(a.v, b.v)}; }    // AVX2
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

    template <uint8_t OP> ARKXMM_API compare(vf32x4 a, vf32x4 b) -> vf32x4 { return {_mm_cmp_ps(a.v, b.v, OP)}; }    // AVX
    template <uint8_t OP> ARKXMM_API compare(vf32x8 a, vf32x8 b) -> vf32x8 { return {_mm256_cmp_ps(a.v, b.v, OP)}; } // AVX
    template <uint8_t OP> ARKXMM_API compare(vf64x2 a, vf64x2 b) -> vf64x2 { return {_mm_cmp_pd(a.v, b.v, OP)}; }    // AVX
    template <uint8_t OP> ARKXMM_API compare(vf64x4 a, vf64x4 b) -> vf64x4 { return {_mm256_cmp_pd(a.v, b.v, OP)}; } // AVX
    ARKXMM_API operator ==(vf32x4 a, vf32x4 b) -> vf32x4 { return compare<_CMP_EQ_OQ>(a, b); }                       // AVX
    ARKXMM_API operator ==(vf64x2 a, vf64x2 b) -> vf64x2 { return compare<_CMP_EQ_OQ>(a, b); }                       // AVX
    ARKXMM_API operator ==(vf32x8 a, vf32x8 b) -> vf32x8 { return compare<_CMP_EQ_OQ>(a, b); }                       // AVX
    ARKXMM_API operator ==(vf64x4 a, vf64x4 b) -> vf64x4 { return compare<_CMP_EQ_OQ>(a, b); }                       // AVX
    ARKXMM_API operator !=(vf32x4 a, vf32x4 b) -> vf32x4 { return compare<_CMP_NEQ_UQ>(a, b); }                      // AVX
    ARKXMM_API operator !=(vf64x2 a, vf64x2 b) -> vf64x2 { return compare<_CMP_NEQ_UQ>(a, b); }                      // AVX
    ARKXMM_API operator !=(vf32x8 a, vf32x8 b) -> vf32x8 { return compare<_CMP_NEQ_UQ>(a, b); }                      // AVX
    ARKXMM_API operator !=(vf64x4 a, vf64x4 b) -> vf64x4 { return compare<_CMP_NEQ_UQ>(a, b); }                      // AVX
    ARKXMM_API operator <(vf32x4 a, vf32x4 b) -> vf32x4 { return compare<_CMP_LT_OS>(a, b); }                        // AVX
    ARKXMM_API operator <(vf64x2 a, vf64x2 b) -> vf64x2 { return compare<_CMP_LT_OS>(a, b); }                        // AVX
    ARKXMM_API operator <(vf32x8 a, vf32x8 b) -> vf32x8 { return compare<_CMP_LT_OS>(a, b); }                        // AVX
    ARKXMM_API operator <(vf64x4 a, vf64x4 b) -> vf64x4 { return compare<_CMP_LT_OS>(a, b); }                        // AVX
    ARKXMM_API operator >(vf32x4 a, vf32x4 b) -> vf32x4 { return compare<_CMP_GT_OS>(a, b); }                        // AVX
    ARKXMM_API operator >(vf64x2 a, vf64x2 b) -> vf64x2 { return compare<_CMP_GT_OS>(a, b); }                        // AVX
    ARKXMM_API operator >(vf32x8 a, vf32x8 b) -> vf32x8 { return compare<_CMP_GT_OS>(a, b); }                        // AVX
    ARKXMM_API operator >(vf64x4 a, vf64x4 b) -> vf64x4 { return compare<_CMP_GT_OS>(a, b); }                        // AVX
    ARKXMM_API operator <=(vf32x4 a, vf32x4 b) -> vf32x4 { return compare<_CMP_LE_OS>(a, b); }                       // AVX
    ARKXMM_API operator <=(vf64x2 a, vf64x2 b) -> vf64x2 { return compare<_CMP_LE_OS>(a, b); }                       // AVX
    ARKXMM_API operator <=(vf32x8 a, vf32x8 b) -> vf32x8 { return compare<_CMP_LE_OS>(a, b); }                       // AVX
    ARKXMM_API operator <=(vf64x4 a, vf64x4 b) -> vf64x4 { return compare<_CMP_LE_OS>(a, b); }                       // AVX
    ARKXMM_API operator >=(vf32x4 a, vf32x4 b) -> vf32x4 { return compare<_CMP_GE_OS>(a, b); }                       // AVX
    ARKXMM_API operator >=(vf64x2 a, vf64x2 b) -> vf64x2 { return compare<_CMP_GE_OS>(a, b); }                       // AVX
    ARKXMM_API operator >=(vf32x8 a, vf32x8 b) -> vf32x8 { return compare<_CMP_GE_OS>(a, b); }                       // AVX
    ARKXMM_API operator >=(vf64x4 a, vf64x4 b) -> vf64x4 { return compare<_CMP_GE_OS>(a, b); }                       // AVX

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
    template <class XMM> ARKXMM_API unpack8_lo(XMM l, XMM h) -> enable::if_iXMM<XMM> { return {_mm_unpacklo_epi8(l.v, h.v)}; }      // SSE2 {l0...l15}, {h0...h15} -> {l0,h0,...,l7,h7}
    template <class YMM> ARKXMM_API unpack8_lo(YMM l, YMM h) -> enable::if_iYMM<YMM> { return {_mm256_unpacklo_epi8(l.v, h.v)}; }   // AVX2 {l0...l15|l16...l31}, {h0...h15|h16...h31} -> {l0,h0,...,l7,h7 | l16,h16,...,l23,h23}
    template <class XMM> ARKXMM_API unpack16_lo(XMM l, XMM h) -> enable::if_iXMM<XMM> { return {_mm_unpacklo_epi16(l.v, h.v)}; }    // SSE2 {l0... l7}, {h0... h7} -> {l0,h0,...,l3,h3}
    template <class YMM> ARKXMM_API unpack16_lo(YMM l, YMM h) -> enable::if_iYMM<YMM> { return {_mm256_unpacklo_epi16(l.v, h.v)}; } // AVX2 {l0... l7| l8...l15}, {h0... h7| h8...h15} -> {l0,h0,...,l3,h3 | l8,h8,...,l11,h11}
    template <class XMM> ARKXMM_API unpack32_lo(XMM l, XMM h) -> enable::if_iXMM<XMM> { return {_mm_unpacklo_epi32(l.v, h.v)}; }    // SSE2 {l0... l3}, {h0... h3} -> {l0,h0,l1,h1}
    template <class YMM> ARKXMM_API unpack32_lo(YMM l, YMM h) -> enable::if_iYMM<YMM> { return {_mm256_unpacklo_epi32(l.v, h.v)}; } // AVX2 {l0... l3| l4... l7}, {h0... h3| h4... h7} -> {l0,h0,l1,h1 | l4,h4,l5,h5}
    template <class XMM> ARKXMM_API unpack64_lo(XMM l, XMM h) -> enable::if_iXMM<XMM> { return {_mm_unpacklo_epi64(l.v, h.v)}; }    // SSE2 {l0... l1}, {h0... h1} -> {l0,h0}
    template <class YMM> ARKXMM_API unpack64_lo(YMM l, YMM h) -> enable::if_iYMM<YMM> { return {_mm256_unpacklo_epi64(l.v, h.v)}; } // AVX2 {l0... l1| l2... l3}, {h0... h1| h2... h3} -> {l0,h0|l2,h2}
    template <class XMM> ARKXMM_API unpack8_hi(XMM l, XMM h) -> enable::if_iXMM<XMM> { return {_mm_unpackhi_epi8(l.v, h.v)}; }      // SSE2 {l0...l15}, {h0...h15} -> {l8,h8,...,l15,h15}
    template <class YMM> ARKXMM_API unpack8_hi(YMM l, YMM h) -> enable::if_iYMM<YMM> { return {_mm256_unpackhi_epi8(l.v, h.v)}; }   // AVX2 {l0...l15|l16...l31}, {h0...h15|h16...h31} -> {l7,h7,...,l15,h15 | l24,h24,...,l31,h31}
    template <class XMM> ARKXMM_API unpack16_hi(XMM l, XMM h) -> enable::if_iXMM<XMM> { return {_mm_unpackhi_epi16(l.v, h.v)}; }    // SSE2 {l0... l7}, {h0... h7} -> {l4,h4,...,l7,h7}
    template <class YMM> ARKXMM_API unpack16_hi(YMM l, YMM h) -> enable::if_iYMM<YMM> { return {_mm256_unpackhi_epi16(l.v, h.v)}; } // AVX2 {l0... l7| l8...l15}, {h0... h7| h8...h15} -> {l4,h4,...,l7,h7 | l12,h12,...,l15,h15}
    template <class XMM> ARKXMM_API unpack32_hi(XMM l, XMM h) -> enable::if_iXMM<XMM> { return {_mm_unpackhi_epi32(l.v, h.v)}; }    // SSE2 {l0... l3}, {h0... h3} -> {l2,h2,l3,h3}
    template <class YMM> ARKXMM_API unpack32_hi(YMM l, YMM h) -> enable::if_iYMM<YMM> { return {_mm256_unpackhi_epi32(l.v, h.v)}; } // AVX2 {l0... l3| l4... l7}, {h0... h3| h4... h7} -> {l2,h2,l3,h3 | l6,h6,l7,h7}
    template <class XMM> ARKXMM_API unpack64_hi(XMM l, XMM h) -> enable::if_iXMM<XMM> { return {_mm_unpackhi_epi64(l.v, h.v)}; }    // SSE2 {l0... l1}, {h0... h1} -> {l1,h1}
    template <class YMM> ARKXMM_API unpack64_hi(YMM l, YMM h) -> enable::if_iYMM<YMM> { return {_mm256_unpackhi_epi64(l.v, h.v)}; } // AVX2 {l0... l1| l2... l3}, {h0... h1| h2... h3} -> {l1,h1 | l3,h3}

    template <class XMM> ARKXMM_API unpack_lo(XMM l, XMM h) -> enable::if_8x16<XMM> { return unpack8_lo(l, h); }   // SSE2 {l0...l15}, {h0...h15} -> {l0,h0,...,l7,h7}
    template <class YMM> ARKXMM_API unpack_lo(YMM l, YMM h) -> enable::if_8x32<YMM> { return unpack8_lo(l, h); }   // AVX2 {l0...l15|l16...l31}, {h0...h15|h16...h31} -> {l0,h0,...,l7,h7 | l16,h16,...,l23,h23}
    template <class XMM> ARKXMM_API unpack_lo(XMM l, XMM h) -> enable::if_16x8<XMM> { return unpack16_lo(l, h); }  // SSE2 {l0... l7}, {h0... h7} -> {l0,h0,...,l3,h3}
    template <class YMM> ARKXMM_API unpack_lo(YMM l, YMM h) -> enable::if_16x16<YMM> { return unpack16_lo(l, h); } // AVX2 {l0... l7| l8...l15}, {h0... h7| h8...h15} -> {l0,h0,...,l3,h3 | l8,h8,...,l11,h11}
    template <class XMM> ARKXMM_API unpack_lo(XMM l, XMM h) -> enable::if_32x4<XMM> { return unpack32_lo(l, h); }  // SSE2 {l0... l3}, {h0... h3} -> {l0,h0,l1,h1}
    template <class YMM> ARKXMM_API unpack_lo(YMM l, YMM h) -> enable::if_32x8<YMM> { return unpack32_lo(l, h); }  // AVX2 {l0... l3| l4... l7}, {h0... h3| h4... h7} -> {l0,h0,l1,h1 | l4,h4,l5,h5}
    template <class XMM> ARKXMM_API unpack_lo(XMM l, XMM h) -> enable::if_64x2<XMM> { return unpack64_lo(l, h); }  // SSE2 {l0... l1}, {h0... h1} -> {l0,h0}
    template <class YMM> ARKXMM_API unpack_lo(YMM l, YMM h) -> enable::if_64x4<YMM> { return unpack64_lo(l, h); }  // AVX2 {l0... l1| l2... l3}, {h0... h1| h2... h3} -> {l0,h0|l2,h2}
    template <class XMM> ARKXMM_API unpack_hi(XMM l, XMM h) -> enable::if_8x16<XMM> { return unpack8_hi(l, h); }   // SSE2 {l0...l15}, {h0...h15} -> {l8,h8,...,l15,h15}
    template <class YMM> ARKXMM_API unpack_hi(YMM l, YMM h) -> enable::if_8x32<YMM> { return unpack8_hi(l, h); }   // AVX2 {l0...l15|l16...l31}, {h0...h15|h16...h31} -> {l7,h7,...,l15,h15 | l24,h24,...,l31,h31}
    template <class XMM> ARKXMM_API unpack_hi(XMM l, XMM h) -> enable::if_16x8<XMM> { return unpack16_hi(l, h); }  // SSE2 {l0... l7}, {h0... h7} -> {l4,h4,...,l7,h7}
    template <class YMM> ARKXMM_API unpack_hi(YMM l, YMM h) -> enable::if_16x16<YMM> { return unpack16_hi(l, h); } // AVX2 {l0... l7| l8...l15}, {h0... h7| h8...h15} -> {l4,h4,...,l7,h7 | l12,h12,...,l15,h15}
    template <class XMM> ARKXMM_API unpack_hi(XMM l, XMM h) -> enable::if_32x4<XMM> { return unpack32_hi(l, h); }  // SSE2 {l0... l3}, {h0... h3} -> {l2,h2,l3,h3}
    template <class YMM> ARKXMM_API unpack_hi(YMM l, YMM h) -> enable::if_32x8<YMM> { return unpack32_hi(l, h); }  // AVX2 {l0... l3| l4... l7}, {h0... h3| h4... h7} -> {l2,h2,l3,h3 | l6,h6,l7,h7}
    template <class XMM> ARKXMM_API unpack_hi(XMM l, XMM h) -> enable::if_64x2<XMM> { return unpack64_hi(l, h); }  // SSE2 {l0... l1}, {h0... h1} -> {l1,h1}
    template <class YMM> ARKXMM_API unpack_hi(YMM l, YMM h) -> enable::if_64x4<YMM> { return unpack64_hi(l, h); }  // AVX2 {l0... l1| l2... l3}, {h0... h1| h2... h3} -> {l1,h1 | l3,h3}

    // avx2 permute
    template <class YMM> ARKXMM_API permute32(YMM v, vi32x8 idx) -> enable::if_iYMM<YMM> { return {_mm256_permutevar8x32_epi32(v.v, idx.v)}; }                                                                                                 // AVX2  idx = 0..7
    template <class YMM> ARKXMM_API permute32(YMM v, vi32x8 idx) -> enable::if_f32x8<YMM> { return {_mm256_permutevar8x32_ps(v.v, idx.v)}; }                                                                                                   // AVX2  idx = 0..7
    template <uint8_t i0, uint8_t i1, uint8_t i2, uint8_t i3, uint8_t i4, uint8_t i5, uint8_t i6, uint8_t i7, class YMM> ARKXMM_API permute32(YMM v) -> enable::if_iYMM<YMM> { return permute32(v, i32x8(i0, i1, i2, i3, i4, i5, i6, i7)); }   // AVX2  idx = 0..7
    template <uint8_t i0, uint8_t i1, uint8_t i2, uint8_t i3, uint8_t i4, uint8_t i5, uint8_t i6, uint8_t i7, class YMM> ARKXMM_API permute32(YMM v) -> enable::if_f32x8<YMM> { return permute32(v, i32x8(i0, i1, i2, i3, i4, i5, i6, i7)); }  // AVX2  idx = 0..7
    template <uint8_t i0, uint8_t i1, uint8_t i2, uint8_t i3, class YMM> ARKXMM_API permute64(YMM v) -> enable::if_iYMM<YMM> { return {_mm256_permute4x64_epi64(v.v, (i0 & 0b11) | (i1 & 0b11) << 2 | (i2 & 0b11) << 4 | (i3 & 0b11) << 6)}; } // AVX2  idx = 0..3
    template <uint8_t i0, uint8_t i1, uint8_t i2, uint8_t i3, class YMM> ARKXMM_API permute64(YMM v) -> enable::if_f64x4<YMM> { return {_mm256_permute4x64_pd(v.v, (i0 & 0b11) | (i1 & 0b11) << 2 | (i2 & 0b11) << 4 | (i3 & 0b11) << 6)}; }   // AVX2  idx = 0..3

    template <uint8_t i0, uint8_t i1, class YMM> ARKXMM_API permute128(YMM v) -> enable::if_iYMM<YMM> { return {_mm256_permute4x64_epi64(v.v, ((i0 & 1) * 0b1010 | 0b0100) | ((i1 & 1) * 0b1010 | 0b0100) << 4)}; }                                   // AVX2  idx = 0..1
    template <uint8_t i0, uint8_t i1, class YMM> ARKXMM_API permute128(YMM v) -> enable::if_f32x8<YMM> { return {_mm256_castpd_ps(_mm256_permute4x64_pd(_mm256_castps_pd(v.v), ((i0 & 1) * 0b1010 | 0b0100) | ((i1 & 1) * 0b1010 | 0b0100) << 4))}; } // AVX2  idx = 0..1
    template <uint8_t i0, uint8_t i1, class YMM> ARKXMM_API permute128(YMM v) -> enable::if_f64x4<YMM> { return {_mm256_permute4x64_pd(v.v, ((i0 & 1) * 0b1010 | 0b0100) | ((i1 & 1) * 0b1010 | 0b0100) << 4)}; }                                     // AVX2  idx = 0..1

    template <int8_t i0, int8_t i1, class YMM> ARKXMM_API permute128(YMM a, YMM b) -> enable::if_iYMM<YMM> { return {_mm256_permute2x128_si256(a.v, b.v, (i0 & 0b1111) | (i1 & 0b1111) << 4)}; } // AVX2  idx = 0..3 or -1
    template <int8_t i0, int8_t i1, class YMM> ARKXMM_API permute128(YMM a, YMM b) -> enable::if_f32x8<YMM> { return {_mm256_permute2f128_ps(a.v, b.v, (i0 & 0b1111) | (i1 & 0b1111) << 4)}; }   // AVX2  idx = 0..3
    template <int8_t i0, int8_t i1, class YMM> ARKXMM_API permute128(YMM a, YMM b) -> enable::if_f64x4<YMM> { return {_mm256_permute2f128_pd(a.v, b.v, (i0 & 0b1111) | (i1 & 0b1111) << 4)}; }   // AVX2  idx = 0..3

    template <class YMM> ARKXMM_API lower128(YMM a) -> enable::if_iYMM<YMM, XMM<typename YMM::element_t>> { return {_mm256_castsi256_si128(a.v)}; }       // AVX
    template <class YMM> ARKXMM_API lower128(YMM a) -> enable::if_f32x8<YMM, XMM<typename YMM::element_t>> { return {_mm256_castps256_ps128(a.v)}; }      // AVX
    template <class YMM> ARKXMM_API lower128(YMM a) -> enable::if_f64x4<YMM, XMM<typename YMM::element_t>> { return {_mm256_castpd256_pd128(a.v)}; }      // AVX
    template <class YMM> ARKXMM_API higher128(YMM a) -> enable::if_iYMM<YMM, XMM<typename YMM::element_t>> { return {_mm256_extractf128_si256(a.v, 1)}; } // AVX
    template <class YMM> ARKXMM_API higher128(YMM a) -> enable::if_f32x8<YMM, XMM<typename YMM::element_t>> { return {_mm256_extractf128_ps(a.v, 1)}; }   // AVX
    template <class YMM> ARKXMM_API higher128(YMM a) -> enable::if_f64x4<YMM, XMM<typename YMM::element_t>> { return {_mm256_extractf128_pd(a.v, 1)}; }   // AVX

    // type conversion
    template <class To> ARKXMM_API convert_cast(vi8x16 i8x8) -> enable::if_<To, vi16x8> { return {_mm_cvtepi8_epi16(i8x8.v)}; }    // SSE4.1
    template <class To> ARKXMM_API convert_cast(vi8x16 i8x8) -> enable::if_<To, vu16x8> { return {_mm_cvtepi8_epi16(i8x8.v)}; }    // SSE4.1
    template <class To> ARKXMM_API convert_cast(vu8x16 u8x8) -> enable::if_<To, vi16x8> { return {_mm_cvtepu8_epi16(u8x8.v)}; }    // SSE4.1
    template <class To> ARKXMM_API convert_cast(vu8x16 u8x8) -> enable::if_<To, vu16x8> { return {_mm_cvtepu8_epi16(u8x8.v)}; }    // SSE4.1
    template <class To> ARKXMM_API convert_cast(vi8x16 i8x4) -> enable::if_<To, vi32x4> { return {_mm_cvtepu8_epi32(i8x4.v)}; }    // SSE4.1
    template <class To> ARKXMM_API convert_cast(vi8x16 i8x4) -> enable::if_<To, vu32x4> { return {_mm_cvtepi8_epi32(i8x4.v)}; }    // SSE4.1
    template <class To> ARKXMM_API convert_cast(vu8x16 u8x4) -> enable::if_<To, vi32x4> { return {_mm_cvtepu8_epi32(u8x4.v)}; }    // SSE4.1
    template <class To> ARKXMM_API convert_cast(vu8x16 u8x4) -> enable::if_<To, vu32x4> { return {_mm_cvtepu8_epi32(u8x4.v)}; }    // SSE4.1
    template <class To> ARKXMM_API convert_cast(vi8x16 i8x2) -> enable::if_<To, vi64x2> { return {_mm_cvtepi8_epi64(i8x2.v)}; }    // SSE4.1
    template <class To> ARKXMM_API convert_cast(vi8x16 i8x2) -> enable::if_<To, vu64x2> { return {_mm_cvtepi8_epi64(i8x2.v)}; }    // SSE4.1
    template <class To> ARKXMM_API convert_cast(vu8x16 u8x2) -> enable::if_<To, vi64x2> { return {_mm_cvtepu8_epi64(u8x2.v)}; }    // SSE4.1
    template <class To> ARKXMM_API convert_cast(vu8x16 u8x2) -> enable::if_<To, vu64x2> { return {_mm_cvtepu8_epi64(u8x2.v)}; }    // SSE4.1
    template <class To> ARKXMM_API convert_cast(vi16x8 i16x4) -> enable::if_<To, vi32x4> { return {_mm_cvtepi16_epi32(i16x4.v)}; } // SSE4.1
    template <class To> ARKXMM_API convert_cast(vi16x8 i16x4) -> enable::if_<To, vu32x4> { return {_mm_cvtepi16_epi32(i16x4.v)}; } // SSE4.1
    template <class To> ARKXMM_API convert_cast(vu16x8 u16x4) -> enable::if_<To, vi32x4> { return {_mm_cvtepu16_epi32(u16x4.v)}; } // SSE4.1
    template <class To> ARKXMM_API convert_cast(vu16x8 u16x4) -> enable::if_<To, vu32x4> { return {_mm_cvtepu16_epi32(u16x4.v)}; } // SSE4.1
    template <class To> ARKXMM_API convert_cast(vi16x8 i16x2) -> enable::if_<To, vi64x2> { return {_mm_cvtepi16_epi64(i16x2.v)}; } // SSE4.1
    template <class To> ARKXMM_API convert_cast(vi16x8 i16x2) -> enable::if_<To, vu64x2> { return {_mm_cvtepi16_epi64(i16x2.v)}; } // SSE4.1
    template <class To> ARKXMM_API convert_cast(vu16x8 u16x2) -> enable::if_<To, vi64x2> { return {_mm_cvtepu16_epi64(u16x2.v)}; } // SSE4.1
    template <class To> ARKXMM_API convert_cast(vu16x8 u16x2) -> enable::if_<To, vu64x2> { return {_mm_cvtepu16_epi64(u16x2.v)}; } // SSE4.1
    template <class To> ARKXMM_API convert_cast(vi32x4 i32x2) -> enable::if_<To, vi64x2> { return {_mm_cvtepi32_epi64(i32x2.v)}; } // SSE4.1
    template <class To> ARKXMM_API convert_cast(vi32x4 i32x2) -> enable::if_<To, vu64x2> { return {_mm_cvtepi32_epi64(i32x2.v)}; } // SSE4.1
    template <class To> ARKXMM_API convert_cast(vu32x4 u32x2) -> enable::if_<To, vi64x2> { return {_mm_cvtepu32_epi64(u32x2.v)}; } // SSE4.1
    template <class To> ARKXMM_API convert_cast(vu32x4 u32x2) -> enable::if_<To, vu64x2> { return {_mm_cvtepu32_epi64(u32x2.v)}; } // SSE4.1

    template <class To> ARKXMM_API convert_cast(vi32x4 i32x4) -> enable::if_<To, vf32x4> { return {_mm_cvtepi32_ps(i32x4.v)}; } // SSE2 {a,b,c,d} -> {a,b,c,d}
    template <class To> ARKXMM_API convert_cast(vi32x4 i32x2) -> enable::if_<To, vf64x2> { return {_mm_cvtepi32_pd(i32x2.v)}; } // SSE2 {a,b,_,_} -> {a,b}
    template <class To> ARKXMM_API convert_cast(vf32x4 f32x4) -> enable::if_<To, vi32x4> { return {_mm_cvtps_epi32(f32x4.v)}; } // SSE2 {a,b,c,d} -> {a,b,c,d}
    template <class To> ARKXMM_API convert_cast(vf32x4 f32x4) -> enable::if_<To, vf64x2> { return {_mm_cvtps_pd(f32x4.v)}; }    // SSE2 {a,b,_,_} -> {a,b}
    template <class To> ARKXMM_API convert_cast(vf64x2 f64x2) -> enable::if_<To, vi32x4> { return {_mm_cvtpd_epi32(f64x2.v)}; } // SSE2 {a,b} -> {a,b,0,0}
    template <class To> ARKXMM_API convert_cast(vf64x2 f64x2) -> enable::if_<To, vf32x4> { return {_mm_cvtpd_ps(f64x2.v)}; }    // SSE2 {a,b} -> {a,b,0,0}

    template <class To> ARKXMM_API convert_cast(vi8x16 i8x16) -> enable::if_<To, vi16x16> { return {_mm256_cvtepi8_epi16(i8x16.v)}; } // AVX2
    template <class To> ARKXMM_API convert_cast(vi8x16 i8x16) -> enable::if_<To, vu16x16> { return {_mm256_cvtepi8_epi16(i8x16.v)}; } // AVX2
    template <class To> ARKXMM_API convert_cast(vu8x16 u8x16) -> enable::if_<To, vi16x16> { return {_mm256_cvtepu8_epi16(u8x16.v)}; } // AVX2
    template <class To> ARKXMM_API convert_cast(vu8x16 u8x16) -> enable::if_<To, vu16x16> { return {_mm256_cvtepu8_epi16(u8x16.v)}; } // AVX2
    template <class To> ARKXMM_API convert_cast(vi8x16 i8x8) -> enable::if_<To, vi32x8> { return {_mm256_cvtepu8_epi32(i8x8.v)}; }    // AVX2
    template <class To> ARKXMM_API convert_cast(vi8x16 i8x8) -> enable::if_<To, vu32x8> { return {_mm256_cvtepi8_epi32(i8x8.v)}; }    // AVX2
    template <class To> ARKXMM_API convert_cast(vu8x16 u8x8) -> enable::if_<To, vi32x8> { return {_mm256_cvtepu8_epi32(u8x8.v)}; }    // AVX2
    template <class To> ARKXMM_API convert_cast(vu8x16 u8x8) -> enable::if_<To, vu32x8> { return {_mm256_cvtepu8_epi32(u8x8.v)}; }    // AVX2
    template <class To> ARKXMM_API convert_cast(vi8x16 i8x4) -> enable::if_<To, vi64x4> { return {_mm256_cvtepi8_epi64(i8x4.v)}; }    // AVX2
    template <class To> ARKXMM_API convert_cast(vi8x16 i8x4) -> enable::if_<To, vu64x4> { return {_mm256_cvtepi8_epi64(i8x4.v)}; }    // AVX2
    template <class To> ARKXMM_API convert_cast(vu8x16 u8x4) -> enable::if_<To, vi64x4> { return {_mm256_cvtepu8_epi64(u8x4.v)}; }    // AVX2
    template <class To> ARKXMM_API convert_cast(vu8x16 u8x4) -> enable::if_<To, vu64x4> { return {_mm256_cvtepu8_epi64(u8x4.v)}; }    // AVX2
    template <class To> ARKXMM_API convert_cast(vi16x8 i16x8) -> enable::if_<To, vi32x8> { return {_mm256_cvtepi16_epi32(i16x8.v)}; } // AVX2
    template <class To> ARKXMM_API convert_cast(vi16x8 i16x8) -> enable::if_<To, vu32x8> { return {_mm256_cvtepi16_epi32(i16x8.v)}; } // AVX2
    template <class To> ARKXMM_API convert_cast(vu16x8 u16x8) -> enable::if_<To, vi32x8> { return {_mm256_cvtepu16_epi32(u16x8.v)}; } // AVX2
    template <class To> ARKXMM_API convert_cast(vu16x8 u16x8) -> enable::if_<To, vu32x8> { return {_mm256_cvtepu16_epi32(u16x8.v)}; } // AVX2
    template <class To> ARKXMM_API convert_cast(vi16x8 i16x4) -> enable::if_<To, vi64x4> { return {_mm256_cvtepi16_epi64(i16x4.v)}; } // AVX2
    template <class To> ARKXMM_API convert_cast(vi16x8 i16x4) -> enable::if_<To, vu64x4> { return {_mm256_cvtepi16_epi64(i16x4.v)}; } // AVX2
    template <class To> ARKXMM_API convert_cast(vu16x8 u16x4) -> enable::if_<To, vi64x4> { return {_mm256_cvtepu16_epi64(u16x4.v)}; } // AVX2
    template <class To> ARKXMM_API convert_cast(vu16x8 u16x4) -> enable::if_<To, vu64x4> { return {_mm256_cvtepu16_epi64(u16x4.v)}; } // AVX2
    template <class To> ARKXMM_API convert_cast(vi32x4 i32x4) -> enable::if_<To, vi64x4> { return {_mm256_cvtepi32_epi64(i32x4.v)}; } // AVX2
    template <class To> ARKXMM_API convert_cast(vi32x4 i32x4) -> enable::if_<To, vu64x4> { return {_mm256_cvtepi32_epi64(i32x4.v)}; } // AVX2
    template <class To> ARKXMM_API convert_cast(vu32x4 u32x4) -> enable::if_<To, vi64x4> { return {_mm256_cvtepu32_epi64(u32x4.v)}; } // AVX2
    template <class To> ARKXMM_API convert_cast(vu32x4 u32x4) -> enable::if_<To, vu64x4> { return {_mm256_cvtepu32_epi64(u32x4.v)}; } // AVX2

    template <class To> ARKXMM_API convert_cast(vi32x8 i32x8) -> enable::if_<To, vf32x8> { return {_mm256_cvtepi32_ps(i32x8.v)}; }  // AVX {a,b,c,d|e,f,g,h} -> {a,b,c,d|e,f,g,h}
    template <class To> ARKXMM_API convert_cast(vi32x4 i32x4) -> enable::if_<To, vf64x4> { return {_mm256_cvtepi32_pd(i32x4.v)}; }  // AVX {a,b,c,d} -> {a,b|c,d}
    template <class To> ARKXMM_API convert_cast(vf32x8 f32x8) -> enable::if_<To, vi32x8> { return {_mm256_cvttps_epi32(f32x8.v)}; } // AVX {a,b,c,d|e,f,g,h} -> {a,b,c,d|e,f,g,h}
    template <class To> ARKXMM_API convert_cast(vf32x4 f32x4) -> enable::if_<To, vf64x4> { return {_mm256_cvtps_pd(f32x4.v)}; }     // AVX {a,b,c,d} -> {a,b|c,d}
    template <class To> ARKXMM_API convert_cast(vf64x4 f64x4) -> enable::if_<To, vi32x4> { return {_mm256_cvttpd_epi32(f64x4.v)}; } // AVX {a,b|c,d} -> {a,b,c,d}
    template <class To> ARKXMM_API convert_cast(vf64x4 f64x4) -> enable::if_<To, vf32x4> { return {_mm256_cvtpd_ps(f64x4.v)}; }     // AVX {a,b|c,d} -> {a,b,c,d}

    // avx2 gather
    template <class XMM> ARKXMM_API gather(const typename XMM::element_t* table, vu32x4 idx) -> enable::if_32x4<XMM> { return {_mm_i32gather_epi32(reinterpret_cast<const int32_t*>(table), idx.v, 4)}; }    // returns 4 elements idx{i,j,k,l}->{xi,xj,xk,xl}
    template <class XMM> ARKXMM_API gather(const typename XMM::element_t* table, vu64x2 idx) -> enable::if_32x4<XMM> { return {_mm_i64gather_epi32(reinterpret_cast<const int32_t*>(table), idx.v, 4)}; }    // returns 2 elements idx{i,j} -> {xi,xj,0,0}
    template <class XMM> ARKXMM_API gather(const typename XMM::element_t* table, vu32x4 idx) -> enable::if_f32x4<XMM> { return {_mm_i32gather_ps(reinterpret_cast<const float32_t*>(table), idx.v, 4)}; }    // returns 4 elements idx{i,j,k,l}->{xi,xj,xk,xl}
    template <class XMM> ARKXMM_API gather(const typename XMM::element_t* table, vu64x2 idx) -> enable::if_f32x4<XMM> { return {_mm_i64gather_ps(reinterpret_cast<const float32_t*>(table), idx.v, 4)}; }    // returns 2 elements idx{i,j} -> {xi,xj,0,0}
    template <class XMM> ARKXMM_API gather(const typename XMM::element_t* table, vu32x4 idx) -> enable::if_64x2<XMM> { return {_mm_i32gather_epi64(reinterpret_cast<const int64_t*>(table), idx.v, 8)}; }    // returns 2 elements idx{i,j,_,_} -> {xi,xj}
    template <class XMM> ARKXMM_API gather(const typename XMM::element_t* table, vu64x2 idx) -> enable::if_64x2<XMM> { return {_mm_i64gather_epi64(reinterpret_cast<const int64_t*>(table), idx.v, 8)}; }    // returns 2 elements idx{i,j} -> {xi,xj}
    template <class XMM> ARKXMM_API gather(const typename XMM::element_t* table, vu32x4 idx) -> enable::if_f64x2<XMM> { return {_mm_i32gather_pd(reinterpret_cast<const float64_t*>(table), idx.v, 8)}; }    // returns 2 elements idx{i,j,_,_} -> {xi,xj}
    template <class XMM> ARKXMM_API gather(const typename XMM::element_t* table, vu64x2 idx) -> enable::if_f64x2<XMM> { return {_mm_i64gather_pd(reinterpret_cast<const float64_t*>(table), idx.v, 8)}; }    // returns 2 elements idx{i,j} -> {xi,xj}
    template <class YMM> ARKXMM_API gather(const typename YMM::element_t* table, vu32x8 idx) -> enable::if_32x8<YMM> { return {_mm256_i32gather_epi32(reinterpret_cast<const int32_t*>(table), idx.v, 4)}; } // returns 8 elements idx{i,j,k,l,m,n,o,p} -> {xi,xj,xk,xl,xm,xn,xo,xp}
    template <class YMM> ARKXMM_API gather(const typename YMM::element_t* table, vu64x4 idx) -> enable::if_32x8<YMM> { return {_mm256_i64gather_epi32(reinterpret_cast<const int32_t*>(table), idx.v, 4)}; } // returns 4 elements idx{i,j,k,l} -> {xi,xj,xk,xl,0,0,0,0}
    template <class YMM> ARKXMM_API gather(const typename YMM::element_t* table, vu32x8 idx) -> enable::if_f32x8<YMM> { return {_mm256_i32gather_ps(reinterpret_cast<const float32_t*>(table), idx.v, 4)}; } // returns 8 elements idx{i,j,k,l,m,n,o,p} -> {xi,xj,xk,xl,xm,xn,xo,xp}
    template <class YMM> ARKXMM_API gather(const typename YMM::element_t* table, vu64x4 idx) -> enable::if_f32x8<YMM> { return {_mm256_i64gather_ps(reinterpret_cast<const float32_t*>(table), idx.v, 4)}; } // returns 4 elements idx{i,j,k,l} -> {xi,xj,xk,xl,0,0,0,0}
    template <class YMM> ARKXMM_API gather(const typename YMM::element_t* table, vu32x4 idx) -> enable::if_64x4<YMM> { return {_mm256_i32gather_epi64(reinterpret_cast<const int64_t*>(table), idx.v, 8)}; } // returns 4 elements idx{i,j,k,l,_,_,_,_} -> {xi,xj,xk,xl}
    template <class YMM> ARKXMM_API gather(const typename YMM::element_t* table, vu64x4 idx) -> enable::if_64x4<YMM> { return {_mm256_i64gather_epi64(reinterpret_cast<const int64_t*>(table), idx.v, 8)}; } // returns 4 elements idx{i,j,k,l} -> {xi,xj,xk,xl}
    template <class YMM> ARKXMM_API gather(const typename YMM::element_t* table, vu32x4 idx) -> enable::if_f64x4<YMM> { return {_mm256_i32gather_pd(reinterpret_cast<const float64_t*>(table), idx.v, 8)}; } // returns 4 elements idx{i,j,k,l,_,_,_,_} -> {xi,xj,xk,xl}
    template <class YMM> ARKXMM_API gather(const typename YMM::element_t* table, vu64x4 idx) -> enable::if_f64x4<YMM> { return {_mm256_i64gather_pd(reinterpret_cast<const float64_t*>(table), idx.v, 8)}; } // returns 4 elements idx{i,j,k,l} -> {xi,xj,xk,xl}

    // NTA prefetch
    ARKXMM_API prefetch_nta(const void* p) -> void { return _mm_prefetch(static_cast<const char*>(p), _MM_HINT_NTA); }

    // PCLMULQDQ carry-less integer multiplication
    template <int i0, int i1> ARKXMM_API clmul(vu64x2 a, vu64x2 b) -> vx128x1 { return {_mm_clmulepi64_si128(a.v, b.v, (i0 & 1) | (i1 & 1) << 4)}; } // PCLMULQDQ carry-less integer multiplication

    //// immediate value extensions
    template <class NMM> ARKXMM_API operator &(NMM a, typename NMM::element_t b) -> enable::if_iNMM<NMM> { return a & xmm::broadcast<NMM>(b); };
    template <class NMM> ARKXMM_API operator |(NMM a, typename NMM::element_t b) -> enable::if_iNMM<NMM> { return a | xmm::broadcast<NMM>(b); }
    template <class NMM> ARKXMM_API operator ^(NMM a, typename NMM::element_t b) -> enable::if_iNMM<NMM> { return a ^ xmm::broadcast<NMM>(b); }
    template <class NMM> ARKXMM_API operator +(NMM a, typename NMM::element_t b) -> enable::if_iNMM<NMM> { return a + xmm::broadcast<NMM>(b); };
    template <class NMM> ARKXMM_API operator -(NMM a, typename NMM::element_t b) -> enable::if_iNMM<NMM> { return a - xmm::broadcast<NMM>(b); }
    template <class NMM> ARKXMM_API operator *(NMM a, typename NMM::element_t b) -> enable::if_iNMM<NMM> { return a * xmm::broadcast<NMM>(b); }
    template <class NMM> ARKXMM_API operator ==(NMM a, typename NMM::element_t b) -> enable::if_iNMM<NMM> { return a == xmm::broadcast<NMM>(b); }
    template <class NMM> ARKXMM_API operator <(NMM a, typename NMM::element_t b) -> enable::if_iNMM<NMM> { return a < xmm::broadcast<NMM>(b); }
    template <class NMM> ARKXMM_API operator >(NMM a, typename NMM::element_t b) -> enable::if_iNMM<NMM> { return a > xmm::broadcast<NMM>(b); }
    template <class NMM> ARKXMM_API max(NMM a, typename NMM::element_t b) -> enable::if_iNMM<NMM> { return max(a, xmm::broadcast<NMM>(b)); }
    template <class NMM> ARKXMM_API min(NMM a, typename NMM::element_t b) -> enable::if_iNMM<NMM> { return min(a, xmm::broadcast<NMM>(b)); }

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
}

namespace arkxmm
{
    using namespace arkana::xmm;
}
