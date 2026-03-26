#include <gtest/gtest.h>

#include "endecode/rsa/rsa.h"

#include "test_util.h"

TEST(test_rsa, test1)
{
    int     bits = 512;
    rsa_st* ctx  = rsa_key_gen(bits, 65537);

    char* plaintext      = (char*)"Hello, RSA!";
    int   plaintext_len  = strlen(plaintext);
    char* ciphertext     = (char*)malloc(bits / 8);
    int   ciphertext_len = rsa_public_encrypt((uint8_t*)plaintext, plaintext_len, (uint8_t*)ciphertext, ctx, NULL);
    char* result         = (char*)malloc(bits / 8);
    int   result_len     = rsa_private_decrypt((uint8_t*)ciphertext, ciphertext_len, (uint8_t*)result, ctx, NULL);
    result[result_len]   = '\0';
    EXPECT_STREQ(plaintext, result);

    // 释放资源
    free(ciphertext);
    free(result);
    rsa_key_free(ctx);
}

TEST(test_rsa, test2)
{
    int     bits = 1024;
    rsa_st* ctx  = rsa_key_gen(bits, 65537);

    char* plaintext      = (char*)"中文";
    int   plaintext_len  = strlen(plaintext);
    char* ciphertext     = (char*)malloc(bits / 8);
    int   ciphertext_len = rsa_public_encrypt((uint8_t*)plaintext, plaintext_len, (uint8_t*)ciphertext, ctx, NULL);
    char* result         = (char*)malloc(bits / 8);
    int   result_len     = rsa_private_decrypt((uint8_t*)ciphertext, ciphertext_len, (uint8_t*)result, ctx, NULL);
    result[result_len]   = '\0';
    EXPECT_STREQ(plaintext, result);

    // 释放资源
    free(ciphertext);
    free(result);
    rsa_key_free(ctx);
}

TEST(test_rsa, test_pkcs1)
{
    int     bits = 2048;
    rsa_st* ctx  = rsa_key_gen(bits, 65537);

    char* plaintext      = (char*)"Hello, RSA!";
    int   plaintext_len  = strlen(plaintext);
    char* ciphertext     = (char*)malloc(bits / 8);
    int   ciphertext_len = rsa_public_encrypt((uint8_t*)plaintext, plaintext_len, (uint8_t*)ciphertext, ctx, pkcs1_v1_5_pad);
    char* result         = (char*)malloc(bits / 8);
    int   result_len     = rsa_private_decrypt((uint8_t*)ciphertext, ciphertext_len, (uint8_t*)result, ctx, pkcs1_v1_5_unpad);
    result[result_len]   = '\0';
    EXPECT_STREQ(plaintext, result);

    // 释放资源
    free(ciphertext);
    free(result);
    rsa_key_free(ctx);
}