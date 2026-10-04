#include "../01_spatial_locality/exercise.h"

void blur_optimized(const float *src, float *dst, int width, int height) {
    for (int y = 1; y < height - 1; y++) {
        const float *row_prev = &src[(y - 1) * width];
        const float *row_curr = &src[y * width];
        const float *row_next = &src[(y + 1) * width];
        float *dst_row = &dst[y * width];

        for (int x = 1; x < width - 1; x++) {
            float sum = row_prev[x - 1] + row_prev[x] + row_prev[x + 1]
                      + row_curr[x - 1] + row_curr[x] + row_curr[x + 1]
                      + row_next[x - 1] + row_next[x] + row_next[x + 1];
            dst_row[x] = sum * (1.0f / 9.0f);
        }
    }
}
