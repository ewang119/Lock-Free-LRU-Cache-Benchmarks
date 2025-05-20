cmake_minimum_required(VERSION 3.13)
project(LRUCacheBenchmark VERSION 1.0 DESCRIPTION "Concurrent LRU Cache Benchmarks" LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Debug)

# Threads
find_package(Threads REQUIRED)

# Google Benchmark
find_package(benchmark REQUIRED)

# Intel TBB
find_package(TBB REQUIRED)

# Include dirtyzipf
include_directories(${CMAKE_SOURCE_DIR}/dirtyzipf)

# Compiler options
add_compile_options(-mcx16 -march=native -g)

# Add benchmark target
set(SOURCES Benchmark_Locking.cpp)
add_executable(LRUCacheBenchmark ${SOURCES})
target_link_libraries(LRUCacheBenchmark
    PRIVATE
        benchmark::benchmark
        Threads::Threads
        TBB::tbb
)
