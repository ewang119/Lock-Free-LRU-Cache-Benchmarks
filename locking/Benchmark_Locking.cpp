// #include "LRU_Locking.h"
#include <benchmark/benchmark.h>
#include <random>
#include <vector>
#include <cmath>
#include <mutex>
#include <unordered_map>
#include "LRU_Locking.h" // or "LRU_Coarse.h"
#include "dirtyzipf/dirty_zipfian_int_distribution.h"

static int default_cache_size = 100'000;
static double default_zipfian = 0.75;
static int default_threads = 64;
static long num_ops = 1'000'000;


// Benchmark puts
static void BM_LRUCachePut(benchmark::State& state) {

    std::vector<uint64_t> sampled_keys(num_ops);

    int cache_size = state.range(0);
    double zipfian = state.range(1) / 100.0;

    LRUCache cache(cache_size);

    // Use dirtyzipf
    std::mt19937_64 gen(42);
    dirtyzipf::dirty_zipfian_int_distribution<uint64_t> zipf(0, cache_size - 1, zipfian);

    // Generate shared sampled keys
    sampled_keys.resize(num_ops);
    for (long i = 0; i < num_ops; ++i) {
        sampled_keys[i] = zipf(gen);
    }

    // Only this part is timed
    for (auto _ : state) {
        for (int i = 0; i < num_ops; ++i) {
            cache.put(sampled_keys[i], i);
        }
    }
}


// Benchmark GETs
static void BM_LRUCacheGet(benchmark::State& state) {

    std::vector<uint64_t> sampled_keys(num_ops);
    
    int cache_size = state.range(0);
    double zipfian = state.range(1) / 100.0;

    LRUCache cache(cache_size);
    
    // Use dirtyzipf
    std::mt19937_64 gen(42);
    dirtyzipf::dirty_zipfian_int_distribution<uint64_t> zipf(0, cache_size - 1, zipfian);

    // Generate shared sampled keys
    sampled_keys.resize(num_ops);
    for (long i = 0; i < num_ops; ++i) {
        sampled_keys[i] = zipf(gen);
    }

    // Pre-fill cache with Zipfian distributed keys
    for (int i = 0; i < cache_size && i < sampled_keys.size(); ++i) {
        cache.put(sampled_keys[i], i);
    }

    // Only GETs are timed, index using 3*i % cache_size
    for (auto _ : state) {
        for (int i = 0; i < cache_size && i < sampled_keys.size(); ++i) {
            benchmark::DoNotOptimize(cache.get(sampled_keys[3*i % cache_size]));
        }
    }
}


// Call the benchmark (PUTs)
BENCHMARK(BM_LRUCachePut)
    ->Args({default_cache_size, 1})->Threads(default_threads)
    ->Args({default_cache_size, 75})->Threads(default_threads)
    ->Args({default_cache_size, 90})->Threads(default_threads)
    ->Args({default_cache_size, 99})->Threads(default_threads);

BENCHMARK(BM_LRUCachePut)
    ->Args({default_cache_size, int(default_zipfian * 100)})->Threads(1)
    ->Args({default_cache_size, int(default_zipfian * 100)})->Threads(4)
    ->Args({default_cache_size, int(default_zipfian * 100)})->Threads(16)
    ->Args({default_cache_size, int(default_zipfian * 100)})->Threads(64)
    ->Args({default_cache_size, int(default_zipfian * 100)})->Threads(128);

BENCHMARK(BM_LRUCachePut)
    ->Args({100, int(default_zipfian * 100)})->Threads(default_threads)
    ->Args({1'000, int(default_zipfian * 100)})->Threads(default_threads)
    ->Args({10'000, int(default_zipfian * 100)})->Threads(default_threads)
    ->Args({100'000, int(default_zipfian * 100)})->Threads(default_threads)
    ->Args({1'000'000, int(default_zipfian * 100)})->Threads(default_threads);


// Call the benchmark (GETs)
BENCHMARK(BM_LRUCacheGet)
    ->Args({default_cache_size, 1})->Threads(default_threads)
    ->Args({default_cache_size, 75})->Threads(default_threads)
    ->Args({default_cache_size, 90})->Threads(default_threads)
    ->Args({default_cache_size, 99})->Threads(default_threads);


BENCHMARK(BM_LRUCacheGet)
    ->Args({default_cache_size, int(default_zipfian * 100)})->Threads(1)
    ->Args({default_cache_size, int(default_zipfian * 100)})->Threads(4)
    ->Args({default_cache_size, int(default_zipfian * 100)})->Threads(16)
    ->Args({default_cache_size, int(default_zipfian * 100)})->Threads(64)
    ->Args({default_cache_size, int(default_zipfian * 100)})->Threads(128);

BENCHMARK(BM_LRUCacheGet)
    ->Args({100, int(default_zipfian * 100)})->Threads(default_threads)
    ->Args({1'000, int(default_zipfian * 100)})->Threads(default_threads)
    ->Args({10'000, int(default_zipfian * 100)})->Threads(default_threads)
    ->Args({100'000, int(default_zipfian * 100)})->Threads(default_threads)
    ->Args({1'000'000, int(default_zipfian * 100)})->Threads(default_threads);



// Main function to run the benchmarks
int main(int argc, char** argv) {

    // Pass args to Google Benchmark
    benchmark::Initialize(&argc, argv);
    benchmark::RunSpecifiedBenchmarks();
    return 0;
}