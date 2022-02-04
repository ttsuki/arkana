/// @file
/// @brief	arkana::camellia
///			- An implementation of Camellia cipher
/// @author Copyright(c) 2021 ttsuki
/// 
/// This software is released under the MIT License.
/// https://opensource.org/licenses/MIT
///
/// - camellia https://info.isl.ntt.co.jp/crypt/camellia/ 

#pragma once

#include "./camellia.h"

namespace arkana::camellia
{
    namespace ia32
    {
        template <size_t key_bits> using key_t = impl::key_t<key_bits>;
        template <size_t key_bits> using key_vector_t = impl::key_vector_t<uint64_t, key_bits>;
        using unit_t = impl::uint64_pair_t;

        template <size_t key_bits, bool for_encryption>
        static key_vector_t<key_bits> generate_key_vector(const key_t<key_bits>& key)
        {
            return impl::generate_key_vector_inlined<key_bits, for_encryption>(key);
        }

        template <size_t key_bits>
        static void process_blocks_ecb(unit_t* dst, const unit_t* src, size_t unit_count, const key_vector_t<key_bits>& kv)
        {
            for (size_t i = 0; i < unit_count; i++)
            {
                unit_t b = load_u<unit_t>(src + i);

                b = impl::process_block_inlined<
                    unit_t,
                    uint64_t,
                    impl::camellia_prewhite<unit_t, uint64_t>,
                    impl::camellia_f_table_lookup,
                    impl::camellia_fl<uint64_t, uint64_t, impl::bits::rotl_be1>,
                    impl::camellia_fl_inv<uint64_t, uint64_t, impl::bits::rotl_be1>,
                    impl::camellia_postwhite<unit_t, uint64_t>
                >(b, kv);

                store_u<unit_t>(dst + i, b);
            }
        }
    }
}
