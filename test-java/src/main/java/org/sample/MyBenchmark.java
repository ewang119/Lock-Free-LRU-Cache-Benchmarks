package org.sample;

import com.github.benmanes.caffeine.cache.Cache;
import com.github.benmanes.caffeine.cache.Caffeine;
import com.google.common.cache.CacheBuilder;
import org.openjdk.jmh.annotations.*;
import site.ycsb.generator.ZipfianGenerator;

import java.util.concurrent.ThreadLocalRandom;
import java.util.concurrent.TimeUnit;

@BenchmarkMode(Mode.Throughput)
@OutputTimeUnit(TimeUnit.MILLISECONDS)
@State(Scope.Benchmark) // Shared cache across threads
public class MyBenchmark {

    @Param({"100000"})  // default; overridden by script
    public int cacheSize;

    @Param({"0.75"})     // default; overridden by script
    public double zipfianSkew;

    @Param({"1000000"}) // Standard
    public long numOps;

    @Param({"64"})  // default; overridden by script
    public int concurrencyLevel;

    private com.google.common.cache.Cache<Integer, Integer> guavaCache;
    private Cache<Integer, Integer> caffeineCache;

    private ZipfianGenerator zipf;
    private int[] sampledKeys;
    private ThreadLocal<Integer> threadIndex;

    @Setup(Level.Iteration)
    public void setup() {
        guavaCache = CacheBuilder.newBuilder()
                .maximumSize(cacheSize)
                .concurrencyLevel(concurrencyLevel)
                .build();

        caffeineCache = Caffeine.newBuilder()
                .maximumSize(cacheSize)
                .build();

        // Generate Zipfian sample keys
        zipf = new ZipfianGenerator(cacheSize, zipfianSkew);
        sampledKeys = new int[(int) cacheSize];

        for (int i = 0; i < cacheSize; i++) {
            sampledKeys[i] = zipf.nextValue().intValue();
        }

        // Prepopulate caches
        for (int i = 0; i < Math.min(cacheSize, sampledKeys.length); i++) {
            int key = sampledKeys[i];
            guavaCache.put(key, i);
            caffeineCache.put(key, i);
        }

        // Thread-local index for safe concurrent access
        threadIndex = ThreadLocal.withInitial(() -> ThreadLocalRandom.current().nextInt((int) numOps));
    }

    private int nextKeyPut() {
        int i = (threadIndex.get() + 1) % sampledKeys.length;
        threadIndex.set(i);
        return sampledKeys[i];
    }

    // Key rotation for get operation
    private int nextKeyGet() {
        int i = (threadIndex.get() + 3) % sampledKeys.length;
        threadIndex.set(i);
        return sampledKeys[i];
    }


    @Benchmark
    public Integer guavaGet() {
        return guavaCache.getIfPresent(nextKeyGet());
    }

    @Benchmark
    public Integer caffeineGet() {
        return caffeineCache.getIfPresent(nextKeyGet());
    }

    @Benchmark
    public void guavaPut() {
        guavaCache.put(nextKeyPut(), threadIndex.get());
    }

    @Benchmark
    public void caffeinePut() {
        caffeineCache.put(nextKeyPut(), threadIndex.get());
    }
}
