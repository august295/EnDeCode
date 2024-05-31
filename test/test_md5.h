#include <gtest/gtest.h>

#include "md5/md5.h"

#include "test_util.h"

TEST(test_md5, str_ascii)
{
    char* plaintext      = (char*)"123456";
    int   plaintext_len  = strlen(plaintext);
    char* good_hash      = (char*)"E10ADC3949BA59ABBE56E057F20F883E";
    char  ciphertext[16] = {0};
    char  temp[33]       = {0};

    md5((uint8_t*)plaintext, plaintext_len, (uint8_t*)ciphertext);
    hex_to_ascii((uint8_t*)ciphertext, 16, 1, (uint8_t*)temp);
    EXPECT_STREQ(good_hash, temp);
}

TEST(test_md5, str_gbk)
{
    char* plaintext      = (char*)"中文";
    int   plaintext_len  = strlen(plaintext);
    char* good_hash      = (char*)"A7BAC2239FCDCB3A067903D8077C4A07";
    char  ciphertext[16] = {0};
    char  temp[33]       = {0};

    md5((uint8_t*)plaintext, plaintext_len, (uint8_t*)ciphertext);
    hex_to_ascii((uint8_t*)ciphertext, 16, 1, (uint8_t*)temp);
    EXPECT_STREQ(good_hash, temp);
}
