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
#include "./camellia-avx2.h"

namespace arkana::camellia
{
    template <class key_t, class enc_t>
    static inline std::unique_ptr<ecb_context_t> make_avx2_ecb_context(key_t k, enc_t e)
    {
        class ecb_context_impl_t final : public virtual ecb_context_t
        {
        public:
            const avx2::impl::key_vector_for_t<key_t> key_vector_;
            explicit ecb_context_impl_t(key_t key, enc_t enc) : key_vector_(avx2::impl::generate_key_vector(key, enc)) { }
            void process_blocks(void* dst, const void* src, size_t length) override { return avx2::impl::process_blocks_ecb(dst, src, length, key_vector_); }
        };

        return std::make_unique<ecb_context_impl_t>(std::move(k), std::move(e));
    }

    std::unique_ptr<ecb_context_t> create_ecb_encrypt_context_avx2(const key_128bit_t* key) { return make_avx2_ecb_context(*key, true_t{}); }
    std::unique_ptr<ecb_context_t> create_ecb_encrypt_context_avx2(const key_192bit_t* key) { return make_avx2_ecb_context(*key, true_t{}); }
    std::unique_ptr<ecb_context_t> create_ecb_encrypt_context_avx2(const key_256bit_t* key) { return make_avx2_ecb_context(*key, true_t{}); }
    std::unique_ptr<ecb_context_t> create_ecb_decrypt_context_avx2(const key_128bit_t* key) { return make_avx2_ecb_context(*key, false_t{}); }
    std::unique_ptr<ecb_context_t> create_ecb_decrypt_context_avx2(const key_192bit_t* key) { return make_avx2_ecb_context(*key, false_t{}); }
    std::unique_ptr<ecb_context_t> create_ecb_decrypt_context_avx2(const key_256bit_t* key) { return make_avx2_ecb_context(*key, false_t{}); }
}
