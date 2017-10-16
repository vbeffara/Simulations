#include <benchmark/benchmark.h>
#include <cmath>

volatile double base = 1.001;

void mul (benchmark::State & state) {
    while (state.KeepRunning()) {
        benchmark::DoNotOptimize (pow(base,state.range(0)));
    }
}
BENCHMARK(mul)->Range(1,1<<30);

BENCHMARK_MAIN();
