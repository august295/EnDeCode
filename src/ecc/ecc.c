#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "endecode/ecc/ecc.h"

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

// 释放资源
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

void ecc_point_set(ECPoint* R, ECPoint* A)
{
    if (A->infinity)
    {
        R->infinity = 1;
        return;
    }
    mpz_set(R->x, A->x);
    mpz_set(R->y, A->y);
    R->infinity = 0;
}

// r = a mod m, but ensure r in [0, m-1]
void mod_pos(mpz_t r, const mpz_t a, const mpz_t m)
{
    mpz_fdiv_r(r, a, m); // remainder
    if (mpz_sgn(r) < 0)
        mpz_add(r, r, m);
}

// compute inv = a^{-1} mod p, return 1 on success, 0 on failure
int inv_mod(mpz_t inv, const mpz_t a, const mpz_t p)
{
    // mpz_invert returns nonzero on success
    return mpz_invert(inv, a, p) != 0;
}

// 椭圆曲线点加法
void ecc_point_add(ECPoint* R, ECPoint* P, ECPoint* Q, EllipticCurve* curve)
{
    if (P->infinity)
    {
        mpz_set(R->x, Q->x);
        mpz_set(R->y, Q->y);
        R->infinity = Q->infinity;
        return;
    }

    if (Q->infinity)
    {
        mpz_set(R->x, P->x);
        mpz_set(R->y, P->y);
        R->infinity = P->infinity;
        return;
    }

    // If P == Q -> doubling
    if (mpz_cmp(P->x, Q->x) == 0)
    {
        if (mpz_cmp(P->y, Q->y) != 0 || mpz_sgn(P->y) == 0)
        { // P == -Q or y == 0 => infinity
            R->infinity = 1;
            return;
        }
        // else handle doubling below by separate function, here fallthrough to doubling code
    }

    // If P != Q
    if (mpz_cmp(P->x, Q->x) != 0)
    {
        mpz_t lambda, num, den, tmp;
        mpz_inits(lambda, num, den, tmp, NULL);

        // lambda = (y2 - y1) * (x2 - x1)^{-1} mod p
        mpz_sub(num, Q->y, P->y);
        mod_pos(num, num, curve->p);
        mpz_sub(den, Q->x, P->x);
        mod_pos(den, den, curve->p);

        if (!inv_mod(den, den, curve->p))
        { // no inverse => points add to infinity (shouldn't happen if p prime and x1!=x2)
            R->infinity = 1;
            mpz_clears(lambda, num, den, tmp, NULL);
            return;
        }
        mpz_mul(lambda, num, den);
        mod_pos(lambda, lambda, curve->p);

        // x3 = lambda^2 - x1 - x2 mod p
        mpz_pow_ui(tmp, lambda, 2);
        mod_pos(tmp, tmp, curve->p);
        mpz_sub(tmp, tmp, P->x);
        mpz_sub(tmp, tmp, Q->x);
        mod_pos(R->x, tmp, curve->p);

        // y3 = lambda*(x1 - x3) - y1 mod p
        mpz_sub(tmp, P->x, R->x);
        mpz_mul(tmp, lambda, tmp);
        mpz_sub(tmp, tmp, P->y);
        mod_pos(R->y, tmp, curve->p);

        R->infinity = 0;
        mpz_clears(lambda, num, den, tmp, NULL);
        return;
    }

    // Here x1 == x2 and y1 == y2 -> doubling
    // Use doubling formula
    {
        mpz_t lambda, num, den, tmp;
        mpz_inits(lambda, num, den, tmp, NULL);

        // lambda = (3*x1^2 + a) * (2*y1)^{-1} mod p
        mpz_pow_ui(num, P->x, 2);    // x1^2
        mpz_mul_ui(num, num, 3);     // 3*x1^2
        mpz_add(num, num, curve->a); // 3*x1^2 + a
        mod_pos(num, num, curve->p);

        mpz_mul_ui(den, P->y, 2);    // 2*y1
        mod_pos(den, den, curve->p);

        if (mpz_sgn(den) == 0 || !inv_mod(den, den, curve->p))
        {
            // tangent is vertical -> infinity
            R->infinity = 1;
            mpz_clears(lambda, num, den, tmp, NULL);
            return;
        }

        mpz_mul(lambda, num, den);
        mod_pos(lambda, lambda, curve->p);

        // x3 = lambda^2 - 2*x1
        mpz_pow_ui(tmp, lambda, 2);
        mod_pos(tmp, tmp, curve->p);
        mpz_submul_ui(tmp, P->x, 2); // tmp = tmp - 2*x1
        mod_pos(R->x, tmp, curve->p);

        // y3 = lambda*(x1 - x3) - y1
        mpz_sub(tmp, P->x, R->x);
        mpz_mul(tmp, lambda, tmp);
        mpz_sub(tmp, tmp, P->y);
        mod_pos(R->y, tmp, curve->p);

        R->infinity = 0;
        mpz_clears(lambda, num, den, tmp, NULL);
        return;
    }
}

// 椭圆曲线点倍乘 (kP)
// Scalar multiplication R = k * P using left-to-right binary double-and-add
void ecc_scalar_mul(ECPoint* R, ECPoint* P, mpz_t k, EllipticCurve* curve)
{
    ECPoint Q;
    ecc_point_init(&Q);
    Q.infinity = 1;
    ECPoint tmp;
    ecc_point_init(&tmp);

    mpz_t kk;
    mpz_init_set(kk, k);

    // If k == 0 -> infinity
    if (mpz_sgn(kk) == 0)
    {
        R->infinity = 1;
        goto done;
    }

    // find bit length
    size_t nbits = mpz_sizeinbase(kk, 2);

    for (int64_t i = nbits - 1; i >= 0; --i)
    {
        // Q = 2*Q
        ecc_point_add(&tmp, &Q, &Q, curve);
        ecc_point_set(&Q, &tmp);

        if (mpz_tstbit(kk, i))
        {
            // Q = Q + P
            ecc_point_add(&tmp, &Q, (ECPoint*)P, curve);
            ecc_point_set(&Q, &tmp);
        }
    }

    ecc_point_set(R, &Q);

done:
    ecc_point_clear(&Q);
    ecc_point_clear(&tmp);
    mpz_clear(kk);
}

// 生成随机数
void ecc_generate_random(mpz_t r, const mpz_t n)
{
    gmp_randstate_t state;
    gmp_randinit_default(state);
    gmp_randseed_ui(state, time(NULL));

    mpz_urandomm(r, state, n);

    gmp_randclear(state);
}

// 生成密钥对
void ecc_generate_keypair(ecc_key_st* st, int nid)
{
    ecc_init_set(&st->curve, &st->G, nid);
    ecc_point_init(&st->public_key);
    mpz_init(st->private_key);

    ecc_generate_random(st->private_key, st->curve.p);                    // 生成私钥
    ecc_scalar_mul(&st->public_key, &st->G, st->private_key, &st->curve); // 生成公钥
}

void ecc_key_st_clear(ecc_key_st* st)
{
    ecc_curve_clear(&st->curve);
    ecc_point_clear(&st->G);
    ecc_point_clear(&st->public_key);
    mpz_clear(st->private_key);
}

bool ecc_point_on_curve(const mpz_t x, const mpz_t y, const mpz_t a, const mpz_t b, const mpz_t p)
{
    // 1. 检查范围
    if (mpz_sgn(x) < 0 || mpz_cmp(x, p) >= 0)
        return false;
    if (mpz_sgn(y) < 0 || mpz_cmp(y, p) >= 0)
        return false;

    mpz_t left, right, t;
    mpz_inits(left, right, t, NULL);

    // left = y^2 mod p
    mpz_mul(left, y, y);
    mpz_mod(left, left, p);

    // right = x^3 + a*x + b mod p
    mpz_mul(right, x, x);     // x^2
    mpz_mul(right, right, x); // x^3
    mpz_mod(right, right, p);

    // t = a*x mod p
    mpz_mul(t, a, x);
    mpz_mod(t, t, p);
    mpz_add(right, right, t);
    mpz_mod(right, right, p);

    // right = right + b mod p
    mpz_add(right, right, b);
    mpz_mod(right, right, p);

    // compare
    bool ok = (mpz_cmp(left, right) == 0);

    mpz_clears(left, right, t, NULL);

    return ok;
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

    ecc_generate_random(k, st->curve.p);       // 生成随机数 k
    ecc_scalar_mul(C1, &st->G, k, &st->curve); // C1 = kG

    ECPoint temp;
    ecc_point_init(&temp);
    ecc_scalar_mul(&temp, &st->public_key, k, &st->curve);  // temp = k * PublicKey
    ecc_point_add(C2, &plaintext_point, &temp, &st->curve); // C2 = P + temp

    // 释放资源
    mpz_clear(message_as_int);
    ecc_point_clear(&plaintext_point);
    ecc_point_clear(&temp);
    mpz_clear(k);
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

    ecc_scalar_mul(&temp, C1, st->private_key, &st->curve); // temp = d * C1
    mpz_neg(temp.y, temp.y);                                // 求负 temp.y = -temp.y
    mpz_mod(temp.y, temp.y, st->curve.p);
    ecc_point_add(&decrypted_point, C2, &temp, &st->curve); // M = C2 + (-temp)

    // 将解密后的点转换回字符串
    size_t count = 0;
    mpz_export(output, &count, 1, 1, 0, 0, decrypted_point.x);
    output[count] = '\0';

    // 释放资源
    ecc_point_clear(&temp);
    ecc_point_clear(&decrypted_point);

    return count;
}
