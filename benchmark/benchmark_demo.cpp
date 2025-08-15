#include "benchmark/benchmark.h"

static void BM_StringCopy(benchmark::State& state) {
    for (auto _ : state) {
        std::string s = "Hello, World!";
        std::string t = s;
        benchmark::DoNotOptimize(t);
    }
}

BENCHMARK(BM_StringCopy);

BENCHMARK_MAIN();
