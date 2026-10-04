#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "../common/harness.h"
#include "exercise.h"
#include "../solutions/03_exercise.h"

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
    print_compare_banner(3, "Struct Packing: Your Solution vs Reference Solution");
    printf("  UnoptimizedMonster size : %2zu bytes\n", sizeof(UnoptimizedMonster));
    printf("  Your OptimizedMonster   : %2zu bytes (Target: <= 32 bytes)\n", sizeof(OptimizedMonster));
    printf("  Reference Monster size  : %2zu bytes\n\n", sizeof(RefOptimizedMonster));

    if (sizeof(OptimizedMonster) > 32) {
        print_fail("Your OptimizedMonster is larger than 32 bytes. Reorder the fields in exercise.h!");
        return 1;
    }

    print_pass("Struct layout successfully packed into <= 32 bytes!");

    size_t unopt_bytes = NUM_MONSTERS * sizeof(UnoptimizedMonster);
    size_t user_bytes = NUM_MONSTERS * sizeof(OptimizedMonster);
    size_t ref_bytes = NUM_MONSTERS * sizeof(RefOptimizedMonster);

    UnoptimizedMonster *unopt = (UnoptimizedMonster *)aligned_alloc_64(unopt_bytes);
    OptimizedMonster *user_arr = (OptimizedMonster *)aligned_alloc_64(user_bytes);
    RefOptimizedMonster *ref_arr = (RefOptimizedMonster *)aligned_alloc_64(ref_bytes);

    for (size_t i = 0; i < NUM_MONSTERS; i++) {
        unopt[i].is_alive = (i % 2 == 0) ? 1 : 0;
        unopt[i].monster_id = (uint64_t)i;
        unopt[i].element_type = 1;
        unopt[i].level = (uint16_t)(i % 100);
        unopt[i].health = 100.0;
        unopt[i].is_boss = 0;
        unopt[i].experience = (uint32_t)(i * 10);
        unopt[i].status_effects = 0;

        user_arr[i].is_alive = unopt[i].is_alive;
        user_arr[i].monster_id = unopt[i].monster_id;
        user_arr[i].element_type = unopt[i].element_type;
        user_arr[i].level = unopt[i].level;
        user_arr[i].health = unopt[i].health;
        user_arr[i].is_boss = unopt[i].is_boss;
        user_arr[i].experience = unopt[i].experience;
        user_arr[i].status_effects = unopt[i].status_effects;

        ref_arr[i].is_alive = unopt[i].is_alive;
        ref_arr[i].monster_id = unopt[i].monster_id;
        ref_arr[i].element_type = unopt[i].element_type;
        ref_arr[i].level = unopt[i].level;
        ref_arr[i].health = unopt[i].health;
        ref_arr[i].is_boss = unopt[i].is_boss;
        ref_arr[i].experience = unopt[i].experience;
        ref_arr[i].status_effects = unopt[i].status_effects;
    }

    double base_health = 0.0;
    process_unoptimized(unopt, NUM_MONSTERS, &base_health);

    double user_health = 0.0;
    process_monsters(user_arr, NUM_MONSTERS, &user_health);

    double ref_health = 0.0;
    ref_process_monsters(ref_arr, NUM_MONSTERS, &ref_health);

    if (fabs(base_health - user_health) > 1e-3) {
        print_fail("process_monsters() output mismatch!");
        printf("    Expected: %.2f, Got: %.2f\n", base_health, user_health);
        aligned_free_64(unopt);
        aligned_free_64(user_arr);
        aligned_free_64(ref_arr);
        return 1;
    }

    print_pass("Correctness verified: Aggregation output matches expected reference!");

    // Benchmark Baseline
    uint64_t t0 = timer_now_ns();
    for (int r = 0; r < RUNS; r++) {
        process_unoptimized(unopt, NUM_MONSTERS, &base_health);
        do_not_optimize(&base_health);
    }
    uint64_t t1 = timer_now_ns();
    double base_ms = ns_to_ms(t1 - t0) / RUNS;

    // Benchmark User Solution
    uint64_t t2 = timer_now_ns();
    for (int r = 0; r < RUNS; r++) {
        process_monsters(user_arr, NUM_MONSTERS, &user_health);
        do_not_optimize(&user_health);
    }
    uint64_t t3 = timer_now_ns();
    double user_ms = ns_to_ms(t3 - t2) / RUNS;

    // Benchmark Reference Solution
    uint64_t t4 = timer_now_ns();
    for (int r = 0; r < RUNS; r++) {
        ref_process_monsters(ref_arr, NUM_MONSTERS, &ref_health);
        do_not_optimize(&ref_health);
    }
    uint64_t t5 = timer_now_ns();
    double ref_ms = ns_to_ms(t5 - t4) / RUNS;

    print_3way_comparison("Baseline (48B Struct)", base_ms,
                          "Your Solution (exercise.c)", user_ms,
                          "Reference (32B Struct)", ref_ms,
                          1.15);

    aligned_free_64(unopt);
    aligned_free_64(user_arr);
    aligned_free_64(ref_arr);
    return 0;
}
