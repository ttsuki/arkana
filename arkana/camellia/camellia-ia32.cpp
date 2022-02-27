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
#include "./camellia-ia32.h"

namespace arkana::camellia
{
    template <class key_t, class enc_t>
    static inline std::unique_ptr<ecb_context_t> make_ia32_ecb_context(key_t k, enc_t e)
    {
        class ecb_context_impl_t final : public virtual ecb_context_t
        {
        public:
            const ia32::impl::key_vector_for_t<key_t> key_vector_;

            explicit ecb_context_impl_t(key_t key, enc_t enc)
                : key_vector_(ia32::impl::generate_key_vector(key, enc))
            {
            }

            void process_blocks(void* dst, const void* src, size_t length) override
            {
                return ia32::impl::process_blocks_ecb(dst, src, length, key_vector_);
            }
        };

        return std::make_unique<ecb_context_impl_t>(std::move(k), std::move(e));
    }

    template <class key_t, class ctr_t>
    static inline std::unique_ptr<ctr_context_t> make_ia32_ctr_context(key_t k, ctr_t ctr)
    {
        class ctr_context_impl_t final : public virtual ctr_context_t
        {
        public:
            const ia32::impl::key_vector_for_t<key_t> key_vector_;
            const ctr_t ctr_provider_;

            explicit ctr_context_impl_t(key_t key, ctr_t ctr)
                : key_vector_(ia32::impl::generate_key_vector(key, true_t{}))
                , ctr_provider_(std::move(ctr))
            {
            }

            void process_bytes(void* dst, const void* src, size_t position, size_t length) override
            {
                return ia32::impl::process_bytes_ctr(dst, src, position, length, key_vector_, ctr_provider_);
            }
        };

        return std::make_unique<ctr_context_impl_t>(std::move(k), std::move(ctr));
    }

    std::unique_ptr<ecb_context_t> create_ecb_encrypt_context_ia32(const key_128bit_t* key) { return make_ia32_ecb_context(*key, true_t{}); }
    std::unique_ptr<ecb_context_t> create_ecb_encrypt_context_ia32(const key_192bit_t* key) { return make_ia32_ecb_context(*key, true_t{}); }
    std::unique_ptr<ecb_context_t> create_ecb_encrypt_context_ia32(const key_256bit_t* key) { return make_ia32_ecb_context(*key, true_t{}); }
    std::unique_ptr<ecb_context_t> create_ecb_decrypt_context_ia32(const key_128bit_t* key) { return make_ia32_ecb_context(*key, false_t{}); }
    std::unique_ptr<ecb_context_t> create_ecb_decrypt_context_ia32(const key_192bit_t* key) { return make_ia32_ecb_context(*key, false_t{}); }
    std::unique_ptr<ecb_context_t> create_ecb_decrypt_context_ia32(const key_256bit_t* key) { return make_ia32_ecb_context(*key, false_t{}); }
    std::unique_ptr<ctr_context_t> create_ctr_context_ia32(const key_128bit_t* key, const ctr_iv_t* iv, const ctr_nonce_t* nonce) { return make_ia32_ctr_context(*key, ia32::impl::generate_rfc5528_ctr_provider(*iv, *nonce)); }
    std::unique_ptr<ctr_context_t> create_ctr_context_ia32(const key_192bit_t* key, const ctr_iv_t* iv, const ctr_nonce_t* nonce) { return make_ia32_ctr_context(*key, ia32::impl::generate_rfc5528_ctr_provider(*iv, *nonce)); }
    std::unique_ptr<ctr_context_t> create_ctr_context_ia32(const key_256bit_t* key, const ctr_iv_t* iv, const ctr_nonce_t* nonce) { return make_ia32_ctr_context(*key, ia32::impl::generate_rfc5528_ctr_provider(*iv, *nonce)); }
}
