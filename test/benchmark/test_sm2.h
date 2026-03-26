#include <benchmark/benchmark.h>

#include "endecode/sm2/sm2.h"

static void bench_sm2_sign(benchmark::State& state)
{
    std::string msg     = "message digest";
    size_t      msg_len = msg.size();
    std::string id      = "1234567812345678";
    size_t      id_len  = id.size();
    ecc_key_st  st;
    sm2_generate_keypair(&st);
    mpz_t r, s;
    mpz_inits(r, s, NULL);

    for (auto _ : state)
    {
        sm2_sign((uint8_t*)msg.data(), msg_len, (uint8_t*)id.data(), id_len, &st, r, s);
    }

    state.SetItemsProcessed(state.iterations());

    // 释放资源
    ecc_key_st_clear(&st);
    mpz_clears(r, s, NULL);
}
BENCHMARK(bench_sm2_sign);
