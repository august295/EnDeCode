#include <gtest/gtest.h>

#include "endecode/des/des.h"

#include "test_util.h"

TEST(test_des, test1)
{
    uint64_t plaintext       = *(uint64_t*)"abcdefgh";
    uint64_t key             = *(uint64_t*)"abcdefgh";
    char*    good_ciphertext = (char*)"2a8d69de9d5fdff9";
    uint8_t  temp[8]         = {0};
    char     temp_str[17]    = {0};

    plaintext = swap64(plaintext);
    key       = swap64(key);
    // 加密
    uint64_t ciphertext = des(plaintext, key, 1);
    bitn_to_bit8<uint64_t>(ciphertext, temp, sizeof(uint64_t), 0);
    hex_to_ascii(temp, 8, 0, (uint8_t*)temp_str);
    EXPECT_STREQ(good_ciphertext, temp_str);

    // 解密
    uint64_t decrypted_text = des(ciphertext, key, 0);
    EXPECT_EQ(plaintext, decrypted_text);
}

TEST(test_des, test2)
{
    uint64_t plaintext       = *(uint64_t*)"12345678";
    uint64_t key             = *(uint64_t*)"12345678";
    char*    good_ciphertext = (char*)"96d0028878d58c89";
    uint8_t  temp[8]         = {0};
    char     temp_str[17]    = {0};

    plaintext = swap64(plaintext);
    key       = swap64(key);
    // 加密
    uint64_t ciphertext = des(plaintext, key, 1);
    bitn_to_bit8<uint64_t>(ciphertext, temp, sizeof(uint64_t), 0);
    hex_to_ascii(temp, 8, 0, (uint8_t*)temp_str);
    EXPECT_STREQ(good_ciphertext, temp_str);

    // 解密
    uint64_t decrypted_text = des(ciphertext, key, 0);
    EXPECT_EQ(plaintext, decrypted_text);
}
