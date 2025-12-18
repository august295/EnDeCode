#include <gtest/gtest.h>

#include "endecode/sm4/sm4.h"

#include "test_util.h"

TEST(test_sm4, test1)
{
    int     len                 = 16;
    uint8_t key[16]             = {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10};
    uint8_t plaintext[16]       = {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10};
    uint8_t good_ciphertext[16] = {0x68, 0x1E, 0xDF, 0x34, 0xD2, 0x06, 0x96, 0x5E, 0x86, 0xB3, 0xE9, 0x4F, 0x53, 0x6E, 0x42, 0x46};
    uint8_t ciphertext[16]      = {0};
    uint8_t temp[16]            = {0};

    // encrypt
    Sm4Context ctx;
    sm4Init(&ctx, key, len);
    sm4EncryptBlock(&ctx, plaintext, ciphertext);
    EXPECT_TRUE(my_equal_array_8bit(good_ciphertext, ciphertext, len));

    // decrypt
    sm4DecryptBlock(&ctx, ciphertext, temp);
    EXPECT_TRUE(my_equal_array_8bit(plaintext, temp, len));
}

TEST(test_sm4, test2)
{
    int     len                 = 16;
    uint8_t key[16]             = {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10};
    uint8_t plaintext[16]       = {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10};
    uint8_t good_ciphertext[16] = {0x59, 0x52, 0x98, 0xC7, 0xC6, 0xFD, 0x27, 0x1F, 0x04, 0x02, 0xF8, 0x04, 0xC3, 0x3D, 0x3F, 0x66};
    uint8_t ciphertext[16]      = {0};

    Sm4Context ctx;
    sm4Init(&ctx, key, len);
    for (int i = 0; i < 1000000; i++)
    {
        sm4EncryptBlock(&ctx, plaintext, ciphertext);
        memmove(plaintext, ciphertext, sizeof(ciphertext));
    }
    EXPECT_TRUE(my_equal_array_8bit(good_ciphertext, ciphertext, len));
}

TEST(test_sm4, test_ecb)
{
    std::string data       = "0123456789abcde"; // 15 bytes
    uint8_t     output[16] = {0};
    size_t      output_len = data.length() + 16;

    sm4_encrypt_ecb((uint8_t*)data.c_str(), data.length(), output, &output_len, pkcs7_pad);
    sm4_decrypt_ecb(output, output_len, output, &output_len, pkcs7_unpad);
    EXPECT_TRUE(my_equal_array_8bit((uint8_t*)data.c_str(), output, data.length()));
}

/**
 * @brief   性能测试
 */
static void bench_sm4_encrypt_ecb(benchmark::State& state, const size_t total_size)
{
    int64_t  buffer_size = (size_t)state.range(0);
    uint8_t* buffer      = (uint8_t*)malloc(buffer_size);
    uint8_t* output      = (uint8_t*)malloc(buffer_size + 16);
    size_t   output_len  = buffer_size + 16;

    for (auto _ : state)
    {
        size_t remaining = total_size;
        while (remaining > 0)
        {
            sm4_encrypt_ecb(buffer, buffer_size, output, &output_len, pkcs7_pad);
            remaining -= buffer_size;
        }

        state.SetBytesProcessed(total_size);
    }

    free(buffer);
    free(output);
}
BENCHMARK_CAPTURE(bench_sm4_encrypt_ecb, 200M, 200 * 1024 * 1024)->Unit(benchmark::kMillisecond)->Arg(1024)->Arg(64 * 1024)->Arg(1024 * 1024);