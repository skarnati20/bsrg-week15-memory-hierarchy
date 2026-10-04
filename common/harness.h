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

static inline void print_compare_banner(int exercise_num, const char *title) {
    printf("\n================================================================================\n");
    printf("  EXERCISE %02d COMPARISON: %s\n", exercise_num, title);
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

static inline void print_3way_comparison(const char *baseline_name, double baseline_ms,
                                         const char *user_name, double user_ms,
                                         const char *ref_name, double ref_ms,
                                         double target_speedup) {
    double user_speedup = (user_ms > 0.0) ? (baseline_ms / user_ms) : 1.0;
    double ref_speedup = (ref_ms > 0.0) ? (baseline_ms / ref_ms) : 1.0;
    double efficiency = (ref_ms > 0.0 && user_ms > 0.0) ? ((ref_ms / user_ms) * 100.0) : 0.0;

    printf("\n  3-Way Performance Benchmark:\n");
    printf("  --------------------------------------------------------------------------------\n");
    printf("  1. %-28s : %8.3f ms  ( 1.00x baseline)\n", baseline_name, baseline_ms);
    printf("  2. %-28s : %8.3f ms  (%5.2fx speedup)  [%5.1f%% of reference speed]\n", user_name, user_ms, user_speedup, efficiency);
    printf("  3. %-28s : %8.3f ms  (%5.2fx speedup)\n", ref_name, ref_ms, ref_speedup);
    printf("  --------------------------------------------------------------------------------\n");

    if (efficiency >= 95.0) {
        printf("\033[1;32m  >>> Outstanding!\033[0m Your solution is performing at full reference speed!\n");
    } else if (user_speedup >= target_speedup) {
        printf("\033[1;32m  >>> Target passed!\033[0m Your solution achieved %.2fx speedup (Ref is %.2fx).\n", user_speedup, ref_speedup);
    } else {
        printf("\033[1;33m  >>> Needs Optimization:\033[0m Your solution reached %.2fx vs reference %.2fx.\n", user_speedup, ref_speedup);
    }
}

#endif // HARNESS_H
