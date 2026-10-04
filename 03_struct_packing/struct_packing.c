#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include "../common/benchmark.h"

#define NUM_ITEMS 8000000
#define RUNS 5

// -----------------------------------------------------------------------------
// Unoptimized Struct (Padded with alignment holes)
// Total Size: 32 bytes
// Useful Payload: 15 bytes (17 bytes / 53% wasted padding)
// -----------------------------------------------------------------------------
typedef struct {
    uint8_t  flag_a;   // 1 byte + 7 bytes padding
    uint64_t id;       // 8 bytes
    uint8_t  flag_b;   // 1 byte + 3 bytes padding
    uint32_t count;    // 4 bytes
    uint8_t  flag_c;   // 1 byte + 7 bytes tail padding
} BadEntity;

// -----------------------------------------------------------------------------
// Optimized Struct (Ordered from largest alignment to smallest)
// Total Size: 16 bytes (50% smaller!)
// Fits 4 instances per 64-byte cache line instead of 2.
// -----------------------------------------------------------------------------
typedef struct {
    uint64_t id;       // 8 bytes (offset 0)
    uint32_t count;    // 4 bytes (offset 8)
    uint8_t  flag_a;   // 1 byte  (offset 12)
    uint8_t  flag_b;   // 1 byte  (offset 13)
    uint8_t  flag_c;   // 1 byte  (offset 14)
                       // 1 byte tail padding to align to 16 bytes
} GoodEntity;

static void print_struct_layout(void) {
    printf("--- Memory Layout Comparison ---\n");
    printf("BadEntity  (sizeof = %2zu bytes):\n", sizeof(BadEntity));
    printf("  offset %2zu : flag_a (1 byte)\n", offsetof(BadEntity, flag_a));
    printf("  [7 bytes padding hole]\n");
    printf("  offset %2zu : id     (8 bytes)\n", offsetof(BadEntity, id));
    printf("  offset %2zu : flag_b (1 byte)\n", offsetof(BadEntity, flag_b));
    printf("  [3 bytes padding hole]\n");
    printf("  offset %2zu : count  (4 bytes)\n", offsetof(BadEntity, count));
    printf("  offset %2zu : flag_c (1 byte)\n", offsetof(BadEntity, flag_c));
    printf("  [7 bytes tail padding]\n");
    printf("  Cache line density: %zu entities per 64B cache line\n\n", 64 / sizeof(BadEntity));

    printf("GoodEntity (sizeof = %2zu bytes):\n", sizeof(GoodEntity));
    printf("  offset %2zu : id     (8 bytes)\n", offsetof(GoodEntity, id));
    printf("  offset %2zu : count  (4 bytes)\n", offsetof(GoodEntity, count));
    printf("  offset %2zu : flag_a (1 byte)\n", offsetof(GoodEntity, flag_a));
    printf("  offset %2zu : flag_b (1 byte)\n", offsetof(GoodEntity, flag_b));
    printf("  offset %2zu : flag_c (1 byte)\n", offsetof(GoodEntity, flag_c));
    printf("  [1 byte tail padding]\n");
    printf("  Cache line density: %zu entities per 64B cache line\n\n", 64 / sizeof(GoodEntity));
}

int main(void) {
    print_header("03 - Struct Packing & Field Ordering (Andrew Kelley DoD Principle)");
    print_struct_layout();

    size_t bad_bytes = NUM_ITEMS * sizeof(BadEntity);
    size_t good_bytes = NUM_ITEMS * sizeof(GoodEntity);

    printf("Dataset: %d elements\n", NUM_ITEMS);
    printf("  BadEntity  Array Total Size: %.2f MB\n", (double)bad_bytes / (1024.0 * 1024.0));
    printf("  GoodEntity Array Total Size: %.2f MB\n\n", (double)good_bytes / (1024.0 * 1024.0));

    BadEntity *bad_arr = (BadEntity *)aligned_alloc_64(bad_bytes);
    GoodEntity *good_arr = (GoodEntity *)aligned_alloc_64(good_bytes);

    for (int i = 0; i < NUM_ITEMS; i++) {
        bad_arr[i].flag_a = 1;
        bad_arr[i].id = (uint64_t)i;
        bad_arr[i].flag_b = 2;
        bad_arr[i].count = (uint32_t)i;
        bad_arr[i].flag_c = 3;

        good_arr[i].id = (uint64_t)i;
        good_arr[i].count = (uint32_t)i;
        good_arr[i].flag_a = 1;
        good_arr[i].flag_b = 2;
        good_arr[i].flag_c = 3;
    }

    // Benchmark Bad Layout
    uint64_t t0 = timer_now_ns();
    uint64_t bad_sum = 0;
    for (int r = 0; r < RUNS; r++) {
        for (int i = 0; i < NUM_ITEMS; i++) {
            if (bad_arr[i].flag_a) {
                bad_sum += bad_arr[i].id + bad_arr[i].count + bad_arr[i].flag_b;
            }
        }
    }
    uint64_t t1 = timer_now_ns();
    double bad_ms = ns_to_ms(t1 - t0) / RUNS;
    do_not_optimize(&bad_sum);

    // Benchmark Good Layout
    uint64_t t2 = timer_now_ns();
    uint64_t good_sum = 0;
    for (int r = 0; r < RUNS; r++) {
        for (int i = 0; i < NUM_ITEMS; i++) {
            if (good_arr[i].flag_a) {
                good_sum += good_arr[i].id + good_arr[i].count + good_arr[i].flag_b;
            }
        }
    }
    uint64_t t3 = timer_now_ns();
    double good_ms = ns_to_ms(t3 - t2) / RUNS;
    do_not_optimize(&good_sum);

    print_bench_result("Padded Struct (32 bytes) ", bad_ms, bad_ms, bad_bytes);
    print_bench_result("Packed Struct (16 bytes) ", good_ms, bad_ms, good_bytes);

    printf("\nExplanation:\n");
    printf("  Simply reordering struct members from largest to smallest alignment halved memory from 32B to 16B.\n");
    printf("  This doubles cache-line density (4 items/line vs 2 items/line), cutting memory traffic in half!\n");

    aligned_free_64(bad_arr);
    aligned_free_64(good_arr);
    return 0;
}
