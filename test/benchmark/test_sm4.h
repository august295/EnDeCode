#include <benchmark/benchmark.h>

#include "endecode/sm4/sm4.h"

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
