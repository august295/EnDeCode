#include <gtest/gtest.h>

#include "zuc/zuc.h"
#include "zuc/zuc_eea3.h"
#include "zuc/zuc_eia3.h"

#include "test_util.h"

TEST(test_zuc, zuc_test1)
{
    uint8_t  key[16]       = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t  iv[16]        = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint32_t good_ouput[2] = {0x27bede74, 0x018082da};

    uint32_t temp[2] = {0};
    int      len     = 2;
    ZUC(key, iv, temp, len);
    EXPECT_TRUE(my_equal_array_32bit(good_ouput, temp, len));
}

TEST(test_zuc, eea3_test1)
{
    uint8_t  key[16]      = {0x17, 0x3d, 0x14, 0xba, 0x50, 0x03, 0x73, 0x1d, 0x7a, 0x60, 0x04, 0x94, 0x70, 0xf0, 0x0a, 0x29};
    uint32_t count        = 0x66035492;
    uint32_t bearer       = 0xf;
    uint32_t direction    = 0x0;
    uint32_t length       = 193;
    uint32_t plaintext[]  = {0x6cf65340, 0x735552ab, 0x0c9752fa, 0x6f9025fe, 0x0bd675d9, 0x005875b2, 0x00000000};
    uint32_t ciphertext[] = {0xa6c85fc6, 0x6afb8533, 0xaafc2518, 0xdfe78494, 0x0ee1e4b0, 0x30238cc8, 0x00000000};

    uint32_t temp[7] = {0};
    zuc_EEA3(key, count, bearer, direction, length, plaintext, temp);
    EXPECT_TRUE(my_equal_array_32bit_1bit(ciphertext, temp, length));

    uint32_t temp2[7] = {0};
    zuc_EEA3(key, count, bearer, direction, length, temp, temp2);
    EXPECT_TRUE(my_equal_array_32bit_1bit(plaintext, temp2, length));
}

TEST(test_zuc, eia3_test1)
{
    uint8_t  key[16]   = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint32_t count     = 0x0;
    uint32_t bearer    = 0x0;
    uint32_t direction = 0x0;
    uint32_t length    = 1;
    uint32_t message[] = {0x00000000};
    uint32_t mac[]     = {0xc8a9595e};

    uint32_t temp[1] = {0};
    zuc_EIA3(key, count, bearer, direction, length, message, temp);
    EXPECT_TRUE(my_equal_array_32bit_1bit(mac, temp, length));

    uint32_t temp2[1] = {0};
    zuc_EEA3(key, count, bearer, direction, length, temp, temp2);
    EXPECT_TRUE(my_equal_array_32bit_1bit(message, temp2, length));
}
