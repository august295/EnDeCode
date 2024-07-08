#include <gtest/gtest.h>

#include "endecode/sha1/sha1.h"

#include "test_util.h"

TEST(test_sha1, test1)
{
    char* data                             = (char*)"";
    int   data_len                         = strlen(data);
    char* good_hash                        = (char*)"da39a3ee5e6b4b0d3255bfef95601890afd80709";
    char  hash[SHA1_DIGEST_LENGTH]         = {0};
    char  temp[2 * SHA1_DIGEST_LENGTH + 1] = {0};

    SHA1((uint8_t*)data, data_len, (uint8_t*)hash);
    hex_to_ascii((uint8_t*)hash, SHA1_DIGEST_LENGTH, 0, (uint8_t*)temp);
    EXPECT_STREQ(good_hash, temp);
}

TEST(test_sha1, test2)
{
    char* data                             = (char*)"中文";
    int   data_len                         = strlen(data);
    char* good_hash                        = (char*)"7be2d2d20c106eee0836c9bc2b939890a78e8fb3";
    char  hash[SHA1_DIGEST_LENGTH]         = {0};
    char  temp[2 * SHA1_DIGEST_LENGTH + 1] = {0};

    SHA1((uint8_t*)data, data_len, (uint8_t*)hash);
    hex_to_ascii((uint8_t*)hash, SHA1_DIGEST_LENGTH, 0, (uint8_t*)temp);
    EXPECT_STREQ(good_hash, temp);
}
