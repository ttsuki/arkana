/// @file
/// @brief	arkana::camellia
///			- An implementation of Camellia cipher
/// @author Copyright(c) 2021 ttsuki
/// 
/// This software is released under the MIT License.
/// https://opensource.org/licenses/MIT
///
/// - camellia: https://info.isl.ntt.co.jp/crypt/camellia/
///
/// This implementation based on
///   "Block Ciphers: Fast Implementations on x86-64 Architecture"
///   -- Oulu : J. Kivilinna, 2013,
///   http://jultika.oulu.fi/Record/nbnfioulu-201305311409

#include "./camellia.h"
#include "./camellia-avx2aesni.h"
#include "../ark/cpuid.h"

namespace arkana::camellia
{
    inline bool cpu_supports_avx2aesni() noexcept
    {
        return cpuid::cpu_supports::AVX2 && cpuid::cpu_supports::AESNI;
    }

    inline void process_blocks_ecb_avx2aesni(void* dst, const void* src, size_t length, const key_vector_small_t& kv)
    {
        return avx2aesni::process_blocks_ecb(dst, src, length, type_punning_cast<const avx2aesni::key_vector_small_t&>(kv));
    }

    inline void process_blocks_ecb_avx2aesni(void* dst, const void* src, size_t length, const key_vector_large_t& kv)
    {
        return avx2aesni::process_blocks_ecb(dst, src, length, type_punning_cast<const avx2aesni::key_vector_large_t&>(kv));
    }

    inline void process_bytes_ctr_avx2aesni(void* dst, const void* src, size_t position, size_t length, const key_vector_small_t& kv, const ctr_vector_t& cv)
    {
        return avx2aesni::process_bytes_ctr(dst, src, position, length, type_punning_cast<const avx2aesni::key_vector_small_t&>(kv), type_punning_cast<const avx2aesni::ctr_vector_t&>(cv));
    }

    inline void process_bytes_ctr_avx2aesni(void* dst, const void* src, size_t position, size_t length, const key_vector_large_t& kv, const ctr_vector_t& cv)
    {
        return avx2aesni::process_bytes_ctr(dst, src, position, length, type_punning_cast<const avx2aesni::key_vector_large_t&>(kv), type_punning_cast<const avx2aesni::ctr_vector_t&>(cv));
    }

    template <class key_vector_t>
    static inline std::unique_ptr<ecb_context_t> make_avx2aesni_ecb_context(key_vector_t kv)
    {
        struct ecb_context_impl_t final : public virtual ecb_context_t
        {
            const key_vector_t key_vector_;
            explicit ecb_context_impl_t(key_vector_t kv) : key_vector_(kv) { }
            ~ecb_context_impl_t() override { secure_be_zero(const_cast<key_vector_t&>(key_vector_)); }
            void process_blocks(void* dst, const void* src, size_t length) override { return process_blocks_ecb_avx2aesni(dst, src, length, key_vector_); }
        };

        return std::make_unique<ecb_context_impl_t>(kv);
    }

    template <class key_vector_t, class ctr_vector_t>
    static inline std::unique_ptr<ctr_context_t> make_avx2aesni_ctr_context(key_vector_t kv, ctr_vector_t cv)
    {
        struct ctr_context_impl_t final : public virtual ctr_context_t
        {
            const key_vector_t key_vector_;
            const ctr_vector_t ctr_vector_;
            explicit ctr_context_impl_t(key_vector_t kv, ctr_vector_t cv) : key_vector_(kv), ctr_vector_(cv) { }
            ~ctr_context_impl_t() override { secure_be_zero(const_cast<key_vector_t&>(key_vector_)), secure_be_zero(const_cast<ctr_vector_t&>(ctr_vector_)); }
            void process_bytes(void* dst, const void* src, size_t position, size_t length) override { return process_bytes_ctr_avx2aesni(dst, src, position, length, key_vector_, ctr_vector_); }
        };

        return std::make_unique<ctr_context_impl_t>(kv, cv);
    }

    std::unique_ptr<ecb_context_t> create_ecb_encrypt_context_avx2aesni(const key_128bit_t* key) { return make_avx2aesni_ecb_context(generate_key_vector_encrypt(key)); }
    std::unique_ptr<ecb_context_t> create_ecb_encrypt_context_avx2aesni(const key_192bit_t* key) { return make_avx2aesni_ecb_context(generate_key_vector_encrypt(key)); }
    std::unique_ptr<ecb_context_t> create_ecb_encrypt_context_avx2aesni(const key_256bit_t* key) { return make_avx2aesni_ecb_context(generate_key_vector_encrypt(key)); }
    std::unique_ptr<ecb_context_t> create_ecb_decrypt_context_avx2aesni(const key_128bit_t* key) { return make_avx2aesni_ecb_context(generate_key_vector_decrypt(key)); }
    std::unique_ptr<ecb_context_t> create_ecb_decrypt_context_avx2aesni(const key_192bit_t* key) { return make_avx2aesni_ecb_context(generate_key_vector_decrypt(key)); }
    std::unique_ptr<ecb_context_t> create_ecb_decrypt_context_avx2aesni(const key_256bit_t* key) { return make_avx2aesni_ecb_context(generate_key_vector_decrypt(key)); }
    std::unique_ptr<ctr_context_t> create_ctr_context_avx2aesni(const key_128bit_t* key, const ctr_iv_t* iv, const ctr_nonce_t* nonce) { return make_avx2aesni_ctr_context(generate_key_vector_encrypt(key), generate_ctr_vector(iv, nonce)); }
    std::unique_ptr<ctr_context_t> create_ctr_context_avx2aesni(const key_192bit_t* key, const ctr_iv_t* iv, const ctr_nonce_t* nonce) { return make_avx2aesni_ctr_context(generate_key_vector_encrypt(key), generate_ctr_vector(iv, nonce)); }
    std::unique_ptr<ctr_context_t> create_ctr_context_avx2aesni(const key_256bit_t* key, const ctr_iv_t* iv, const ctr_nonce_t* nonce) { return make_avx2aesni_ctr_context(generate_key_vector_encrypt(key), generate_ctr_vector(iv, nonce)); }
}
