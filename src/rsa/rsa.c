#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#ifdef _WIN32
    #include <windows.h>
#else
    #include <sys/time.h>
#endif

#include <gmp.h>

#include "endecode/pad/pkcs.h"
#include "endecode/rsa/rsa.h"

// 生成随机素数 p 和 q
void generate_prime(mpz_t p, unsigned int bits)
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
void rsa_keygen(mpz_t n, mpz_t e, mpz_t d, unsigned int bits)
{
    mpz_t p, q, phi, gcd;
    mpz_inits(p, q, phi, gcd, NULL);

    // 生成 p 和 q
    generate_prime(p, bits / 2);
    generate_prime(q, bits / 2);

    // n = p * q
    mpz_mul(n, p, q);

    // phi = (p - 1) * (q - 1)
    mpz_t p1, q1;
    mpz_inits(p1, q1, NULL);
    mpz_sub_ui(p1, p, 1);
    mpz_sub_ui(q1, q, 1);
    mpz_mul(phi, p1, q1);

    // 选择 e，通常选择 65537
    mpz_set_ui(e, 7);
    while (mpz_gcd_ui(NULL, e, mpz_get_ui(phi)) != 1)
    {
        mpz_add_ui(e, e, 2); // 尝试下一个奇数
    }

    // 计算 d = e^(-1) mod phi
    mpz_invert(d, e, phi);

    // 清理
    mpz_clears(p, q, phi, gcd, p1, q1, NULL);
}

void rsa_encrypt(mpz_t ciphertext, const mpz_t plaintext, const mpz_t e, const mpz_t n)
{
    // c = m^e mod n
    mpz_powm(ciphertext, plaintext, e, n);
}

void rsa_decrypt(mpz_t plaintext, const mpz_t ciphertext, const mpz_t d, const mpz_t n)
{
    // m = c^d mod n
    mpz_powm(plaintext, ciphertext, d, n);
}

void rsa()
{
    int bit_len = 1024;

    // 生成 1024 位的密钥对
    mpz_t n, e, d;
    mpz_inits(n, e, d, NULL);
    rsa_keygen(n, e, d, bit_len);

    mpz_t plaintext, ciphertext, decryptedtext;
    mpz_inits(plaintext, ciphertext, decryptedtext, NULL);
    const char* message     = "Hello, RSA!";
    size_t      message_len = strlen(message);
    char pad[128] = {0};
    pkcs1_v1_5_pad(pad, bit_len / 8, message, message_len);
    mpz_import(plaintext, bit_len / 8, 1, 1, 0, 0, pad);
    gmp_printf("m: %Zd\n", plaintext);

    // 加密
    rsa_encrypt(ciphertext, plaintext, e, n);
    gmp_printf("c: %Zd\n", ciphertext);

    // 解密
    rsa_decrypt(decryptedtext, ciphertext, d, n);
    gmp_printf("m: %Zd\n", decryptedtext);

    char export[128]  = {0};
    size_t export_len = 0;
    mpz_export(export, &export_len, 1, 1, 0, 0, decryptedtext);
    char   unpad[128] = {0};
    size_t unpad_len  = 0;
    pkcs1_v1_5_unpad(unpad, &unpad_len, export, export_len);
    printf("Origin text: %s\n", unpad);

    // 清理
    mpz_clears(n, e, d, plaintext, ciphertext, decryptedtext, NULL);
}
