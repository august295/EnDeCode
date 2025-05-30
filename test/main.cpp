#define NOMINMAX

#if defined(_WIN32) || defined(_WIN64)
    #include <windows.h>
#endif

#include <gtest/gtest.h>
#include <benchmark/benchmark.h>

#include "test_a5_1.h"
#include "test_aes.h"
#include "test_des.h"
#include "test_rc4.h"
#include "test_sm4.h"
#include "test_snow_3g.h"
#include "test_zuc.h"

#include "test_md5.h"
#include "test_sha1.h"
#include "test_sha2.h"
#include "test_sha3.h"
#include "test_sm3.h"

#ifdef ENABLE_GMP
    #include "test_ecc.h"
    #include "test_rsa.h"
#endif

#include "test_base64.h"
#include "test_crc.h"

#include "test_asn1.h"

int main(int argc, char** argv)
{
#if defined(_WIN32) || defined(_WIN64)
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    // 初始化 Google Test
    ::testing::InitGoogleTest(&argc, argv);

    // 初始化 Google Benchmark
    ::benchmark::Initialize(&argc, argv);

    // 运行 Google Test
    int gtest_result = RUN_ALL_TESTS();
    if (gtest_result != 0)
    {
        return gtest_result;
    }

    // 运行 Google Benchmark
    ::benchmark::RunSpecifiedBenchmarks();
    return 0;
}