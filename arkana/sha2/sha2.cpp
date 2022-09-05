/// @file
/// @brief	arkana::sha2
///			- An implementation of SHA-1, SHA-2
/// @author Copyright(c) 2021 ttsuki
/// 
/// This software is released under the MIT License.
/// https://opensource.org/licenses/MIT

#include "./sha2.h"
#include "../ark/cpuid.h"

namespace arkana::sha2
{
    std::unique_ptr<md5_context_t> create_md5_context()
    {
        return cpuid::cpu_supports::AVX2
                   ? create_md5_context_avx2()
                   : create_md5_context_ref();
    }

    std::unique_ptr<sha1_context_t> create_sha1_context()
    {
        return cpuid::cpu_supports::AVX2
                   ? create_sha1_context_avx2()
                   : create_sha1_context_ref();
    }

    std::unique_ptr<sha224_context_t> create_sha224_context()
    {
        return cpuid::cpu_supports::AVX2
                   ? create_sha224_context_avx2()
                   : create_sha224_context_ref();
    }

    std::unique_ptr<sha256_context_t> create_sha256_context()
    {
        return cpuid::cpu_supports::AVX2
                   ? create_sha256_context_avx2()
                   : create_sha256_context_ref();
    }

    std::unique_ptr<sha384_context_t> create_sha384_context()
    {
        return cpuid::cpu_supports::AVX2
                   ? create_sha384_context_avx2()
                   : create_sha384_context_ref();
    }

    std::unique_ptr<sha512_context_t> create_sha512_context()
    {
        return cpuid::cpu_supports::AVX2
                   ? create_sha512_context_avx2()
                   : create_sha512_context_ref();
    }

    std::unique_ptr<sha512_224_context_t> create_sha512_224_context()
    {
        return cpuid::cpu_supports::AVX2
                   ? create_sha512_224_context_avx2()
                   : create_sha512_224_context_ref();
    }

    std::unique_ptr<sha512_256_context_t> create_sha512_256_context()
    {
        return cpuid::cpu_supports::AVX2
                   ? create_sha512_256_context_avx2()
                   : create_sha512_256_context_ref();
    }
}
