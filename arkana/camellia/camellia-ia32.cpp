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
#include "./camellia-ia32.h"

std::unique_ptr<arkana::camellia::ecb_context_t> arkana::camellia::create_ecb_context_ia32(const key_128bit_t* key, bool encrypt)
{
    return encrypt
               ? static_cast<std::unique_ptr<ecb_context_t>>(std::make_unique<ecb_context_impl_t<key_128bit_t, ia32::unit_t, ia32::generate_key_vector<128, true>, ia32::process_blocks_ecb<128>>>(key))
               : static_cast<std::unique_ptr<ecb_context_t>>(std::make_unique<ecb_context_impl_t<key_128bit_t, ia32::unit_t, ia32::generate_key_vector<128, false>, ia32::process_blocks_ecb<128>>>(key));
}

std::unique_ptr<arkana::camellia::ecb_context_t> arkana::camellia::create_ecb_context_ia32(const key_192bit_t* key, bool encrypt)
{
    return encrypt
        ? static_cast<std::unique_ptr<ecb_context_t>>(std::make_unique<ecb_context_impl_t<key_192bit_t, ia32::unit_t, ia32::generate_key_vector<192, true>, ia32::process_blocks_ecb<192>>>(key))
        : static_cast<std::unique_ptr<ecb_context_t>>(std::make_unique<ecb_context_impl_t<key_192bit_t, ia32::unit_t, ia32::generate_key_vector<192, false>, ia32::process_blocks_ecb<192>>>(key));
}


std::unique_ptr<arkana::camellia::ecb_context_t> arkana::camellia::create_ecb_context_ia32(const key_256bit_t* key, bool encrypt)
{
    return encrypt
        ? static_cast<std::unique_ptr<ecb_context_t>>(std::make_unique<ecb_context_impl_t<key_256bit_t, ia32::unit_t, ia32::generate_key_vector<256, true>, ia32::process_blocks_ecb<256>>>(key))
        : static_cast<std::unique_ptr<ecb_context_t>>(std::make_unique<ecb_context_impl_t<key_256bit_t, ia32::unit_t, ia32::generate_key_vector<256, false>, ia32::process_blocks_ecb<256>>>(key));
}