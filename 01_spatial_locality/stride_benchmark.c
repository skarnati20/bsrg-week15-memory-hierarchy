#include <stdio.h>
#include <stdlib.h>
#include "../common/benchmark.h"

#define ARRAY_SIZE (64 * 1024 * 1024) // 64 million ints (256 MB, exceeds L3 cache)

int main(void) {
    print_header("01 - Stride Benchmark: Cache Line Striding Effect");
    printf("Array Size: %d ints (%.2f MB)\n", ARRAY_SIZE, (double)(ARRAY_SIZE * sizeof(int)) / (1024.0 * 1024.0));
    printf("Testing access time per operation as stride increases from 1 to 128 ints:\n\n");

    int *arr = (int *)aligned_alloc_64(ARRAY_SIZE * sizeof(int));
    if (!arr) {
        fprintf(stderr, "Failed to allocate memory\n");
        return 1;
    }

    for (int i = 0; i < ARRAY_SIZE; i++) {
        arr[i] = 1;
    }

    int strides[] = {1, 2, 4, 8, 16, 32, 64, 128};
    int num_strides = sizeof(strides) / sizeof(strides[0]);

    printf("  %-10s | %-12s | %-15s | %-15s\n", "Stride", "Bytes Jumped", "Time (ms)", "ns per access");
    printf("  -----------+--------------+-----------------+----------------\n");

    for (int s = 0; s < num_strides; s++) {
        int stride = strides[s];
        size_t count = ARRAY_SIZE / stride;
        
        // Warm up / touch memory
        volatile int dummy = 0;
        for (size_t i = 0; i < ARRAY_SIZE; i += stride) {
            dummy += arr[i];
        }
        (void)dummy;

        uint64_t t0 = timer_now_ns();
        volatile int sum = 0;
        for (size_t i = 0; i < ARRAY_SIZE; i += stride) {
            sum += arr[i];
        }
        uint64_t t1 = timer_now_ns();
        do_not_optimize((void *)&sum);

        double elapsed_ms = ns_to_ms(t1 - t0);
        double ns_per_op = (double)(t1 - t0) / (double)count;

        printf("  %-10d | %-12zu | %15.3f | %15.3f ns\n",
               stride, stride * sizeof(int), elapsed_ms, ns_per_op);
    }

    printf("\nKey Takeaway:\n");
    printf("  A cache line on modern CPUs is 64 bytes (16 x 4-byte integers).\n");
    printf("  - For stride <= 16: Multiple accesses share the SAME fetched 64-byte cache line.\n");
    printf("  - For stride >= 16: Every single read touches a DIFFERENT cache line, wasting unused data\n");
    printf("    in the cache line and maximizing memory bus bandwidth pressure!\n");

    aligned_free_64(arr);
    return 0;
}
