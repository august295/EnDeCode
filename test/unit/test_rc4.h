#include <gtest/gtest.h>

#include "endecode/rc4/rc4.h"

#include "test_util.h"

TEST(test_rc4, crypt)
{
    Rc4Context context;
    char*      key           = (char*)"password";
    int        key_len       = 8;
    char*      data          = (char*)"hello";
    int        data_len      = 6;
    char       cipher[6]     = {0};
    char       plaintext[6]  = {0};
    char*      data2         = (char*)"world";
    int        data2_len     = 6;
    char       cipher2[6]    = {0};
    char       plaintext2[6] = {0};

    // 加密
    rc4_init(&context, (uint8_t*)key, key_len);
    rc4_crypt(&context, (uint8_t*)data, (uint8_t*)cipher, data_len);
    rc4_crypt(&context, (uint8_t*)data2, (uint8_t*)cipher2, data2_len);

    // 解密
    rc4_init(&context, (uint8_t*)key, key_len);
    rc4_crypt(&context, (uint8_t*)cipher, (uint8_t*)plaintext, data_len);
    rc4_crypt(&context, (uint8_t*)cipher2, (uint8_t*)plaintext2, data2_len);

    EXPECT_TRUE(my_equal_array_8bit((uint8_t*)data, (uint8_t*)plaintext, data_len));
    EXPECT_TRUE(my_equal_array_8bit((uint8_t*)data2, (uint8_t*)plaintext2, data2_len));
}
