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
#include "./camellia.private.hpp"
#include "./camellia.h"

std::unique_ptr<arkana::camellia::ecb_context_t> arkana::camellia::create_ecb_context_ia32(const key_128bit_t* key, bool encrypt)
{
    return encrypt
               ? static_cast<std::unique_ptr<ecb_context_t>>(std::make_unique<ecb_context_impl_t<key_128bit_t, generate_key_vector<128, true>, process_blocks_ecb<128>>>(key))
               : static_cast<std::unique_ptr<ecb_context_t>>(std::make_unique<ecb_context_impl_t<key_128bit_t, generate_key_vector<128, false>, process_blocks_ecb<128>>>(key));
}

std::unique_ptr<arkana::camellia::ecb_context_t> arkana::camellia::create_ecb_context_ia32(const key_192bit_t* key, bool encrypt)
{
    return encrypt
               ? static_cast<std::unique_ptr<ecb_context_t>>(std::make_unique<ecb_context_impl_t<key_192bit_t, generate_key_vector<192, true>, process_blocks_ecb<192>>>(key))
               : static_cast<std::unique_ptr<ecb_context_t>>(std::make_unique<ecb_context_impl_t<key_192bit_t, generate_key_vector<192, false>, process_blocks_ecb<192>>>(key));
}


std::unique_ptr<arkana::camellia::ecb_context_t> arkana::camellia::create_ecb_context_ia32(const key_256bit_t* key, bool encrypt)
{
    return encrypt
               ? static_cast<std::unique_ptr<ecb_context_t>>(std::make_unique<ecb_context_impl_t<key_256bit_t, generate_key_vector<256, true>, process_blocks_ecb<256>>>(key))
               : static_cast<std::unique_ptr<ecb_context_t>>(std::make_unique<ecb_context_impl_t<key_256bit_t, generate_key_vector<256, false>, process_blocks_ecb<256>>>(key));
}
