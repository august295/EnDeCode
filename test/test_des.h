#include <stdlib.h>
#include <string.h>

#include <gtest/gtest.h>

#include "des/des.h"

TEST(test_des, str_ascii)
{
    char* plaintext     = "123456";
    int   plaintext_len = strlen(plaintext);

    uint64_t key = 0;
    genkey(&key);

    char* ciphertext     = (char*)calloc(plaintext_len + 1, sizeof(char));
    int   ciphertext_len = des_ecb_zero(plaintext, plaintext_len, true, key, &ciphertext);

    char* result     = (char*)calloc(plaintext_len + 1, sizeof(char));
    int   result_len = des_ecb_zero(ciphertext, ciphertext_len, false, key, &result);

    EXPECT_EQ(std::string(plaintext, plaintext_len), std::string(result, result_len));
}

TEST(test_des, str_gbk)
{
    char* plaintext     = "中文";
    int   plaintext_len = strlen(plaintext);

    uint64_t key = 0;
    genkey(&key);

    char* ciphertext     = (char*)calloc(plaintext_len + 1, sizeof(char));
    int   ciphertext_len = des_ecb_zero(plaintext, plaintext_len, true, key, &ciphertext);

    char* result     = (char*)calloc(plaintext_len + 1, sizeof(char));
    int   result_len = des_ecb_zero(ciphertext, ciphertext_len, false, key, &result);

    EXPECT_EQ(std::string(plaintext, plaintext_len), std::string(result, result_len));
}