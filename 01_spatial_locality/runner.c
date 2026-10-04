#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "../common/harness.h"
#include "exercise.h"

#define WIDTH  2048
#define HEIGHT 2048
#define RUNS   5

static void blur_column_major_baseline(const float *src, float *dst, int width, int height) {
    for (int x = 1; x < width - 1; x++) {
        for (int y = 1; y < height - 1; y++) {
            float sum = 0.0f;
            for (int dy = -1; dy <= 1; dy++) {
                for (int dx = -1; dx <= 1; dx++) {
                    sum += src[(y + dy) * width + (x + dx)];
                }
            }
            dst[y * width + x] = sum / 9.0f;
        }
    }
}

int main(void) {
    print_test_banner(1, "Spatial Locality (2D Filter)");
    print_info("Testing 2048 x 2048 image blur filter across 5 iterations...");

    size_t total_pixels = (size_t)WIDTH * HEIGHT;
    size_t bytes = total_pixels * sizeof(float);

    float *src = (float *)aligned_alloc_64(bytes);
    float *dst_expected = (float *)aligned_alloc_64(bytes);
    float *dst_actual = (float *)aligned_alloc_64(bytes);

    for (size_t i = 0; i < total_pixels; i++) {
        src[i] = (float)(i % 256);
        dst_expected[i] = 0.0f;
        dst_actual[i] = 0.0f;
    }

    // Generate expected reference
    blur_column_major_baseline(src, dst_expected, WIDTH, HEIGHT);

    // Run student code once to verify correctness
    blur_optimized(src, dst_actual, WIDTH, HEIGHT);

    // Verify interior pixels
    bool passed = true;
    for (int y = 1; y < HEIGHT - 1; y++) {
        for (int x = 1; x < WIDTH - 1; x++) {
            size_t idx = (size_t)y * WIDTH + x;
            if (fabsf(dst_expected[idx] - dst_actual[idx]) > 1e-4f) {
                print_fail("Result mismatch found!");
                printf("    At (x=%d, y=%d): Expected %.4f, Got %.4f\n", x, y, dst_expected[idx], dst_actual[idx]);
                passed = false;
                break;
            }
        }
        if (!passed) break;
    }

    if (!passed) {
        aligned_free_64(src);
        aligned_free_64(dst_expected);
        aligned_free_64(dst_actual);
        return 1;
    }

    print_pass("Correctness verified: All filtered pixel values match expected reference!");

    // Benchmark Baseline
    uint64_t t0 = timer_now_ns();
    for (int r = 0; r < RUNS; r++) {
        blur_column_major_baseline(src, dst_expected, WIDTH, HEIGHT);
    }
    uint64_t t1 = timer_now_ns();
    double baseline_ms = ns_to_ms(t1 - t0) / RUNS;

    // Benchmark Student Solution
    uint64_t t2 = timer_now_ns();
    for (int r = 0; r < RUNS; r++) {
        blur_optimized(src, dst_actual, WIDTH, HEIGHT);
    }
    uint64_t t3 = timer_now_ns();
    double opt_ms = ns_to_ms(t3 - t2) / RUNS;

    print_perf_comparison("Column-Major (Baseline)", baseline_ms, "Optimized Solution", opt_ms, 4.0);

    aligned_free_64(src);
    aligned_free_64(dst_expected);
    aligned_free_64(dst_actual);
    return 0;
}
