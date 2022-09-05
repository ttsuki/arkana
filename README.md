𝒂𝒓𝒌a𝒏𝒂
======

Somthing of an fs infra library (Intel AVX2 accelerated) for C++17

---

## arkana::ark

### [arkana::ark](arkana/ark.h): A useful infra library collection (header-only library)
  - [bitmanip.h](arkana/ark/bitmanip.h): Bit-manipulation helper (rotl, rotr, byteswap)
  - [memory.h](arkana/ark/memory.h): Type conversion helper (load_u, store_u, memcpy, memset, bit_cast)
  - [cpuid.h](arkana/ark/cpuid.h): CPU feature flag loader
  - [uint128.h](arkana/ark/uint128.h): Unsigned 128 bit integer and bitwise-, arithmetic-, and comparing- operators
  - [xmm.h](arkana/ark/xmm.h): Typed SSE/AVX integer vector operation wraper library (vu32x4, vu16x16, etc)
  - [lutgen.h](arkana/ark/lutgen.h): Compile-time LUT generation helper
  - [hexilit.h](arkana/ark/hexilit.h): Compile-time hexadecimal-integer parser
  - [base64.h](arkana/ark/base64.h): Compile-time base64 decoder  

---

## arkana.lib

### [arkana::camellia](arkana/camellia.h): Camellia Encryption Algorithm (ECB-mode: RFC 3713 / CTR-mode: RFC 5528) 
  - [camellia-ref.h](arkana/camellia/camellia-ref.h): Reference implementation
  - [camellia-avx2.h](arkana/camellia/camellia-avx2.h): AVX2 LUT accelerated implementation (approx. 2x faster than ref-impl)
  - [camellia-avx2aesni.h](arkana/camellia/camellia-avx2aesni.h): AVX2-AESNI accelerated implementation (based on ["Block Ciphers: Fast Implementations on x86-64 Architecture" -- Oulu : J. Kivilinna, 2013](http://jultika.oulu.fi/Record/nbnfioulu-201305311409))  (approx. 6x faster than ref-impl)
### [arkana::crc32](arkana/crc32.h): CRC-32 (ISO 3309)
  - [crc32-ref.h](arkana/crc32/crc32-ref.h): Reference implementation
  - [crc32-ia32.h](arkana/crc32/crc32-ia32.h): IA32 loop-unrolling implementation (approx. 6x faster than ref-impl)
  - [crc32-avx2.h](arkana/crc32/crc32-avx2.h): AVX2 LUT accelerated implementation (approx. 7x faster than ref-impl)
  - [crc32-avx2clmul.h](arkana/crc32/crc32-avx2clmul.h): pclmul accelerated implementation (based on ["Fast CRC Computation for Generic Polynomials Using PCLMULQDQ Instruction"  -- V. Gopal, E. Ozturk, J. Guilford, et al., 2009](https://www.intel.com/content/dam/www/public/us/en/documents/white-papers/fast-crc-computation-generic-polynomials-pclmulqdq-paper.pdf)) (approx. 20x faster than ref-impl)
### [arkana::sha2](arkana/sha2.h): SHA-1, SHA-2(SHA-256,SHA-224,SHA-512,SHA-384,SHA-512/224,SHA-512/256) (NIST FIPS PUB 180-4)
  - [sha2-ref.h](arkana/sha2/sha2-ref.h): Reference implementation
  - [sha2-avx2.h](arkana/sha2/sha2-avx2.h): AVX2 accelerated implementation (based on ["Fast SHA-256 Implementations on Intel® Architecture Processors" -- J. Guilford, K. Yap, V. Gopal, 2012](https://www.intel.com/content/dam/www/public/us/en/documents/white-papers/sha-256-implementations-paper.pdf))

---

## arkana-test

- Tests for arkana.lib (using googletest)

---

## Getting started

### Visual Studio

1. Include `arkana.lib.vcxproj` into your solution.
2. Reference it from your project.

### CMake

Do `cmake`.

---

[MIT License](LICENSE) Copyright (c) 2020-2022 ttsuki
