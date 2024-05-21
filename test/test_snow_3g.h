#include <stdlib.h>
#include <string.h>

#include <gtest/gtest.h>

#include "snow_3g/snow_3g.h"

TEST(test_snow_3g, case1)
{
    uint32_t k[4]  = {0};
    uint32_t IV[4] = {0};
    int      n     = 2;
    uint32_t ks[2] = {0};

    k[0] = 0x2BD6459F;
    k[1] = 0x82C5B300;
    k[2] = 0x952C4910;
    k[3] = 0x4881FF48;

    IV[0] = 0xEA024714;
    IV[1] = 0xAD5C4D84;
    IV[2] = 0xDF1F9B25;
    IV[3] = 0x1C0BF45F;

    snow_3g(k, IV, n, ks);

    EXPECT_EQ(0xABEE9704, ks[0]);
    EXPECT_EQ(0x7AC31373, ks[1]);
}

TEST(test_snow_3g, case2)
{
    uint32_t k[4]  = {0};
    uint32_t IV[4] = {0};
    int      n     = 2;
    uint32_t ks[2] = {0};

    k[0] = 0x8CE33E2C;
    k[1] = 0xC3C0B5FC;
    k[2] = 0x1F3DE8A6;
    k[3] = 0xDC66B1F3;

    IV[0] = 0xD3C5D592;
    IV[1] = 0x327FB11C;
    IV[2] = 0xDE551988;
    IV[3] = 0xCEB2F9B7;

    snow_3g(k, IV, n, ks);

    EXPECT_EQ(0xEFF8A342, ks[0]);
    EXPECT_EQ(0xF751480F, ks[1]);
}

TEST(test_snow_3g, case3)
{
    uint32_t k[4]  = {0};
    uint32_t IV[4] = {0};
    int      n     = 2;
    uint32_t ks[2] = {0};

    k[0] = 0x4035C668;
    k[1] = 0x0AF8C6D1;
    k[2] = 0xA8FF8667;
    k[3] = 0xB1714013;

    IV[0] = 0x62A54098;
    IV[1] = 0x1BA6F9B7;
    IV[2] = 0x4592B0E7;
    IV[3] = 0x8690F71B;

    snow_3g(k, IV, n, ks);

    EXPECT_EQ(0xA8C874A9, ks[0]);
    EXPECT_EQ(0x7AE7C4F8, ks[1]);
}

TEST(test_snow_3g, case4)
{
    uint32_t k[4]     = {0};
    uint32_t IV[4]    = {0};
    int      n        = 2500;
    uint32_t ks[2500] = {0};

    k[0] = 0x0DED7263;
    k[1] = 0x109CF92E;
    k[2] = 0x3352255A;
    k[3] = 0x140E0F76;

    IV[0] = 0x6B68079A;
    IV[1] = 0x41A7C4C9;
    IV[2] = 0x1BEFD79F;
    IV[3] = 0x7FDCC233;

    snow_3g(k, IV, n, ks);

    EXPECT_EQ(0xD712C05C, ks[0]);
    EXPECT_EQ(0xA937C2A6, ks[1]);
    EXPECT_EQ(0xEB7EAAE3, ks[2]);
    EXPECT_EQ(0x9C0DB3AA, ks[2499]);
}