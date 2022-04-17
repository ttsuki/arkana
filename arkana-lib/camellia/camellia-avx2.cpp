/// @file
/// @brief	arkana::camellia
///			- An implementation of Camellia cipher
/// @author Copyright(c) 2021 ttsuki
/// 
/// This software is released under the MIT License.
/// https://opensource.org/licenses/MIT
///
/// - camellia https://info.isl.ntt.co.jp/crypt/camellia/ 

#include "../camellia.h"
#include <arkana/camellia/camellia-avx2.h>

namespace arkana::camellia
{
    template <class key_vector_t>
    static inline std::unique_ptr<ecb_context_t> make_avx2_ecb_context(key_vector_t kv)
    {
        class ecb_context_impl_t final : public virtual ecb_context_t
        {
        public:
            const key_vector_t key_vector_;

            explicit ecb_context_impl_t(key_vector_t kv)
                : key_vector_(kv)
            {
            }

            void process_blocks(void* dst, const void* src, size_t length) override
            {
                return avx2::process_blocks_ecb(dst, src, length, key_vector_);
            }
        };

        return std::make_unique<ecb_context_impl_t>(kv);
    }

    template <class key_vector_t>
    static inline std::unique_ptr<ctr_context_t> make_avx2_ctr_context(key_vector_t kv, ctr_iv_t ctr_iv, ctr_nonce_t ctr_nonce)
    {
        class ctr_context_impl_t final : public virtual ctr_context_t
        {
        public:
            const key_vector_t key_vector_;
            const ctr_iv_t ctr_iv_;
            const ctr_nonce_t ctr_nonce_;

            explicit ctr_context_impl_t(key_vector_t kv, ctr_iv_t ctr_iv, ctr_nonce_t ctr_nonce)
                : key_vector_(kv)
                , ctr_iv_(ctr_iv)
                , ctr_nonce_(ctr_nonce)
            {
            }

            void process_bytes(void* dst, const void* src, size_t position, size_t length) override
            {
                return avx2::process_bytes_ctr(dst, src, position, length, key_vector_, ctr_iv_, ctr_nonce_);
            }
        };

        return std::make_unique<ctr_context_impl_t>(kv, ctr_iv, ctr_nonce);
    }

    std::unique_ptr<ecb_context_t> create_ecb_encrypt_context_avx2(const key_128bit_t* key) { return make_avx2_ecb_context(avx2::generate_key_vector_encrypt(*key)); }
    std::unique_ptr<ecb_context_t> create_ecb_encrypt_context_avx2(const key_192bit_t* key) { return make_avx2_ecb_context(avx2::generate_key_vector_encrypt(*key)); }
    std::unique_ptr<ecb_context_t> create_ecb_encrypt_context_avx2(const key_256bit_t* key) { return make_avx2_ecb_context(avx2::generate_key_vector_encrypt(*key)); }
    std::unique_ptr<ecb_context_t> create_ecb_decrypt_context_avx2(const key_128bit_t* key) { return make_avx2_ecb_context(avx2::generate_key_vector_decrypt(*key)); }
    std::unique_ptr<ecb_context_t> create_ecb_decrypt_context_avx2(const key_192bit_t* key) { return make_avx2_ecb_context(avx2::generate_key_vector_decrypt(*key)); }
    std::unique_ptr<ecb_context_t> create_ecb_decrypt_context_avx2(const key_256bit_t* key) { return make_avx2_ecb_context(avx2::generate_key_vector_decrypt(*key)); }
    std::unique_ptr<ctr_context_t> create_ctr_context_avx2(const key_128bit_t* key, const ctr_iv_t* iv, const ctr_nonce_t* nonce) { return make_avx2_ctr_context(avx2::generate_key_vector_encrypt(*key), *iv, *nonce); }
    std::unique_ptr<ctr_context_t> create_ctr_context_avx2(const key_192bit_t* key, const ctr_iv_t* iv, const ctr_nonce_t* nonce) { return make_avx2_ctr_context(avx2::generate_key_vector_encrypt(*key), *iv, *nonce); }
    std::unique_ptr<ctr_context_t> create_ctr_context_avx2(const key_256bit_t* key, const ctr_iv_t* iv, const ctr_nonce_t* nonce) { return make_avx2_ctr_context(avx2::generate_key_vector_encrypt(*key), *iv, *nonce); }
}
