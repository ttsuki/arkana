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

#include "../camellia.h"
#include "./camellia-avx2aesni.h"

namespace arkana::camellia
{
    template <class key_t, class enc_t>
    static inline std::unique_ptr<ecb_context_t> make_avx2aesni_ecb_context(key_t k, enc_t e)
    {
        class ecb_context_impl_t final : public virtual ecb_context_t
        {
        public:
            const avx2aesni::impl::key_vector_for_t<key_t> key_vector_;

            explicit ecb_context_impl_t(key_t key, enc_t enc)
                : key_vector_(avx2aesni::impl::generate_key_vector(key, enc))
            {
            }

            void process_blocks(void* dst, const void* src, size_t length) override
            {
                return avx2aesni::impl::process_blocks_ecb(dst, src, length, key_vector_);
            }
        };

        return std::make_unique<ecb_context_impl_t>(std::move(k), std::move(e));
    }

    template <class key_t, class ctr_t>
    static inline std::unique_ptr<ctr_context_t> make_avx2aesni_ctr_context(key_t k, ctr_t ctr)
    {
        class ctr_context_impl_t final : public virtual ctr_context_t
        {
        public:
            const avx2aesni::impl::key_vector_for_t<key_t> key_vector_;
            const ctr_t ctr_provider_;

            explicit ctr_context_impl_t(key_t key, ctr_t ctr)
                : key_vector_(avx2aesni::impl::generate_key_vector(key, true_t{}))
                , ctr_provider_(std::move(ctr))
            {
            }

            void process_bytes(void* dst, const void* src, size_t position, size_t length) override
            {
                return avx2aesni::impl::process_bytes_ctr(dst, src, position, length, key_vector_, ctr_provider_);
            }
        };

        return std::make_unique<ctr_context_impl_t>(std::move(k), std::move(ctr));
    }

    std::unique_ptr<ecb_context_t> create_ecb_encrypt_context_avx2aesni(const key_128bit_t* key) { return make_avx2aesni_ecb_context(*key, true_t{}); }
    std::unique_ptr<ecb_context_t> create_ecb_encrypt_context_avx2aesni(const key_192bit_t* key) { return make_avx2aesni_ecb_context(*key, true_t{}); }
    std::unique_ptr<ecb_context_t> create_ecb_encrypt_context_avx2aesni(const key_256bit_t* key) { return make_avx2aesni_ecb_context(*key, true_t{}); }
    std::unique_ptr<ecb_context_t> create_ecb_decrypt_context_avx2aesni(const key_128bit_t* key) { return make_avx2aesni_ecb_context(*key, false_t{}); }
    std::unique_ptr<ecb_context_t> create_ecb_decrypt_context_avx2aesni(const key_192bit_t* key) { return make_avx2aesni_ecb_context(*key, false_t{}); }
    std::unique_ptr<ecb_context_t> create_ecb_decrypt_context_avx2aesni(const key_256bit_t* key) { return make_avx2aesni_ecb_context(*key, false_t{}); }
    std::unique_ptr<ctr_context_t> create_ctr_context_avx2aesni(const key_128bit_t* key, const ctr_iv_t* iv, const ctr_nonce_t* nonce) { return make_avx2aesni_ctr_context(*key, avx2aesni::impl::generate_rfc5528_ctr_provider(*iv, *nonce)); }
    std::unique_ptr<ctr_context_t> create_ctr_context_avx2aesni(const key_192bit_t* key, const ctr_iv_t* iv, const ctr_nonce_t* nonce) { return make_avx2aesni_ctr_context(*key, avx2aesni::impl::generate_rfc5528_ctr_provider(*iv, *nonce)); }
    std::unique_ptr<ctr_context_t> create_ctr_context_avx2aesni(const key_256bit_t* key, const ctr_iv_t* iv, const ctr_nonce_t* nonce) { return make_avx2aesni_ctr_context(*key, avx2aesni::impl::generate_rfc5528_ctr_provider(*iv, *nonce)); }
}
