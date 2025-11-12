#include "endecode/sm2/sm2.h"
#include "endecode/sm3/sm3.h"

// 预定义设置
void sm2_init_set(EllipticCurve* curve, ECPoint* G)
{
    ECC_Curve_Params* params = &sm2_params;
    ecc_curve_init_set(curve, params->a, params->b, params->p, 16);
    ecc_point_init_set(G, params->Gx, params->Gy, 16);
}

// 生成密钥对
void sm2_generate_keypair(ecc_key_st* st)
{
    sm2_init_set(&st->curve, &st->G);
    ecc_point_init(&st->public_key);
    mpz_init(st->private_key);

    gmp_randstate_t state;
    gmp_randinit_default(state);
    gmp_randseed_ui(state, time(NULL));

    mpz_urandomm(st->private_key, state, st->curve.p);                    // 生成私钥
    ecc_scalar_mul(&st->public_key, &st->G, st->private_key, &st->curve); // 生成公钥

    gmp_randclear(state);
}

void sm2_key_st_clear(ecc_key_st* st)
{
    ecc_curve_clear(&st->curve);
    ecc_point_clear(&st->G);
    ecc_point_clear(&st->public_key);
    mpz_clear(st->private_key);
}

void sm2_sign(
    const uint8_t* msg,
    size_t         msg_len,
    const uint8_t* id,
    size_t         id_len,
    ecc_key_st*    st,
    mpz_t          r,
    mpz_t          s)
{
    // Step 1: M' = Za || M
    // Step 2: e = SM3(M')
    uint8_t digest[32];
    uint8_t pubkey[64];
    mpz_export(pubkey, NULL, 1, 1, 1, 0, st->public_key.x);
    mpz_export(pubkey + 32, NULL, 1, 1, 1, 0, st->public_key.y);

    sm3_z(id, id_len, pubkey, 64, msg, msg_len, digest);

    mpz_t e;
    mpz_init(e);
    mpz_import(e, 32, 1, 1, 1, 0, digest);
#ifdef CONSOLE_PRINT
    gmp_printf("e = %ZX\n", e);
#endif

    mpz_t k;
    mpz_init(k);
    mpz_t tmp;
    mpz_init(tmp);
    ECPoint kG;
    ecc_point_init(&kG);
    mpz_t n;
    mpz_init_set_str(n, sm2_params.n, 16);

    while (1)
    {
        // Step 3: generate random k in [1, n-1]
        gmp_randstate_t state;
        gmp_randinit_default(state);
        gmp_randseed_ui(state, time(NULL));
        mpz_urandomm(k, state, n);
        // mpz_set_str(k, "59276E27D506861A16680F3AD9C02DCCEF3CC1FA3CDBE4CE6D54B80DEAC1BC21", 16);
        if (mpz_cmp_ui(k, 1) < 0 || mpz_cmp(k, n) >= 0)
        {
            continue;
        }

        // Step 4: (x1, y1) = [k]G
        ECPoint G = {0};
        ecc_point_init(&G);
        mpz_set(G.x, st->G.x);
        mpz_set(G.y, st->G.y);
        ecc_scalar_mul(&kG, &G, k, &st->curve);
#ifdef CONSOLE_PRINT
        gmp_printf("k = %ZX\n", k);
        gmp_printf("kx = %ZX\n", kG.x);
        gmp_printf("ky = %ZX\n", kG.y);
#endif

        // Step 5: r = (e + x1) mod n
        mpz_add(r, e, kG.x);
        mpz_mod(r, r, n);
        // check r != 0
        if (mpz_cmp_ui(r, 0) == 0)
            continue;
        // check r + k = n
        mpz_add(tmp, r, k);
        mpz_mod(tmp, tmp, n);
        if (mpz_cmp_ui(tmp, 0) == 0)
            continue;
#ifdef CONSOLE_PRINT
        gmp_printf("r = %ZX\n", r);
#endif

        // Step 6: s = ((1 + da)^-1 * (k - r * da)) mod n
        mpz_t t1;
        mpz_init(t1);
        mpz_t t2;
        mpz_init(t2);
        mpz_t inv;
        mpz_init(inv);

        mpz_add_ui(t1, st->private_key, 1);
        mpz_mod(t1, t1, n);     // (1 + dA) mod n

        mpz_mul(tmp, r, st->private_key);
        mpz_mod(tmp, tmp, n);   // (r * dA) mod n

        mpz_sub(t2, k, tmp);
        mpz_mod(t2, t2, n);     // (k - r*dA) mod n

        mpz_invert(inv, t1, n); // ((1 + dA)^-1) mod n

        mpz_mul(s, inv, t2);
        mpz_mod(s, s, n);       // 最终 mod n

        // check s != 0
        if (mpz_cmp_ui(s, 0) == 0)
            continue;
#ifdef CONSOLE_PRINT
        gmp_printf("s = %ZX\n", s);
#endif
        break;
    }

    ecc_point_clear(&kG);
    mpz_clears(k, tmp, e, n, NULL);
}

int sm2_verify(
    const uint8_t* msg,
    size_t         msg_len,
    const uint8_t* id,
    size_t         id_len,
    ecc_key_st*    st,
    mpz_t          r,
    mpz_t          s)
{
    mpz_t n;
    mpz_init_set_str(n, sm2_params.n, 16);
    // Step 1+2: r and s E [1, n-1]
    if (mpz_cmp_ui(r, 1) < 0 || mpz_cmp(r, n) >= 0)
        return 0;
    if (mpz_cmp_ui(s, 1) < 0 || mpz_cmp(s, n) >= 0)
        return 0;

    // Step 3+4: e = sm3_z(id, pubkey, msg)
    uint8_t digest[32];
    uint8_t pubkey[64];
    mpz_export(pubkey, NULL, 1, 1, 1, 0, st->public_key.x);
    mpz_export(pubkey + 32, NULL, 1, 1, 1, 0, st->public_key.y);
    sm3_z(id, id_len, pubkey, 64, msg, msg_len, digest);

    mpz_t e;
    mpz_init(e);
    mpz_import(e, 32, 1, 1, 1, 0, digest);
#ifdef CONSOLE_PRINT
    gmp_printf("e = %ZX\n", e);
#endif

    // Step 5: t = (r + s) mod n
    mpz_t t;
    mpz_init(t);
    mpz_add(t, r, s);
    mpz_mod(t, t, n);
    if (mpz_cmp_ui(t, 0) == 0)
    {
        mpz_clears(e, t, NULL);
        return 0;
    }
#ifdef CONSOLE_PRINT
    gmp_printf("t = %ZX\n", t);
#endif

    // Step 6: (x1, y1) = [s]G + [t]PA
    ECPoint G;
    ecc_point_init(&G);
    mpz_set(G.x, st->G.x);
    mpz_set(G.y, st->G.y);

    ECPoint sG, tPA, R;
    ecc_point_init(&sG);
    ecc_point_init(&tPA);
    ecc_point_init(&R);

    ecc_scalar_mul(&sG, &G, s, &st->curve);
    ecc_scalar_mul(&tPA, &st->public_key, t, &st->curve);
    ecc_point_add(&R, &sG, &tPA, &st->curve);
#ifdef CONSOLE_PRINT
    gmp_printf("sGx = %ZX\n", sG.x);
    gmp_printf("sGy = %ZX\n", sG.y);
    gmp_printf("tPx = %ZX\n", tPA.x);
    gmp_printf("tPy = %ZX\n", tPA.y);
    gmp_printf("x1 = %ZX\n", R.x);
    gmp_printf("y1 = %ZX\n", R.y);
#endif

    // Step 7: R = (e + x) mod n
    mpz_t Rv, x_mod;
    mpz_inits(Rv, x_mod, NULL);
    mpz_mod(x_mod, R.x, st->curve.p);
    mpz_add(Rv, e, x_mod);
    mpz_mod(Rv, Rv, n);
#ifdef CONSOLE_PRINT
    gmp_printf("Rv = %ZX\n", Rv);
#endif

    // 验证结果
    int ok = 0;
    ok     = (mpz_cmp(Rv, r) == 0);
    mpz_clears(Rv, x_mod, NULL);

    // 释放资源
    mpz_clears(e, t, NULL);
    ecc_point_clear(&G);
    ecc_point_clear(&sG);
    ecc_point_clear(&tPA);
    ecc_point_clear(&R);

    return ok;
}
