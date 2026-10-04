#ifndef HARNESS_H
#define HARNESS_H

#include "benchmark.h"
#include <stdbool.h>

// Pretty test report helpers
static inline void print_test_banner(int exercise_num, const char *title) {
    printf("\n================================================================================\n");
    printf("  EXERCISE %02d: %s\n", exercise_num, title);
    printf("================================================================================\n");
}

static inline void print_pass(const char *msg) {
    printf("\033[1;32m  [PASS]\033[0m %s\n", msg);
}

static inline void print_fail(const char *msg) {
    printf("\033[1;31m  [FAIL]\033[0m %s\n", msg);
}

static inline void print_info(const char *msg) {
    printf("  [INFO] %s\n", msg);
}

static inline void print_perf_comparison(const char *baseline_name, double baseline_ms,
                                        const char *opt_name, double opt_ms,
                                        double target_speedup) {
    double speedup = (opt_ms > 0.0) ? (baseline_ms / opt_ms) : 1.0;
    printf("\n  Performance Results:\n");
    printf("  --------------------------------------------------------------------\n");
    printf("  %-30s : %8.3f ms\n", baseline_name, baseline_ms);
    printf("  %-30s : %8.3f ms  (%5.2fx speedup)\n", opt_name, opt_ms, speedup);
    printf("  --------------------------------------------------------------------\n");
    
    if (speedup >= target_speedup) {
        printf("\033[1;32m  Target achieved!\033[0m Reached %.2fx speedup (Target: >= %.1fx)\n", speedup, target_speedup);
    } else {
        printf("\033[1;33m  Notice:\033[0m Speedup is %.2fx (Target: >= %.1fx). Check if your layout or loop can be optimized further.\n", speedup, target_speedup);
    }
}

#endif // HARNESS_H
