#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "../common/harness.h"
#include "exercise.h"

#define NUM_UNITS 1000000
#define RUNS 5
#define DT 0.016f

void ref_tick_hot_units(HotUnit *units, size_t count, float dt);

typedef struct {
    // Hot data (16 bytes)
    float x, y;
    float health;
    uint32_t active_target_id;

    // Cold metadata (200 bytes)
    char unit_type_name[64];
    char sound_on_death[64];
    char icon_path[64];
    int kill_count;
    int rank;
} MonolithicUnit;

static void tick_monolithic(MonolithicUnit *units, size_t count, float dt) {
    for (size_t i = 0; i < count; i++) {
        if (units[i].health > 0.0f) {
            units[i].x += 10.0f * dt;
            units[i].y += 10.0f * dt;
            units[i].health -= 1.0f * dt;
        }
    }
}

int main(void) {
    print_compare_banner(5, "Hot/Cold Splitting: Your Solution vs Reference Solution");

    MonolithicUnit *mono = (MonolithicUnit *)aligned_alloc_64(NUM_UNITS * sizeof(MonolithicUnit));
    HotUnit *user_hot = (HotUnit *)aligned_alloc_64(NUM_UNITS * sizeof(HotUnit));
    HotUnit *ref_hot = (HotUnit *)aligned_alloc_64(NUM_UNITS * sizeof(HotUnit));

    for (size_t i = 0; i < NUM_UNITS; i++) {
        mono[i].x = 0.0f;
        mono[i].y = 0.0f;
        mono[i].health = 100.0f;
        mono[i].active_target_id = (uint32_t)i;

        user_hot[i].x = mono[i].x;
        user_hot[i].y = mono[i].y;
        user_hot[i].health = mono[i].health;
        user_hot[i].active_target_id = mono[i].active_target_id;

        ref_hot[i].x = mono[i].x;
        ref_hot[i].y = mono[i].y;
        ref_hot[i].health = mono[i].health;
        ref_hot[i].active_target_id = mono[i].active_target_id;
    }

    // Run one step to test correctness
    tick_monolithic(mono, NUM_UNITS, DT);
    tick_hot_units(user_hot, NUM_UNITS, DT);
    ref_tick_hot_units(ref_hot, NUM_UNITS, DT);

    bool passed = true;
    for (size_t i = 0; i < 100; i++) {
        if (fabsf(ref_hot[i].x - user_hot[i].x) > 1e-3f || fabsf(ref_hot[i].health - user_hot[i].health) > 1e-3f) {
            print_fail("State mismatch between your solution and reference solution!");
            printf("    At unit %zu: Ref x=%.3f, health=%.3f | Yours x=%.3f, health=%.3f\n",
                   i, ref_hot[i].x, ref_hot[i].health, user_hot[i].x, user_hot[i].health);
            passed = false;
            break;
        }
    }

    if (!passed) {
        aligned_free_64(mono);
        aligned_free_64(user_hot);
        aligned_free_64(ref_hot);
        return 1;
    }

    print_pass("Correctness verified: Simulation state matches reference implementation!");

    // Benchmark Monolithic Baseline
    uint64_t t0 = timer_now_ns();
    for (int r = 0; r < RUNS; r++) {
        tick_monolithic(mono, NUM_UNITS, DT);
    }
    uint64_t t1 = timer_now_ns();
    double mono_ms = ns_to_ms(t1 - t0) / RUNS;
    do_not_optimize(mono);

    // Benchmark User Solution
    uint64_t t2 = timer_now_ns();
    for (int r = 0; r < RUNS; r++) {
        tick_hot_units(user_hot, NUM_UNITS, DT);
    }
    uint64_t t3 = timer_now_ns();
    double user_ms = ns_to_ms(t3 - t2) / RUNS;
    do_not_optimize(user_hot);

    // Benchmark Reference Solution
    uint64_t t4 = timer_now_ns();
    for (int r = 0; r < RUNS; r++) {
        ref_tick_hot_units(ref_hot, NUM_UNITS, DT);
    }
    uint64_t t5 = timer_now_ns();
    double ref_ms = ns_to_ms(t5 - t4) / RUNS;
    do_not_optimize(ref_hot);

    print_3way_comparison("Baseline (216B Monolithic)", mono_ms,
                          "Your Solution (exercise.c)", user_ms,
                          "Reference (16B Hot)", ref_ms,
                          5.0);

    aligned_free_64(mono);
    aligned_free_64(user_hot);
    aligned_free_64(ref_hot);
    return 0;
}
