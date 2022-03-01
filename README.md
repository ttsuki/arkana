arkana
======

C++17 x86/AVX2 accelerated fs-infra library.

---

## arkana.lib

### [arkana::camellia](arkana/camellia.h) - Camellia Encryption Algorithm (ECB-mode: RFC 3713 / CTR-mode: RFC 5528)
  - [camellia.h](arkana/camellia/camellia.h): Reference implementation
  - [camellia-avx2.h](arkana/camellia/camellia-avx2.h): AVX2 LUT accelerated implementation (approx. 2x faster than ref-impl)
  - [camellia-avx2aesni.h](arkana/camellia/camellia-avx2aesni.h): AVX2-AESNI accelerated implementation (based on ["Block Ciphers: Fast Implementations on x86-64 Architecture" -- Oulu : J. Kivilinna, 2013](http://jultika.oulu.fi/Record/nbnfioulu-201305311409))  (approx. 6x faster than ref-impl)

### [arkana::crc32](arkana/crc32.h) - CRC-32 (ISO 3309)
  - [crc32.h](arkana/crc32/crc32.h): Reference implementation
  - [crc32-ia32.h](arkana/crc32/crc32-ia32.h): IA32 loop-unrolling implementation (approx. 6x faster than ref-impl)
  - [crc32-avx2.h](arkana/crc32/crc32-avx2.h): AVX2 LUT accelerated implementation (approx. 7x faster than ref-impl)
  - [crc32-avx2clmul.h](arkana/crc32/crc32-avx2clmul.h): pclmul accelerated implementation (based on ["Fast CRC Computation for Generic Polynomials Using PCLMULQDQ Instruction"  -- V. Gopal, E. Ozturk, J. Guilford, et al., 2009](https://www.intel.com/content/dam/www/public/us/en/documents/white-papers/fast-crc-computation-generic-polynomials-pclmulqdq-paper.pdf)) (approx. 20x faster than ref-impl)

### [arkana::sha2](arkana/sha2.h) - SHA2 (224,256,384,512) (NIST FIPS PUB 180-4)
  - [sha2.h](arkana/sha2/sha2.h): Reference implementation

### [arkana::bits](arkana/bits.h) - helper library
  - [bit-manip.h](arkana/bits/bit-manip.h): Bit-manipulation helper (rotl, rotr, byteswap)
  - [memory.h](arkana/bits/memory.h): Type conversion helper (load_u, store_u, memcpy, memset, bit_cast)
  - [table.h](arkana/bits/table.h): LUT generation helper
  - [cpuid.h](arkana/bits/cpuid.h): CPU feature flag loader
  - [uint128.h](arkana/bits/uint128.h): 128 bit unsigned integer operators (bitwise, arithmetic, comparing operations)
  - [xmm.h](arkana/bits/xmm.h): Typed SSE/AVX integer vector operation wraper library (vu32x4, vu16x16, etc)
  - [hex-int-literals.h](arkana/bits/hex-int-literals.h): Compile-time hexadecimal-integer parser
  - [base64-decoder.h](arkana/bits/base64-decoder.h): Compile-time base64 decoder  

---

[MIT License](LICENSE) Copyright (c) 2020 ttsuki
