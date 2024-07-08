#include <gtest/gtest.h>

#include "endecode/sm3/sm3.h"

#include "test_util.h"

TEST(test_sm3, test1)
{
    char*    data                             = (char*)"abc";
    int      data_len                         = strlen(data);
    uint32_t good_hash[SM3_DIGEST_LENGTH / 4] = {0x66c7f0f4, 0x62eeedd9, 0xd1f2d46b, 0xdc10e4e2, 0x4167c487, 0x5cf2f7a2, 0x297da02b, 0x8f4ba8e0};
    uint8_t  hash[SM3_DIGEST_LENGTH]          = {0};
    uint32_t temp[SM3_DIGEST_LENGTH / 4]      = {0};

    sm3((uint8_t*)data, data_len, (uint8_t*)hash);
    array_bit8_to_bitn<uint32_t>(hash, temp, SM3_DIGEST_LENGTH / 4, 0);
    EXPECT_TRUE(my_equal_array_32bit(good_hash, temp, SM3_DIGEST_LENGTH / 4));
}

TEST(test_sm3, test2)
{
    uint32_t data[16]                         = {0x61626364, 0x61626364, 0x61626364, 0x61626364, 0x61626364, 0x61626364, 0x61626364, 0x61626364, 0x61626364, 0x61626364, 0x61626364, 0x61626364, 0x61626364, 0x61626364, 0x61626364, 0x61626364};
    uint32_t good_hash[SM3_DIGEST_LENGTH / 4] = {0xdebe9ff9, 0x2275b8a1, 0x38604889, 0xc18e5a4d, 0x6fdb70e5, 0x387e5765, 0x293dcba3, 0x9c0c5732};
    uint8_t  hash[SM3_DIGEST_LENGTH]          = {0};
    uint32_t temp[SM3_DIGEST_LENGTH / 4]      = {0};
    int      data_len                         = 64;

    for (int i = 0; i < 16; i++)
    {
        data[i] = swap32(data[i]);
    }
    sm3((uint8_t*)data, data_len, (uint8_t*)hash);
    array_bit8_to_bitn<uint32_t>(hash, temp, SM3_DIGEST_LENGTH / 4, 0);
    EXPECT_TRUE(my_equal_array_32bit(good_hash, temp, SM3_DIGEST_LENGTH / 4));
}
