#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "../common/harness.h"
#include "exercise.h"

#define WIDTH  2048
#define HEIGHT 2048
#define RUNS   5

// Reference function signature
void ref_blur_optimized(const float *src, float *dst, int width, int height);

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
    print_compare_banner(1, "Spatial Locality: Your Solution vs Reference Solution");

    size_t total_pixels = (size_t)WIDTH * HEIGHT;
    size_t bytes = total_pixels * sizeof(float);

    float *src = (float *)aligned_alloc_64(bytes);
    float *dst_base = (float *)aligned_alloc_64(bytes);
    float *dst_user = (float *)aligned_alloc_64(bytes);
    float *dst_ref = (float *)aligned_alloc_64(bytes);

    for (size_t i = 0; i < total_pixels; i++) {
        src[i] = (float)(i % 256);
        dst_base[i] = 0.0f;
        dst_user[i] = 0.0f;
        dst_ref[i] = 0.0f;
    }

    // Run implementations
    blur_column_major_baseline(src, dst_base, WIDTH, HEIGHT);
    blur_optimized(src, dst_user, WIDTH, HEIGHT);
    ref_blur_optimized(src, dst_ref, WIDTH, HEIGHT);

    // Verify user correctness against reference
    bool passed = true;
    for (int y = 1; y < HEIGHT - 1; y++) {
        for (int x = 1; x < WIDTH - 1; x++) {
            size_t idx = (size_t)y * WIDTH + x;
            if (fabsf(dst_ref[idx] - dst_user[idx]) > 1e-4f) {
                print_fail("Your solution's output does NOT match the reference solution!");
                printf("    At (x=%d, y=%d): Ref=%.4f, Yours=%.4f\n", x, y, dst_ref[idx], dst_user[idx]);
                passed = false;
                break;
            }
        }
        if (!passed) break;
    }

    if (!passed) {
        aligned_free_64(src);
        aligned_free_64(dst_base);
        aligned_free_64(dst_user);
        aligned_free_64(dst_ref);
        return 1;
    }

    print_pass("Correctness verified: Your output exactly matches the reference output!");

    // Benchmark Baseline
    uint64_t t0 = timer_now_ns();
    for (int r = 0; r < RUNS; r++) {
        blur_column_major_baseline(src, dst_base, WIDTH, HEIGHT);
    }
    uint64_t t1 = timer_now_ns();
    double base_ms = ns_to_ms(t1 - t0) / RUNS;

    // Benchmark User Solution
    uint64_t t2 = timer_now_ns();
    for (int r = 0; r < RUNS; r++) {
        blur_optimized(src, dst_user, WIDTH, HEIGHT);
    }
    uint64_t t3 = timer_now_ns();
    double user_ms = ns_to_ms(t3 - t2) / RUNS;

    // Benchmark Reference Solution
    uint64_t t4 = timer_now_ns();
    for (int r = 0; r < RUNS; r++) {
        ref_blur_optimized(src, dst_ref, WIDTH, HEIGHT);
    }
    uint64_t t5 = timer_now_ns();
    double ref_ms = ns_to_ms(t5 - t4) / RUNS;

    print_3way_comparison("Baseline (Column-Major)", base_ms,
                          "Your Solution (exercise.c)", user_ms,
                          "Reference (Row-Major)", ref_ms,
                          4.0);

    aligned_free_64(src);
    aligned_free_64(dst_base);
    aligned_free_64(dst_user);
    aligned_free_64(dst_ref);
    return 0;
}
