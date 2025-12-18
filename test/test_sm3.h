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

TEST(test_sm3, sm3_hmac)
{
    char*    data                             = (char*)"12345678";
    int      data_len                         = strlen(data);
    char*    key                              = (char*)"12345678";
    int      key_len                          = strlen(key);
    uint32_t good_hash[SM3_DIGEST_LENGTH / 4] = {0xacbf5c1e, 0x7ff1e4e8, 0x543d8ed1, 0xe1d48a0a, 0x20dce2a2, 0x40805599, 0xa57fb884, 0x0660f86f};
    uint8_t  hash[SM3_DIGEST_LENGTH]          = {0};
    uint32_t temp[SM3_DIGEST_LENGTH / 4]      = {0};

    sm3_hmac((uint8_t*)key, key_len, (uint8_t*)data, data_len, (uint8_t*)hash);
    array_bit8_to_bitn<uint32_t>(hash, temp, SM3_DIGEST_LENGTH / 4, 0);
    EXPECT_TRUE(my_equal_array_32bit(good_hash, temp, SM3_DIGEST_LENGTH / 4));
}

TEST(test_sm3, sm3_z)
{
    // clang-format off
    uint8_t key[] = {
        0x09, 0xF9, 0xDF, 0x31, 0x1E, 0x54, 0x21, 0xA1, 0x50, 0xDD, 0x7D, 0x16, 0x1E, 0x4B, 0xC5, 0xC6,
        0x72, 0x17, 0x9F, 0xAD, 0x18, 0x33, 0xFC, 0x07, 0x6B, 0xB0, 0x8F, 0xF3, 0x56, 0xF3, 0x50, 0x20,
        0xCC, 0xEA, 0x49, 0x0C, 0xE2, 0x67, 0x75, 0xA5, 0x2D, 0xC6, 0xEA, 0x71, 0x8C, 0xC1, 0xAA, 0x60,
        0x0A, 0xED, 0x05, 0xFB, 0xF3, 0x5E, 0x08, 0x4A, 0x66, 0x32, 0xF6, 0x07, 0x2D, 0xA9, 0xAD, 0x13,
    };
    // clang-format on
    int      key_len                          = 64;
    char*    data                             = (char*)"message digest";
    int      data_len                         = strlen(data);
    char*    id                               = (char*)"1234567812345678";
    int      id_len                           = strlen(id);
    uint32_t good_z[SM3_DIGEST_LENGTH / 4]    = {0xB2E14C5C, 0x79C6DF5B, 0x85F4FE7E, 0xD8DB7A26, 0x2B9DA7E0, 0x7CCB0EA9, 0xF4747B8C, 0xCDA8A4F3};
    uint32_t good_hash[SM3_DIGEST_LENGTH / 4] = {0XF0B43E94, 0XBA45ACCA, 0XACE692ED, 0X534382EB, 0X17E6AB5A, 0X19CE7B31, 0XF4486FDF, 0XC0D28640};
    uint8_t  hash[SM3_DIGEST_LENGTH]          = {0};
    uint32_t temp[SM3_DIGEST_LENGTH / 4]      = {0};

    sm3_z((uint8_t*)id, id_len, (uint8_t*)key, key_len, NULL, 0, (uint8_t*)hash);
    array_bit8_to_bitn<uint32_t>(hash, temp, SM3_DIGEST_LENGTH / 4, 0);
    EXPECT_TRUE(my_equal_array_32bit(good_z, temp, SM3_DIGEST_LENGTH / 4));

    sm3_z((uint8_t*)id, id_len, (uint8_t*)key, key_len, (uint8_t*)data, data_len, (uint8_t*)hash);
    array_bit8_to_bitn<uint32_t>(hash, temp, SM3_DIGEST_LENGTH / 4, 0);
    EXPECT_TRUE(my_equal_array_32bit(good_hash, temp, SM3_DIGEST_LENGTH / 4));
}

/**
 * @brief   性能测试
 */
static void bench_sm3(benchmark::State& state, const size_t total_size)
{
    uint8_t  hash[SM3_DIGEST_LENGTH] = {0};
    int64_t  data_len                = state.range(0);
    uint8_t* data                    = (uint8_t*)malloc(data_len);

    for (auto _ : state)
    {
        size_t remaining = total_size;
        while (remaining > 0)
        {
            sm3(data, data_len, (uint8_t*)hash);
            remaining -= data_len;
        }
        state.SetBytesProcessed(total_size);
    }

    free(data);
}
BENCHMARK_CAPTURE(bench_sm3, 200M, 200 * 1024 * 1024)->Unit(benchmark::kMillisecond)->Arg(1024)->Arg(64 * 1024)->Arg(1024 * 1024);