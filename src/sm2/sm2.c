#include <malloc.h>
#include <time.h>

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

    ecc_generate_random(st->private_key, st->curve.p);                    // 生成私钥
    ecc_scalar_mul(&st->public_key, &st->G, st->private_key, &st->curve); // 生成公钥
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
        ecc_generate_random(k, n);
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

        // 释放资源
        mpz_clears(t1, t2, inv, NULL);
        ecc_point_clear(&G);

        // check s != 0
        if (mpz_cmp_ui(s, 0) == 0)
            continue;
#ifdef CONSOLE_PRINT
        gmp_printf("s = %ZX\n", s);
#endif
        break;
    }

    // 释放资源
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
    mpz_clears(n, e, t, NULL);
    ecc_point_clear(&G);
    ecc_point_clear(&sG);
    ecc_point_clear(&tPA);
    ecc_point_clear(&R);

    return ok;
}

int sm2_kdf(const uint8_t* Z, size_t Zlen, size_t klen_bytes, uint8_t* out)
{
    if (klen_bytes == 0 || !Z || !out)
        return -1;

    size_t hash_len = SM3_DIGEST_LENGTH;
    size_t n        = (klen_bytes + hash_len - 1) / hash_len;

    mpz_t ct;
    mpz_init(ct);

    uint8_t hash_tmp[SM3_DIGEST_LENGTH];
    size_t  out_pos = 0;

    // 预分配 Z||ct 缓冲区，避免每次 malloc/free
    uint8_t* buf = (uint8_t*)malloc(Zlen + 4);
    if (!buf)
    {
        mpz_clear(ct);
        return -2;
    }

    for (size_t i = 1; i <= n; ++i)
    {
        mpz_set_ui(ct, i);

        // 导出计数器为 4 字节大端
        uint8_t counter_buf[4] = {0};
        size_t  count_bytes    = 0;
        uint8_t tmp[4]         = {0};
        mpz_export(tmp, &count_bytes, 1, 1, 1, 0, ct);
        if (count_bytes > 4)
            count_bytes = 4; // 安全截断
        memcpy(counter_buf + (4 - count_bytes), tmp, count_bytes);

        // 拼接 Z || counter_buf
        memcpy(buf, Z, Zlen);
        memcpy(buf + Zlen, counter_buf, 4);

        // SM3 哈希
        sm3(buf, Zlen + 4, hash_tmp);

        // 拷贝到输出
        size_t to_copy = (out_pos + hash_len <= klen_bytes) ? hash_len : (klen_bytes - out_pos);
        memcpy(out + out_pos, hash_tmp, to_copy);
        out_pos += to_copy;
    }

    free(buf);
    mpz_clear(ct);

    // 检查是否全零
    int all_zero = 1;
    for (size_t i = 0; i < klen_bytes; ++i)
    {
        if (out[i] != 0)
        {
            all_zero = 0;
            break;
        }
    }
    if (all_zero)
        return -3;

    return 0;
}

void mpz_to_bytes(const mpz_t x, uint8_t* buf, size_t buf_len)
{
    memset(buf, 0, buf_len);

    // 临时存储 mpz_export 导出的字节
    size_t   count = 0;
    uint8_t* tmp   = (uint8_t*)calloc(1, buf_len);
    mpz_export(tmp, &count, 1, 1, 1, 0, x);

    if (count > buf_len)
    {
        memcpy(buf, tmp + (count - buf_len), buf_len);
    }
    else
    {
        memcpy(buf + (buf_len - count), tmp, count);
    }

    free(tmp);
}

int sm2_encrypt(
    const uint8_t* msg,
    size_t         msg_len,
    ecc_key_st*    st,
    uint8_t*       out,
    size_t*        out_len)
{
    int ret = 0;

    mpz_t n;
    mpz_init_set_str(n, sm2_params.n, 16);
    mpz_t k;
    mpz_init(k);
    ECPoint C1;
    ecc_point_init(&C1);
    ECPoint S;
    ecc_point_init(&S);
    ECPoint Temp;
    ecc_point_init(&Temp);
    uint8_t* C2 = (uint8_t*)malloc(msg_len);
    uint8_t  C3[32];

    while (1)
    {
        // Step 1: generate random k in [1, n-1]
        ecc_generate_random(k, n);
        // mpz_set_str(k, "59276E27D506861A16680F3AD9C02DCCEF3CC1FA3CDBE4CE6D54B80DEAC1BC21", 16);
        if (mpz_cmp_ui(k, 1) < 0 || mpz_cmp(k, n) >= 0)
        {
            continue;
        }

        // Step 2: C1 = [k]G
        ecc_scalar_mul(&C1, &st->G, k, &st->curve);
#ifdef CONSOLE_PRINT
        gmp_printf("C1x = %ZX\n", C1.x);
        gmp_printf("C1y = %ZX\n", C1.y);
#endif

        // Step 4: S = [k]Pb
        ecc_scalar_mul(&Temp, &st->public_key, k, &st->curve);
#ifdef CONSOLE_PRINT
        gmp_printf("Sx = %ZX\n", Temp.x);
        gmp_printf("Sy = %ZX\n", Temp.y);
#endif

        // Step 5: t = KDF(x2 || y2, klen)
        uint8_t Sxy[64];
        mpz_export(Sxy, NULL, 1, 1, 1, 0, Temp.x);
        mpz_export(Sxy + 32, NULL, 1, 1, 1, 0, Temp.y);
        size_t klen_bytes = msg_len * 8;

        uint8_t* t = (uint8_t*)malloc(klen_bytes);
        ret        = sm2_kdf(Sxy, 64, klen_bytes, t);
        if (ret != 0)
        {
            continue;
        }
#ifdef CONSOLE_PRINT
        printf("t = ");
        print_hex(t, msg_len, 4, 1);
#endif

        // Step 6: C2 = M XOR t
        for (size_t i = 0; i < msg_len; ++i)
        {
            C2[i] = msg[i] ^ t[i];
        }
#ifdef CONSOLE_PRINT
        printf("C2 = ");
        print_hex(C2, msg_len, 4, 1);
#endif

        // Step 7: C3 = Hash(x2 || M || y2)
        uint8_t* C3_input = (uint8_t*)malloc(32 + msg_len + 32);
        mpz_export(C3_input, NULL, 1, 1, 1, 0, Temp.x);
        memcpy(C3_input + 32, msg, msg_len);
        mpz_export(C3_input + 32 + msg_len, NULL, 1, 1, 1, 0, Temp.y);
        sm3(C3_input, 32 + msg_len + 32, C3);
        free(C3_input);
#ifdef CONSOLE_PRINT
        printf("C3 = ");
        print_hex(C3, 32, 4, 1);
#endif

        free(t);
        ret = 1;
        break;
    }

    // 输出结果 C1 || C3 || C2
    size_t C1_len      = 1 + 32 + 32; // 0x04 + x + y
    size_t C3_len      = 32;
    size_t C2_len      = msg_len;
    size_t out_len_sum = C1_len + C3_len + C2_len;

    uint8_t* C11 = (uint8_t*)malloc(C1_len);
    C11[0]       = 0x04;
    mpz_to_bytes(C1.x, C11 + 1, 32);
    mpz_to_bytes(C1.y, C11 + 1 + 32, 32);
    memcpy(out, C11, C1_len);
    memcpy(out + C1_len, C3, C3_len);
    memcpy(out + C1_len + C3_len, C2, C2_len);
    *out_len = out_len_sum;

    mpz_clears(n, k, NULL);
    ecc_point_clear(&C1);
    ecc_point_clear(&S);
    ecc_point_clear(&Temp);
    free(C2);
    free(C11);
    return ret;
}

int sm2_decrypt(
    uint8_t*    cipher,
    size_t      cipher_len,
    ecc_key_st* st,
    uint8_t*    out,
    size_t*     out_len)
{
    int ret = 0;
    // Step 1: 解析 C1
    // 默认 C1 格式: 0x04 || x || y
    size_t C1_len = 1 + 32 + 32;
    if (cipher_len < C1_len + 32)
    {
        ret = -1;
        goto fail1;
    }
    ECPoint C1;
    ecc_point_init(&C1);
    mpz_import(C1.x, 32, 1, 1, 1, 0, cipher + 1);
    mpz_import(C1.y, 32, 1, 1, 1, 0, cipher + 1 + 32);
    gmp_printf("C1x = %ZX\n", C1.x);
    gmp_printf("C1y = %ZX\n", C1.y);

    // 验证 C1 是否在曲线上
    if (!ecc_point_on_curve(C1.x, C1.y, st->curve.a, st->curve.b, st->curve.p))
    {
        ret = -2;
        goto fail2;
    }

    // Step 3: C11 = [dB]C1
    ECPoint C11;
    ecc_point_init(&C11);
    ecc_scalar_mul(&C11, &C1, st->private_key, &st->curve);
    if (C11.infinity)
    {
        ret = -3;
        goto fail3;
    }
#ifdef CONSOLE_PRINT
    gmp_printf("C11x = %ZX\n", C11.x);
    gmp_printf("C11y = %ZX\n", C11.y);
#endif

    // Step 4: t = KDF(x2 || y2, klen)
    uint8_t C11xy[64];
    mpz_export(C11xy, NULL, 1, 1, 1, 0, C11.x);
    mpz_export(C11xy + 32, NULL, 1, 1, 1, 0, C11.y);
    size_t   klen_bytes = cipher_len * 8;
    uint8_t* t          = (uint8_t*)malloc(klen_bytes);
    if (sm2_kdf(C11xy, 64, klen_bytes, t) != 0)
    {
        ret = -4;
        goto fail4;
    }
    size_t C2_len = cipher_len - C1_len - 32;
#ifdef CONSOLE_PRINT
    printf("t = ");
    print_hex(t, C2_len, 4, 1);
#endif

    // Step 5: M = C2 XOR t
    uint8_t* C2 = (uint8_t*)malloc(C2_len);
    memcpy(C2, cipher + C1_len + 32, C2_len);
    for (size_t i = 0; i < C2_len; ++i)
    {
        C2[i] = C2[i] ^ t[i];
    }
#ifdef CONSOLE_PRINT
    printf("M = ");
    print_hex(C2, C2_len, 4, 1);
#endif

    // Step 6: u = Hash(x2 || M || y2)
    uint8_t* u_input = (uint8_t*)malloc(32 + C2_len + 32);
    memcpy(u_input, C11xy, 32);
    memcpy(u_input + 32, C2, C2_len);
    memcpy(u_input + 32 + C2_len, C11xy + 32, 32);
    uint8_t u[32];
    sm3(u_input, 32 + C2_len + 32, u);
#ifdef CONSOLE_PRINT
    printf("u = ");
    print_hex(u, 32, 4, 1);
#endif

    // 判断 u = C3
    uint8_t C3[32];
    memcpy(C3, cipher + C1_len, 32);
    if (memcmp(u, C3, 32) == 0)
    {
        ret = 1;
        memcpy(out, C2, C2_len);
        *out_len = C2_len;
    }
    else
    {
        ret = -5;
    }

    // 释放资源
    free(u_input);
    free(C2);
fail4:
    free(t);
fail3:
    ecc_point_clear(&C11);
fail2:
    ecc_point_clear(&C1);
fail1:
    return ret;
}