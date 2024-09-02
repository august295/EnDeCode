#ifndef __ECC_H__
#define __ECC_H__

#include <stdint.h>

#include <gmp.h>

#define NID_X9_62_prime256v1 415 // secp256r1
#define NID_secp256k1        714

// 椭圆曲线结构
typedef struct
{
    mpz_t a;
    mpz_t b;
    mpz_t p; // y^2 = x^3 + ax + b (mod p)
} EllipticCurve;

// 点结构
typedef struct
{
    mpz_t x;
    mpz_t y;
    int   infinity; // 用于表示无穷远点
} ECPoint;

typedef struct ecc_key_st
{
    EllipticCurve curve;
    ECPoint       G;
    ECPoint       public_key;
    mpz_t         private_key;
} ecc_key_st;

#include "endecode/common/endecode_export.hpp"
#ifdef __cplusplus
extern "C"
{
#endif

ENDECODE_API void ecc_generate_keypair(ecc_key_st* st, int nid);

ENDECODE_API void ecc_key_st_clear(ecc_key_st* st);

ENDECODE_API int ecc_public_encrypt(ECPoint* C1, ECPoint* C2, const uint8_t* input, uint32_t input_len, ecc_key_st* st);

ENDECODE_API int ecc_private_decrypt(uint8_t* output, ECPoint* C1, ECPoint* C2, ecc_key_st* st);

#ifdef __cplusplus
}
#endif

#endif