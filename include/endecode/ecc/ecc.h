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

// 预定义椭圆曲线参数
typedef struct
{
    const char* name;
    const char* p;
    const char* a;
    const char* b;
    const char* Gx;
    const char* Gy;
    const char* n; // 曲线的阶
    int         h; // 余因子
} ECC_Curve_Params;

static ECC_Curve_Params secp256k1_params = {
    .name = "secp256k1",
    .p    = "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F",
    .a    = "0",
    .b    = "7",
    .Gx   = "79BE667EF9DCBBAC55A06295CE870B07029BFCDB2DCE28D959F2815B16F81798",
    .Gy   = "483ADA7726A3C4655DA4FBFC0E1108A8FD17B448A68554199C47D08FFB10D4B8",
    .n    = "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEBAAEDCE6AF48A03BBFD25E8CD0364141",
    .h    = 1,
};

static ECC_Curve_Params secp256r1_params = {
    .name = "secp256r1",
    .p    = "FFFFFFFF00000001000000000000000000000000FFFFFFFFFFFFFFFFFFFFFFFF",
    .a    = "FFFFFFFF00000001000000000000000000000000FFFFFFFFFFFFFFFFFFFFFFFC",
    .b    = "5AC635D8AA3A93E7B3EBBD55769886BC651D06B0CC53B0F63BCE3C3E27D2604B",
    .Gx   = "6B17D1F2E12C4247F8BCE6E563A440F277037D812DEB33A0F4A13945D898C296",
    .Gy   = "4FE342E2FE1A7F9B8EE7EB4A7C0F9E162BCE33576B315ECECBB6406837BF51F5",
    .n    = "FFFFFFFF00000000FFFFFFFFFFFFFFFFBCE6FAADA7179E84F3B9CAC2FC632551",
    .h    = 1,
};

#include "endecode/common/endecode_export.hpp"
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