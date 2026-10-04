#include <stdio.h>
#include <stdlib.h>
#include "../common/benchmark.h"

#define ROWS 4096
#define COLS 4096
#define ITERATIONS 5

// Row-major traversal (Cache friendly):
// Memory is laid out row by row in contiguous memory.
// Accessing element [r][c] then [r][c+1] means accessing adjacent 4-byte floats.
// Since a CPU cache line is 64 bytes, 1 memory fetch loads 16 consecutive floats.
// Result: 1 cache miss per 16 accesses (93.75% L1 cache hit rate from spatial locality).
static double sum_row_major(const float *matrix, size_t rows, size_t cols) {
    double total = 0.0;
    for (size_t r = 0; r < rows; r++) {
        for (size_t c = 0; c < cols; c++) {
            total += matrix[r * cols + c];
        }
    }
    return total;
}

// Column-major traversal (Cache hostile):
// Accessing [0][c], then [1][c], then [2][c] jumps by (cols * sizeof(float)) bytes = 16,384 bytes!
// Every access jumps far past the 64-byte cache line boundary into a completely different page/line.
// Result: Nearly every single float access triggers an L1/L2 cache miss.
static double sum_col_major(const float *matrix, size_t rows, size_t cols) {
    double total = 0.0;
    for (size_t c = 0; c < cols; c++) {
        for (size_t r = 0; r < rows; r++) {
            total += matrix[r * cols + c];
        }
    }
    return total;
}

int main(void) {
    print_header("01 - Spatial Locality: Row-Major vs Column-Major Traversal");
    
    size_t total_elements = (size_t)ROWS * COLS;
    size_t total_bytes = total_elements * sizeof(float);
    printf("Matrix Size: %d x %d floats (%.2f MB)\n", ROWS, COLS, (double)total_bytes / (1024.0 * 1024.0));
    printf("Iterations : %d runs\n\n", ITERATIONS);

    float *matrix = (float *)aligned_alloc_64(total_bytes);
    if (!matrix) {
        fprintf(stderr, "Failed to allocate memory\n");
        return 1;
    }

    for (size_t i = 0; i < total_elements; i++) {
        matrix[i] = (float)(i % 100) * 0.01f;
    }

    // Warm up
    volatile double warmup = sum_row_major(matrix, ROWS, COLS);
    (void)warmup;

    // Benchmark Row-Major
    uint64_t t0 = timer_now_ns();
    double sum_row = 0.0;
    for (int iter = 0; iter < ITERATIONS; iter++) {
        sum_row += sum_row_major(matrix, ROWS, COLS);
    }
    uint64_t t1 = timer_now_ns();
    double row_ms = ns_to_ms(t1 - t0) / ITERATIONS;
    do_not_optimize(&sum_row);

    // Benchmark Column-Major
    uint64_t t2 = timer_now_ns();
    double sum_col = 0.0;
    for (int iter = 0; iter < ITERATIONS; iter++) {
        sum_col += sum_col_major(matrix, ROWS, COLS);
    }
    uint64_t t3 = timer_now_ns();
    double col_ms = ns_to_ms(t3 - t2) / ITERATIONS;
    do_not_optimize(&sum_col);

    print_bench_result("Column-Major (Cache-Hostile)", col_ms, col_ms, total_bytes);
    print_bench_result("Row-Major (Cache-Friendly)", row_ms, col_ms, total_bytes);

    printf("\nExplanation:\n");
    printf("  In C, 2D arrays are stored linearly in row-major format.\n");
    printf("  Row-major access reads data sequentially, saturating 64-byte cache lines and\n");
    printf("  allowing the CPU's hardware prefetcher to stream data into L1/L2 caches ahead of time.\n");
    printf("  Column-major jumps 16KB per step, thrashing cache lines and stalling the CPU pipeline.\n");

    aligned_free_64(matrix);
    return 0;
}
