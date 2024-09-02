#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#ifdef _WIN32
    #include <windows.h>
#else
    #include <sys/time.h>
#endif

#include "endecode/rsa/rsa.h"

// 生成随机素数 p 和 q
// 保证生成素数不一样
void generate_prime(mpz_t p, uint32_t bits)
{
    gmp_randstate_t state;
    gmp_randinit_default(state);
#ifdef _WIN32
    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);
    gmp_randseed_ui(state, (unsigned long)counter.QuadPart);
#else
    struct timeval tv;
    gettimeofday(&tv, NULL);
    gmp_randseed_ui(state, tv.tv_sec * 1000000 + tv.tv_usec);
#endif
    mpz_urandomb(p, state, bits);
    mpz_nextprime(p, p);
    gmp_randclear(state);
}

// 生成密钥对
rsa_st* rsa_key_gen(uint32_t bits, uint32_t e)
{
    rsa_st* ctx = (rsa_st*)malloc(sizeof(rsa_st));
    ctx->bits   = bits;
    mpz_inits(ctx->n, ctx->e, ctx->d, ctx->p, ctx->q, ctx->p1, ctx->q1, ctx->phi, NULL);

    // 生成 p 和 q
    generate_prime(ctx->p, bits / 2);
    generate_prime(ctx->q, bits / 2);

    // n = p * q
    mpz_mul(ctx->n, ctx->p, ctx->q);

    // phi = (p - 1) * (q - 1)
    mpz_sub_ui(ctx->p1, ctx->p, 1);
    mpz_sub_ui(ctx->q1, ctx->q, 1);
    mpz_mul(ctx->phi, ctx->p1, ctx->q1);

    // 选择 e，通常选择 65537
    mpz_set_ui(ctx->e, e);
    while (mpz_gcd_ui(NULL, ctx->e, mpz_get_ui(ctx->phi)) != 1)
    {
        mpz_add_ui(ctx->e, ctx->e, 2); // 尝试下一个奇数
    }

    // 计算 d = e^(-1) mod phi
    mpz_invert(ctx->d, ctx->e, ctx->phi);
    return ctx;
}

void rsa_key_free(rsa_st* ctx)
{
    mpz_clears(ctx->n, ctx->e, ctx->d, ctx->p, ctx->q, ctx->p1, ctx->q1, ctx->phi, NULL);
}

int rsa_public_encrypt(const uint8_t* input, uint32_t input_len, uint8_t* output, const rsa_st* ctx, _func_pad func_pad)
{
    int      ret        = -1;
    uint32_t block_size = ctx->bits / 8;
    char*    pad        = NULL;
    mpz_t    in, out;
    mpz_inits(in, out, NULL);
    if (func_pad)
    {
        char* pad = (char*)malloc(block_size);
        func_pad(pad, block_size, input, input_len);
        mpz_import(in, block_size, 1, 1, 0, 0, pad);
    }
    else
    {
        char* pad = (char*)malloc(input_len);
        memcpy(pad, input, input_len);
        mpz_import(in, input_len, 1, 1, 0, 0, pad);
    }

    // 加密 c = m^e mod n
    mpz_powm(out, in, ctx->e, ctx->n);
    size_t output_len = 0;
    mpz_export(output, &output_len, 1, 1, 0, 0, out);
    ret = output_len;

    // 清理
    mpz_clears(in, out, NULL);
    free(pad);
    return ret;
}

int rsa_private_decrypt(const uint8_t* input, uint32_t input_len, uint8_t* output, const rsa_st* ctx, _func_unpad func_unpad)
{
    int ret = -1;

    mpz_t in, out;
    mpz_inits(in, out, NULL);
    mpz_import(in, input_len, 1, 1, 0, 0, input);

    // 解密 m = c^d mod n
    mpz_powm(out, in, ctx->d, ctx->n);

    char* export      = malloc(input_len);
    size_t export_len = 0;
    mpz_export(export, &export_len, 1, 1, 0, 0, out);
    ret = export_len;
    if (func_unpad)
    {
        size_t output_len = 0;
        func_unpad(output, &output_len, export, export_len);
        ret = output_len;
    }
    else
    {
        memcpy(output, export, export_len);
    }

    // 清理
    mpz_clears(in, out, NULL);
    free(export);
    return ret;
}
