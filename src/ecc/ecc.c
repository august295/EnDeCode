#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "endecode/ecc/ecc.h"

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

ECC_Curve_Params* ecc_get_curve_params(int nid)
{
    switch (nid)
    {
    case NID_X9_62_prime256v1:
        return &secp256r1_params;
    case NID_secp256k1:
        return &secp256k1_params;
    default:
        return NULL;
    }
}

// 初始化点
void ecc_point_init(ECPoint* point)
{
    mpz_init(point->x);
    mpz_init(point->y);
    point->infinity = 0;
}

// 初始化椭圆曲线
void ecc_curve_init_set(EllipticCurve* curve, const char* a_str, const char* b_str, const char* p_str, int base)
{
    mpz_init_set_str(curve->a, a_str, base);
    mpz_init_set_str(curve->b, b_str, base);
    mpz_init_set_str(curve->p, p_str, base);
}

// 初始化点
void ecc_point_init_set(ECPoint* point, const char* x_str, const char* y_str, int base)
{
    mpz_init_set_str(point->x, x_str, base);
    mpz_init_set_str(point->y, y_str, base);
    point->infinity = 0;
}

// 预定义设置
void ecc_init_set(EllipticCurve* curve, ECPoint* G, int nid)
{
    ECC_Curve_Params* params = ecc_get_curve_params(nid);
    ecc_curve_init_set(curve, params->a, params->b, params->p, 16);
    ecc_point_init_set(G, params->Gx, params->Gy, 16);
}

// 释放资源-
void ecc_curve_clear(EllipticCurve* curve)
{
    mpz_clear(curve->a);
    mpz_clear(curve->b);
    mpz_clear(curve->p);
}

void ecc_point_clear(ECPoint* point)
{
    mpz_clear(point->x);
    mpz_clear(point->y);
}

// 椭圆曲线点加法
void ecc_point_add(ECPoint* result, ECPoint* P, ECPoint* Q, EllipticCurve* curve)
{
    if (P->infinity)
    {
        mpz_set(result->x, Q->x);
        mpz_set(result->y, Q->y);
        result->infinity = Q->infinity;
        return;
    }

    if (Q->infinity)
    {
        mpz_set(result->x, P->x);
        mpz_set(result->y, P->y);
        result->infinity = P->infinity;
        return;
    }

    mpz_t lambda, temp;
    mpz_inits(lambda, temp, NULL);

    if (mpz_cmp(P->x, Q->x) == 0 && mpz_cmp(P->y, Q->y) == 0)
    {
        // 点倍加：lambda = (3 * x^2 + a) / (2 * y) mod p
        mpz_mul(lambda, P->x, P->x);       // x^2
        mpz_mul_ui(lambda, lambda, 3);     // 3 * x^2
        mpz_add(lambda, lambda, curve->a); // 3 * x^2 + a
        mpz_mul_ui(temp, P->y, 2);         // 2 * y
        mpz_invert(temp, temp, curve->p);  // (2 * y)^(-1) mod p
        mpz_mul(lambda, lambda, temp);     // lambda = (3 * x^2 + a) / (2 * y)
    }
    else
    {
        // 点加：lambda = (y2 - y1) / (x2 - x1) mod p
        mpz_sub(lambda, Q->y, P->y);      // y2 - y1
        mpz_sub(temp, Q->x, P->x);        // x2 - x1
        mpz_invert(temp, temp, curve->p); // (x2 - x1)^(-1) mod p
        mpz_mul(lambda, lambda, temp);    // lambda = (y2 - y1) / (x2 - x1)
    }

    mpz_mod(lambda, lambda, curve->p);

    // x3 = lambda^2 - x1 - x2 mod p
    mpz_mul(result->x, lambda, lambda);  // lambda^2
    mpz_sub(result->x, result->x, P->x); // lambda^2 - x1
    mpz_sub(result->x, result->x, Q->x); // lambda^2 - x1 - x2
    mpz_mod(result->x, result->x, curve->p);

    // y3 = lambda * (x1 - x3) - y1 mod p
    mpz_sub(result->y, P->x, result->x);   // x1 - x3
    mpz_mul(result->y, lambda, result->y); // lambda * (x1 - x3)
    mpz_sub(result->y, result->y, P->y);   // lambda * (x1 - x3) - y1
    mpz_mod(result->y, result->y, curve->p);

    mpz_clears(lambda, temp, NULL);
}

// 椭圆曲线点倍乘 (kP)
void ecc_point_multiply(ECPoint* result, ECPoint* P, mpz_t k, EllipticCurve* curve)
{
    ECPoint R;
    ecc_point_init(&R);
    R.infinity = 1;

    ECPoint Q;
    ecc_point_init(&Q);
    mpz_set(Q.x, P->x);
    mpz_set(Q.y, P->y);

    while (mpz_cmp_ui(k, 0) > 0)
    {
        if (mpz_odd_p(k))
        {
            ecc_point_add(&R, &R, &Q, curve);
        }
        ecc_point_add(&Q, &Q, &Q, curve);
        mpz_fdiv_q_2exp(k, k, 1);
    }

    mpz_set(result->x, R.x);
    mpz_set(result->y, R.y);
    result->infinity = R.infinity;

    ecc_point_clear(&R);
    ecc_point_clear(&Q);
}

// 生成密钥对
void ecc_generate_keypair(ecc_key_st* st, int nid)
{
    ecc_init_set(&st->curve, &st->G, nid);
    ecc_point_init(&st->public_key);
    mpz_init(st->private_key);

    gmp_randstate_t state;
    gmp_randinit_default(state);
    gmp_randseed_ui(state, time(NULL));

    mpz_urandomm(st->private_key, state, st->curve.p);                         // 生成私钥
    ecc_point_multiply(&st->public_key, &st->G, st->private_key, &st->curve); // 生成公钥

    gmp_randclear(state);
}

void ecc_key_st_clear(ecc_key_st* st)
{
    ecc_curve_clear(&st->curve);
    ecc_point_clear(&st->G);
    ecc_point_clear(&st->public_key);
    mpz_clear(st->private_key);
}

/**
 * C1 = kG
 * C2 = P + (k x PublicKey)
 */
int ecc_public_encrypt(ECPoint* C1, ECPoint* C2, const uint8_t* input, uint32_t input_len, ecc_key_st* st)
{
    ecc_point_init(C1);
    ecc_point_init(C2);

    // 将字符串转换为数值
    mpz_t message_as_int;
    mpz_init(message_as_int);
    mpz_import(message_as_int, input_len, 1, 1, 0, 0, input);

    // 映射数值到椭圆曲线点
    ECPoint plaintext_point;
    ecc_point_init(&plaintext_point);
    mpz_set(plaintext_point.x, message_as_int);

    mpz_t k;
    mpz_init(k);

    gmp_randstate_t state;
    gmp_randinit_default(state);
    gmp_randseed_ui(state, time(NULL));
    mpz_urandomm(k, state, st->curve.p);           // 生成随机数 k
    ecc_point_multiply(C1, &st->G, k, &st->curve); // C1 = kG

    ECPoint temp;
    ecc_point_init(&temp);
    ecc_point_multiply(&temp, &st->public_key, k, &st->curve); // temp = k * PublicKey
    ecc_point_add(C2, &plaintext_point, &temp, &st->curve);    // C2 = P + temp

    ecc_point_clear(&temp);
    mpz_clear(k);
    gmp_randclear(state);
    return 0;
}

/**
 * M = C2 - (d x C1)
 */
int ecc_private_decrypt(uint8_t* output, ECPoint* C1, ECPoint* C2, ecc_key_st* st)
{
    ECPoint temp;
    ecc_point_init(&temp);
    ECPoint decrypted_point;
    ecc_point_init(&decrypted_point);

    ecc_point_multiply(&temp, C1, st->private_key, &st->curve); // temp = d * C1
    mpz_neg(temp.y, temp.y);                                     // 求负 temp.y = -temp.y
    mpz_mod(temp.y, temp.y, st->curve.p);
    ecc_point_add(&decrypted_point, C2, &temp, &st->curve); // M = C2 + (-temp)
    ecc_point_clear(&temp);

    // 将解密后的点转换回字符串
    char   decrypted_str[256];
    size_t count = 0;
    mpz_export(output, &count, 1, 1, 0, 0, decrypted_point.x);
    output[count] = '\0';
    return count;
}
