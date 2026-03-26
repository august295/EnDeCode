#include <gtest/gtest.h>

#include "endecode/sha2/sha2_256.h"

#include "test_util.h"

TEST(test_sha2_256, test1)
{
    char* data                                 = (char*)"";
    int   data_len                             = strlen(data);
    char* good_hash                            = (char*)"e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855";
    char  hash[SHA2_256_DIGEST_LENGTH]         = {0};
    char  temp[2 * SHA2_256_DIGEST_LENGTH + 1] = {0};

    sha2_256((uint8_t*)data, data_len, (uint8_t*)hash);
    hex_to_ascii((uint8_t*)hash, SHA2_256_DIGEST_LENGTH, 0, (uint8_t*)temp);
    EXPECT_STREQ(good_hash, temp);
}

TEST(test_sha2_256, test2)
{
    char* data                                 = (char*)"abc";
    int   data_len                             = strlen(data);
    char* good_hash                            = (char*)"ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad";
    char  hash[SHA2_256_DIGEST_LENGTH]         = {0};
    char  temp[2 * SHA2_256_DIGEST_LENGTH + 1] = {0};

    sha2_256((uint8_t*)data, data_len, (uint8_t*)hash);
    hex_to_ascii((uint8_t*)hash, SHA2_256_DIGEST_LENGTH, 0, (uint8_t*)temp);
    EXPECT_STREQ(good_hash, temp);
}

TEST(test_sha2_256, test3)
{
    char* data                                 = (char*)"abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
    int   data_len                             = strlen(data);
    char* good_hash                            = (char*)"248d6a61d20638b8e5c026930c3e6039a33ce45964ff2167f6ecedd419db06c1";
    char  hash[SHA2_256_DIGEST_LENGTH]         = {0};
    char  temp[2 * SHA2_256_DIGEST_LENGTH + 1] = {0};

    sha2_256((uint8_t*)data, data_len, (uint8_t*)hash);
    hex_to_ascii((uint8_t*)hash, SHA2_256_DIGEST_LENGTH, 0, (uint8_t*)temp);
    EXPECT_STREQ(good_hash, temp);
}