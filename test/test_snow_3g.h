#include <string.h>

#include <gtest/gtest.h>

#include "endecode/snow_3g/snow_3g.h"
#include "endecode/snow_3g/uea2.h"
#include "endecode/snow_3g/uia2.h"

#include "test_util.h"

TEST(test_snow_3g, uea2)
{
    // 最好是 32bit 倍数，算法内部没有实现补位
    uint8_t  key[16]  = {0x5A, 0xCB, 0x1D, 0x64, 0x4C, 0x0D, 0x51, 0x20, 0x4E, 0xA5, 0xF1, 0x45, 0x10, 0x10, 0xD8, 0x52};
    uint32_t count    = 0xFA556B26;
    uint32_t bearer   = 0x03;
    uint32_t dir      = 0x01;
    uint32_t size     = 16;
    uint8_t  data[16] = {0xAD, 0x9C, 0x44, 0x1F, 0x89, 0x0B, 0x38, 0xC4, 0x57, 0xA4, 0x9D, 0x42, 0x14, 0x07, 0xE8};
    uint32_t length   = 120;

    uint8_t temp[16] = {0};
    memcpy(temp, data, size);
    snow_3g_uea2(key, count, bearer, dir, temp, length);

    snow_3g_uea2(key, count, bearer, dir, temp, length);
    EXPECT_TRUE(my_equal_array_8bit(data, temp, size));
}

TEST(test_snow_3g, uia2)
{
    // 最好是 32bit 倍数，算法内部没有实现补位
    uint8_t  key[16]  = {0x2B, 0xD6, 0x45, 0x9F, 0x82, 0xC5, 0xB3, 0x00, 0x95, 0x2C, 0x49, 0x10, 0x48, 0x81, 0xFF, 0x48};
    uint32_t count    = 0x38A6F056;
    uint32_t fresh    = 0x05D2EC49;
    uint32_t dir      = 0x01;
    uint32_t size     = 24;
    uint8_t  data[24] = {0x6B, 0x22, 0x77, 0x37, 0x29, 0x6F, 0x39, 0x3C, 0x80, 0x79, 0x35, 0x3E, 0xDC, 0x87, 0xE2, 0xE8, 0x05, 0xD2, 0xEC, 0x49, 0xA4, 0xF2, 0xD8, 0xE0};
    uint32_t length   = 189;

    uint8_t temp[24] = {0};
    memcpy(temp, data, size);
    snow_3g_uia2(key, count, fresh, dir, temp, length);

    snow_3g_uia2(key, count, fresh, dir, temp, length);
    EXPECT_TRUE(my_equal_array_8bit(data, temp, size));
}
