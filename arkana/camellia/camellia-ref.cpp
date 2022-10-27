/// @file
/// @brief	arkana::camellia
///			- An implementation of Camellia cipher
/// @author Copyright(c) 2021 ttsuki
/// 
/// This software is released under the MIT License.
/// https://opensource.org/licenses/MIT
///
/// - camellia https://info.isl.ntt.co.jp/crypt/camellia/ 

#include "./camellia.h"
#include "./camellia-ref.h"

#include "../ark/memory.h"

namespace arkana::camellia
{
    key_vector_small_t generate_key_vector_encrypt(const key_128bit_t* key) { return bit_cast<key_vector_small_t>(ref::generate_key_vector_encrypt(*key)); }
    key_vector_large_t generate_key_vector_encrypt(const key_192bit_t* key) { return bit_cast<key_vector_large_t>(ref::generate_key_vector_encrypt(*key)); }
    key_vector_large_t generate_key_vector_encrypt(const key_256bit_t* key) { return bit_cast<key_vector_large_t>(ref::generate_key_vector_encrypt(*key)); }
    key_vector_small_t generate_key_vector_decrypt(const key_128bit_t* key) { return bit_cast<key_vector_small_t>(ref::generate_key_vector_decrypt(*key)); }
    key_vector_large_t generate_key_vector_decrypt(const key_192bit_t* key) { return bit_cast<key_vector_large_t>(ref::generate_key_vector_decrypt(*key)); }
    key_vector_large_t generate_key_vector_decrypt(const key_256bit_t* key) { return bit_cast<key_vector_large_t>(ref::generate_key_vector_decrypt(*key)); }
    ctr_vector_t generate_ctr_vector(const ctr_iv_t* iv, const ctr_nonce_t* nonce) { return bit_cast<ctr_vector_t>(ref::generate_ctr_vector(*iv, *nonce)); }
}
