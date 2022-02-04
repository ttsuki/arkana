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
#include "./camellia.private.hpp"
#include "./camellia-avx2aesni.h"

std::unique_ptr<arkana::camellia::ecb_context_t> arkana::camellia::create_ecb_context_avx2aesni(const key_128bit_t* key, bool encrypt)
{
    return encrypt
        ? static_cast<std::unique_ptr<ecb_context_t>>(std::make_unique<ecb_context_impl_t<key_128bit_t, avx2aesni::unit_t, avx2aesni::generate_key_vector<128, true>, avx2aesni::process_blocks_ecb<128>>>(key))
        : static_cast<std::unique_ptr<ecb_context_t>>(std::make_unique<ecb_context_impl_t<key_128bit_t, avx2aesni::unit_t, avx2aesni::generate_key_vector<128, false>, avx2aesni::process_blocks_ecb<128>>>(key));
}

std::unique_ptr<arkana::camellia::ecb_context_t> arkana::camellia::create_ecb_context_avx2aesni(const key_192bit_t* key, bool encrypt)
{
    return encrypt
        ? static_cast<std::unique_ptr<ecb_context_t>>(std::make_unique<ecb_context_impl_t<key_192bit_t, avx2aesni::unit_t, avx2aesni::generate_key_vector<192, true>, avx2aesni::process_blocks_ecb<192>>>(key))
        : static_cast<std::unique_ptr<ecb_context_t>>(std::make_unique<ecb_context_impl_t<key_192bit_t, avx2aesni::unit_t, avx2aesni::generate_key_vector<192, false>, avx2aesni::process_blocks_ecb<192>>>(key));
}


std::unique_ptr<arkana::camellia::ecb_context_t> arkana::camellia::create_ecb_context_avx2aesni(const key_256bit_t* key, bool encrypt)
{
    return encrypt
        ? static_cast<std::unique_ptr<ecb_context_t>>(std::make_unique<ecb_context_impl_t<key_256bit_t, avx2aesni::unit_t, avx2aesni::generate_key_vector<256, true>, avx2aesni::process_blocks_ecb<256>>>(key))
        : static_cast<std::unique_ptr<ecb_context_t>>(std::make_unique<ecb_context_impl_t<key_256bit_t, avx2aesni::unit_t, avx2aesni::generate_key_vector<256, false>, avx2aesni::process_blocks_ecb<256>>>(key));
}
