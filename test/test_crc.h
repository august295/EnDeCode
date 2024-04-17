#include <stdlib.h>
#include <string.h>

#include <gtest/gtest.h>

#include "crc/crc.h"

TEST(test_crc, str_ascii_crc_32)
{
    char*    plaintext     = "123456";
    int      plaintext_len = strlen(plaintext);
    uint32_t ciphertext    = crc32((uint8_t*)plaintext, plaintext_len, crc_32);
    char     result[32]    = {0};
    sprintf(result, "%08X", ciphertext);

    EXPECT_EQ(std::string("0972D361"), std::string(result));
}

TEST(test_crc, str_gbk_crc_32)
{
    char*    plaintext     = "中文";
    int      plaintext_len = strlen(plaintext);
    uint32_t ciphertext    = crc32((uint8_t*)plaintext, plaintext_len, crc_32);
    char     result[32]    = {0};
    sprintf(result, "%08X", ciphertext);

    EXPECT_EQ(std::string("5A09ED37"), std::string(result));
}

TEST(test_crc, hex_crc_32)
{
    // clang-format off
    // 99 aa bb cc dd ee 54 e1 ad 7e c5 11 08 00 45 00 00 2e 12 34 40 00 40 11 a4 43 c0 a8 01 51 c0 a8 01 a6 17 c1 1b a9 00 1a 6c 2c 51 51 51 51 51 51 51 51 51 51 51 51 51 51 51 51 51 51
    uint8_t plaintext[] = {
        0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, // 目的 MAC 地址
        0x54, 0xe1, 0xad, 0x7e, 0xc5, 0x11, // 源 MAC 地址
        0x08, 0x00,                         // 帧类型
        0x45,                               // 版本号 4，首部长度 20 字节 ==> 5
        0x00,                               // 服务类型，默认
        0x00, 0x2e,                         // 总长度（首部 + 数据）
        0x12, 0x34,                         // 16 标识位
        0x40, 0x00,                         // 3 比特标志不分片 010 ，13 比特片偏移
        0x40,                               // 生存时间 64
        0x11,                               // 上层协议，以 UDP 为例 17
        0xa4, 0x43,                         // IP 首部检验和
        0xc0, 0xa8, 0x01, 0x51,             // 源 IP 地址		192.168.1.81
        0xc0, 0xa8, 0x01, 0xa6,             // 目的地 IP 地址 	192.168.1.166
        0x17, 0xc1,                         // 源端口
        0x1b, 0xa9,                         // 目的地端口
        0x00, 0x1a,                         // UDP 长度
        0x6c, 0x2c,                         // UDP 检验和
        0x51, 0x51, 0x51, 0x51, 0x51, 0x51, // 传输数据
        0x51, 0x51, 0x51, 0x51, 0x51, 0x51,
        0x51, 0x51, 0x51, 0x51, 0x51, 0x51
    };
    // clang-format on
    int      plaintext_len = sizeof(plaintext) / sizeof(plaintext[0]);
    uint32_t ciphertext    = crc32((uint8_t*)plaintext, plaintext_len, crc_32);
    char     result[32]    = {0};
    sprintf(result, "%08X", ciphertext);

    EXPECT_EQ(std::string("A128EA35"), std::string(result));
}