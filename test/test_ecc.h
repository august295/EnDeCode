#include <gtest/gtest.h>

#include "endecode/ecc/ecc.h"

#include "test_util.h"

TEST(test_ecc, test1)
{
    const char* plaintext_str = "Hello, World!";

    // 获取密钥对
    ecc_key_st st;
    ecc_generate_keypair(&st, NID_secp256k1);

    // 加密
    ECPoint C1;
    ECPoint C2;
    ecc_public_encrypt(&C1, &C2, (uint8_t*)plaintext_str, strlen(plaintext_str), &st);

    // 解密
    char decrypted_str[256];
    ecc_private_decrypt((uint8_t*)decrypted_str, &C1, &C2, &st);
    EXPECT_STREQ(decrypted_str, plaintext_str);

    ecc_key_st_clear(&st);
}

TEST(test_ecc, test2)
{
    const char* plaintext_str = "中文";

    // 获取密钥对
    ecc_key_st st;
    ecc_generate_keypair(&st, NID_secp256k1);

    // 加密
    ECPoint C1;
    ECPoint C2;
    ecc_public_encrypt(&C1, &C2, (uint8_t*)plaintext_str, strlen(plaintext_str), &st);

    // 解密
    char decrypted_str[256];
    ecc_private_decrypt((uint8_t*)decrypted_str, &C1, &C2, &st);
    EXPECT_STREQ(decrypted_str, plaintext_str);

    ecc_key_st_clear(&st);
}