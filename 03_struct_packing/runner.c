#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "../common/harness.h"

#ifdef USE_SOLUTION_HEADER
#include "../solutions/03_exercise.h"
#else
#include "exercise.h"
#endif

#define NUM_MONSTERS 8000000
#define RUNS 5

static void process_unoptimized(const UnoptimizedMonster *m, size_t count, double *out_total) {
    double total = 0.0;
    for (size_t i = 0; i < count; i++) {
        if (m[i].is_alive) {
            total += m[i].health;
        }
    }
    *out_total = total;
}

int main(void) {
    print_test_banner(3, "Struct Packing & Memory Layout");
    printf("  UnoptimizedMonster size : %2zu bytes\n", sizeof(UnoptimizedMonster));
    printf("  OptimizedMonster size   : %2zu bytes (Target: <= 32 bytes)\n\n", sizeof(OptimizedMonster));

    if (sizeof(OptimizedMonster) > 32) {
        print_fail("OptimizedMonster is larger than 32 bytes. Reorder the fields in exercise.h!");
        return 1;
    }

    print_pass("Struct layout successfully packed into <= 32 bytes (eliminated internal padding)!");

    size_t unopt_bytes = NUM_MONSTERS * sizeof(UnoptimizedMonster);
    size_t opt_bytes = NUM_MONSTERS * sizeof(OptimizedMonster);

    UnoptimizedMonster *unopt = (UnoptimizedMonster *)aligned_alloc_64(unopt_bytes);
    OptimizedMonster *opt = (OptimizedMonster *)aligned_alloc_64(opt_bytes);

    for (size_t i = 0; i < NUM_MONSTERS; i++) {
        unopt[i].is_alive = (i % 2 == 0) ? 1 : 0;
        unopt[i].monster_id = (uint64_t)i;
        unopt[i].element_type = 1;
        unopt[i].level = (uint16_t)(i % 100);
        unopt[i].health = 100.0;
        unopt[i].is_boss = 0;
        unopt[i].experience = (uint32_t)(i * 10);
        unopt[i].status_effects = 0;

        opt[i].is_alive = unopt[i].is_alive;
        opt[i].monster_id = unopt[i].monster_id;
        opt[i].element_type = unopt[i].element_type;
        opt[i].level = unopt[i].level;
        opt[i].health = unopt[i].health;
        opt[i].is_boss = unopt[i].is_boss;
        opt[i].experience = unopt[i].experience;
        opt[i].status_effects = unopt[i].status_effects;
    }

    double expected_health = 0.0;
    process_unoptimized(unopt, NUM_MONSTERS, &expected_health);

    double actual_health = 0.0;
    process_monsters(opt, NUM_MONSTERS, &actual_health);

    if (fabs(expected_health - actual_health) > 1e-3) {
        print_fail("process_monsters() output mismatch!");
        printf("    Expected health: %.2f, Got: %.2f\n", expected_health, actual_health);
        aligned_free_64(unopt);
        aligned_free_64(opt);
        return 1;
    }

    print_pass("Correctness verified: Aggregation output matches expected reference!");

    // Benchmark Baseline
    uint64_t t0 = timer_now_ns();
    for (int r = 0; r < RUNS; r++) {
        process_unoptimized(unopt, NUM_MONSTERS, &expected_health);
        do_not_optimize(&expected_health);
    }
    uint64_t t1 = timer_now_ns();
    double unopt_ms = ns_to_ms(t1 - t0) / RUNS;

    // Benchmark Optimized
    uint64_t t2 = timer_now_ns();
    for (int r = 0; r < RUNS; r++) {
        process_monsters(opt, NUM_MONSTERS, &actual_health);
        do_not_optimize(&actual_health);
    }
    uint64_t t3 = timer_now_ns();
    double opt_ms = ns_to_ms(t3 - t2) / RUNS;

    print_perf_comparison("Padded Struct (48 bytes)", unopt_ms, "Packed Struct (32 bytes)", opt_ms, 1.15);

    aligned_free_64(unopt);
    aligned_free_64(opt);
    return 0;
}
