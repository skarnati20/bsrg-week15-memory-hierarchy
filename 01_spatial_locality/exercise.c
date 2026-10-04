#include "exercise.h"

/*
 * =============================================================================
 * EXERCISE 01: Spatial Locality - 2D Matrix Filtering
 * =============================================================================
 * 
 * PROBLEM:
 * A naive image blur filter iterates column-by-column (looping x on the outer
 * loop, and y on the inner loop). Since 2D arrays in C are stored linearly in
 * row-major order, this causes the CPU to jump by (width * 4 bytes) on every step,
 * loading a 64-byte cache line from RAM only to use 4 bytes and discard 60 bytes!
 *
 * TASK:
 * Implement blur_optimized() using ROW-MAJOR traversal (y outer, x inner).
 *
 * REQUIREMENTS:
 * 1. For all interior pixels (1 <= y < height - 1, 1 <= x < width - 1):
 *    dst[y * width + x] = (sum of all 9 pixels in the 3x3 window centered at (x, y)) / 9.0f;
 * 2. Traverse memory sequentially so consecutive reads hit the same 64-byte cache line.
 * 3. Target speedup: >= 4.0x faster than column-major baseline.
 */
void blur_optimized(const float *src, float *dst, int width, int height) {
    /* TODO: Implement your cache-friendly row-major 3x3 blur filter here! */
    (void)src;
    (void)dst;
    (void)width;
    (void)height;
}
