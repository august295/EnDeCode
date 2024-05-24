#include <stdlib.h>
#include <string.h>

#include <gtest/gtest.h>

#include "md5/md5.h"

TEST(test_md5, str_ascii)
{
    char* plaintext     = (char*)"123456";
    int   plaintext_len = strlen(plaintext);

    char* ciphertext     = (char*)malloc(sizeof(char) * 32);
    int   ciphertext_len = 32;
    md5_ascii((uint8_t*)plaintext, plaintext_len, 0, (uint8_t*)ciphertext);

    EXPECT_EQ("E10ADC3949BA59ABBE56E057F20F883E", std::string(ciphertext, ciphertext_len));
}

TEST(test_md5, str_gbk)
{
    char* plaintext     = (char*)"中文";
    int   plaintext_len = strlen(plaintext);

    char* ciphertext     = (char*)malloc(sizeof(char) * 32);
    int   ciphertext_len = 32;
    md5_ascii((uint8_t*)plaintext, plaintext_len, 0, (uint8_t*)ciphertext);

    EXPECT_EQ("A7BAC2239FCDCB3A067903D8077C4A07", std::string(ciphertext, ciphertext_len));
}
