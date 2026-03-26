#include <gtest/gtest.h>

#include "endecode/sha3/sha3_256.h"

#include "test_util.h"

TEST(test_sha3_256, test1)
{
    char* data                                 = (char*)"";
    int   data_len                             = strlen(data);
    char* good_hash                            = (char*)"a7ffc6f8bf1ed76651c14756a061d662f580ff4de43b49fa82d80a4b80f8434a";
    char  hash[SHA3_256_DIGEST_LENGTH]         = {0};
    char  temp[2 * SHA3_256_DIGEST_LENGTH + 1] = {0};

    sha3_256((uint8_t*)data, data_len, (uint8_t*)hash);
    hex_to_ascii((uint8_t*)hash, SHA3_256_DIGEST_LENGTH, 0, (uint8_t*)temp);
    EXPECT_STREQ(good_hash, temp);
}

TEST(test_sha3_256, test2)
{
    char* data                                 = (char*)"abc";
    int   data_len                             = strlen(data);
    char* good_hash                            = (char*)"3a985da74fe225b2045c172d6bd390bd855f086e3e9d525b46bfe24511431532";
    char  hash[SHA3_256_DIGEST_LENGTH]         = {0};
    char  temp[2 * SHA3_256_DIGEST_LENGTH + 1] = {0};

    sha3_256((uint8_t*)data, data_len, (uint8_t*)hash);
    hex_to_ascii((uint8_t*)hash, SHA3_256_DIGEST_LENGTH, 0, (uint8_t*)temp);
    EXPECT_STREQ(good_hash, temp);
}
