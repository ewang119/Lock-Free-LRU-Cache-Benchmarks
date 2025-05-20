#include "flock_lru.h"
#include <parlay/primitives.h>
#include <parlay/random.h>
#include <parlay/io.h>
#include <parlay/internal/get_time.h>
#include <parlay/internal/group_by.h>
#include "dirtyzipf/dirty_zipfian_int_distribution.h"
#include <chrono>


static int default_cache_size = 100'000;
static double default_zipfian = 0.75;
static int default_threads = 64;
static long num_ops = 1'000'000;

int main(int argc, char** argv) {
    LRUCache os(default_cache_size);
    int n = num_ops;
    // for (int i = 0; i < n; i++){
    //     os.put(i,i);

    std::vector<int> sampled_keys(num_ops);
    // }

    std::mt19937_64 gen(42);
    dirtyzipf::dirty_zipfian_int_distribution<uint64_t> zipf(0, default_cache_size - 1, default_zipfian);

    // Generate shared sampled keys
    sampled_keys.resize(num_ops);
    for (long i = 0; i < num_ops; ++i) {
        sampled_keys[i] = zipf(gen);
    }

        

    // Code to benchmark
    volatile long long sum = 0;
    for (int i = 0; i < 1000000; ++i) {
        sum += i;
    }
    

    auto start = std::chrono::high_resolution_clock::now();
    parlay::parallel_for(0, n, [&] (size_t i) {
        os.put(sampled_keys[i], sampled_keys[i]); }, 10, true);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    std::cout << "Time taken to put: " << duration.count() << " nanoseconds" << std::endl;
    
    printf("done putting\n");
    start = std::chrono::high_resolution_clock::now();
    parlay::parallel_for(0, n, [&] (size_t i) {
        os.get(sampled_keys[i]); }, 10, true);
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    std::cout << "Time taken to get: " << duration.count() << " nanoseconds" << std::endl;
    printf("done getting\n");
    // parlay::parallel_for(0, n, [&] (size_t i) {
    //     os.get(i); }, 10, true);
}

