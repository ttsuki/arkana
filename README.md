arkana
======

C++17 x86/AVX2 accelerated fs-infra library.

* Camellia Encryption Algorithm (RFC 3713)
  - reference implementation
  - AVX2 LUT accelerated implementation
  - AVX2-AESNI accelerated implementation (based on ["Block Ciphers: Fast Implementations on x86-64 Architecture" -- Oulu : J. Kivilinna, 2013](http://jultika.oulu.fi/Record/nbnfioulu-201305311409))
* CRC-32 (ISO 3309)
  - reference implementation
  - ia32 unrolling implementation
  - AVX2 LUT accelerated implementation
  - pclmul accelerated implementation (based on ["Fast CRC Computation for Generic Polynomials Using PCLMULQDQ Instruction"  -- V. Gopal, E. Ozturk, J. Guilford, et al., 2009](https://www.intel.com/content/dam/www/public/us/en/documents/white-papers/fast-crc-computation-generic-polynomials-pclmulqdq-paper.pdf))
* SHA2 (224,256,384,512) (NIST FIPS PUB 180-4)
  - reference implementation

