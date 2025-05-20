# LRU Cache Benchmark

In this project, we benchmark the performance of four different implementations of a LRU-type cache:
- Our implementation using the "lock-free locks" based DList and Hashmap from [cmuparlay/flock](https://github.com/cmuparlay/flock) (C++)
- Our fine-grained locking implementation built on Intel [TBB](https://github.com/uxlfoundation/oneTBB) Hashmap and Scoped Locks (C++)
- [Caffeine](https://github.com/ben-manes/caffeine) Cache (Java)
- [Guava](https://github.com/google/guava/) Cache (Java)

## Benchmarking Setup

- [Google Benchmark](https://github.com/google/benchmark) for the locking implementation
- [cmuparlay](https://github.com/cmuparlay/parlaylib) for the lock-free implementation
- [Java Microbenchmark Harness](https://github.com/openjdk/jmh) (JMH) for Caffeine and Guava

## Variables Tested

- Zipfian parameter for get/put key distribution (0.01, 0.75, 0.9, 0.99)
  - [dirtyzipf](https://github.com/ekg/dirtyzipf) for C++
  - [YCSB](https://github.com/brianfrankcooper/YCSB/tree/master) for Java
- Number of threads used during benchmarking (1, 4, 16, 64, 128 (overclock))
- Cache Size (100, 10000, 100000, 1000000)

## Dependencies

- CMake ≥ 3.13
- C++17
- Google Benchmark
- JMH (requires Maven)
- TBB
- cmuparlay/flock
- cmuparlay/parlaylib
- dirtyzipf
- YCSB
- Caffeine
- Guava
