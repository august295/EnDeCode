#define NOMINMAX

#if defined(_WIN32) || defined(_WIN64)
    #include <windows.h>
#endif

#include <benchmark/benchmark.h>

#include "test_sm4.h"

#include "test_sm3.h"

#ifdef ENABLE_GMP
    #include "test_sm2.h"
#endif

#include "test_asn1.h"

int main(int argc, char** argv)
{
#if defined(_WIN32) || defined(_WIN64)
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    // Google Benchmark
    ::benchmark::Initialize(&argc, argv);
    ::benchmark::RunSpecifiedBenchmarks();
    return 0;
}