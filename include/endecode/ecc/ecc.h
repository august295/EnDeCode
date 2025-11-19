#ifndef __ECC_H__
#define __ECC_H__

#include <stdint.h>
#include <stdbool.h>

#include <gmp.h>

#include "endecode/ecc/ecc_define.h"

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

#include "endecode/common/endecode_export.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   ecc 椭圆曲线函数
 */
ENDECODE_API void ecc_point_init(ECPoint* point);
ENDECODE_API void ecc_curve_init_set(EllipticCurve* curve, const char* a_str, const char* b_str, const char* p_str, int base);
ENDECODE_API void ecc_point_init_set(ECPoint* point, const char* x_str, const char* y_str, int base);
ENDECODE_API void ecc_init_set(EllipticCurve* curve, ECPoint* G, int nid);
ENDECODE_API void ecc_curve_clear(EllipticCurve* curve);
ENDECODE_API void ecc_point_clear(ECPoint* point);
ENDECODE_API void ecc_point_add(ECPoint* R, ECPoint* P, ECPoint* Q, EllipticCurve* curve);
ENDECODE_API void ecc_scalar_mul(ECPoint* R, ECPoint* P, mpz_t k, EllipticCurve* curve);
ENDECODE_API void ecc_generate_random(mpz_t r, const mpz_t n);
ENDECODE_API bool ecc_point_on_curve(const mpz_t x, const mpz_t y, const mpz_t a, const mpz_t b, const mpz_t p);

/**
 * @brief   ecc 椭圆曲线算法
 */
ENDECODE_API void ecc_generate_keypair(ecc_key_st* st, int nid);

ENDECODE_API void ecc_key_st_clear(ecc_key_st* st);

ENDECODE_API int ecc_public_encrypt(ECPoint* C1, ECPoint* C2, const uint8_t* input, uint32_t input_len, ecc_key_st* st);

ENDECODE_API int ecc_private_decrypt(uint8_t* output, ECPoint* C1, ECPoint* C2, ecc_key_st* st);

#ifdef __cplusplus
}
#endif

#endif