#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <string.h>

#define CACHE_LINE_SIZE 64

// High-resolution monotonic timer returning nanoseconds
static inline uint64_t timer_now_ns(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000000000ULL + (uint64_t)ts.tv_nsec;
}

// Convert nanoseconds to milliseconds
static inline double ns_to_ms(uint64_t ns) {
    return (double)ns / 1.0e6;
}

// Prevent compiler from optimizing away computation
static inline void do_not_optimize(void *p) {
    __asm__ volatile("" : : "g"(p) : "memory");
}

// Memory allocation aligned to cache line boundary (64 bytes)
static inline void *aligned_alloc_64(size_t size) {
    void *ptr = NULL;
    if (posix_memalign(&ptr, CACHE_LINE_SIZE, size) != 0) {
        return malloc(size);
    }
    return ptr;
}

static inline void aligned_free_64(void *ptr) {
    free(ptr);
}

// Print benchmark result row
static inline void print_bench_result(const char *name, double time_ms, double baseline_ms, size_t bytes_processed) {
    double speedup = (time_ms > 0.0) ? (baseline_ms / time_ms) : 1.0;
    if (bytes_processed > 0 && time_ms > 0.0) {
        double gb_per_sec = ((double)bytes_processed / (1024.0 * 1024.0 * 1024.0)) / (time_ms / 1000.0);
        printf("  %-36s : %8.3f ms  (%5.2fx speedup)  [%6.2f GB/s]\n", name, time_ms, speedup, gb_per_sec);
    } else {
        printf("  %-36s : %8.3f ms  (%5.2fx speedup)\n", name, time_ms, speedup);
    }
}

static inline void print_header(const char *title) {
    printf("\n================================================================================\n");
    printf("  %s\n", title);
    printf("================================================================================\n");
}

#endif // BENCHMARK_H
