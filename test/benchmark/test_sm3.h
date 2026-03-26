#include <benchmark/benchmark.h>

#include "endecode/sm3/sm3.h"

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