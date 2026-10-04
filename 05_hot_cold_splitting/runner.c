#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "../common/harness.h"
#include "exercise.h"

#define NUM_UNITS 1000000
#define RUNS 5
#define DT 0.016f

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
    print_test_banner(5, "Hot / Cold Data Splitting");
    printf("  MonolithicUnit size : %3zu bytes (Working set: %.2f MB)\n",
           sizeof(MonolithicUnit), (double)(NUM_UNITS * sizeof(MonolithicUnit)) / (1024.0 * 1024.0));
    printf("  HotUnit size        : %3zu bytes (Working set: %.2f MB)\n\n",
           sizeof(HotUnit), (double)(NUM_UNITS * sizeof(HotUnit)) / (1024.0 * 1024.0));

    MonolithicUnit *mono = (MonolithicUnit *)aligned_alloc_64(NUM_UNITS * sizeof(MonolithicUnit));
    HotUnit *hot = (HotUnit *)aligned_alloc_64(NUM_UNITS * sizeof(HotUnit));

    for (size_t i = 0; i < NUM_UNITS; i++) {
        mono[i].x = 0.0f;
        mono[i].y = 0.0f;
        mono[i].health = 100.0f;
        mono[i].active_target_id = (uint32_t)i;

        hot[i].x = mono[i].x;
        hot[i].y = mono[i].y;
        hot[i].health = mono[i].health;
        hot[i].active_target_id = mono[i].active_target_id;
    }

    // Run one step to test correctness
    tick_monolithic(mono, NUM_UNITS, DT);
    tick_hot_units(hot, NUM_UNITS, DT);

    bool passed = true;
    for (size_t i = 0; i < 100; i++) {
        if (fabsf(mono[i].x - hot[i].x) > 1e-3f || fabsf(mono[i].health - hot[i].health) > 1e-3f) {
            print_fail("State mismatch after tick_hot_units()!");
            printf("    At unit %zu: Expected x=%.3f, health=%.3f | Got x=%.3f, health=%.3f\n",
                   i, mono[i].x, mono[i].health, hot[i].x, hot[i].health);
            passed = false;
            break;
        }
    }

    if (!passed) {
        aligned_free_64(mono);
        aligned_free_64(hot);
        return 1;
    }

    print_pass("Correctness verified: Simulation state matches monolithic reference!");

    // Benchmark Monolithic
    uint64_t t0 = timer_now_ns();
    for (int r = 0; r < RUNS; r++) {
        tick_monolithic(mono, NUM_UNITS, DT);
    }
    uint64_t t1 = timer_now_ns();
    double mono_ms = ns_to_ms(t1 - t0) / RUNS;
    do_not_optimize(mono);

    // Benchmark Hot Split
    uint64_t t2 = timer_now_ns();
    for (int r = 0; r < RUNS; r++) {
        tick_hot_units(hot, NUM_UNITS, DT);
    }
    uint64_t t3 = timer_now_ns();
    double hot_ms = ns_to_ms(t3 - t2) / RUNS;
    do_not_optimize(hot);

    print_perf_comparison("Monolithic Entity (216B)", mono_ms, "Hot/Cold Split (16B)", hot_ms, 5.0);

    aligned_free_64(mono);
    aligned_free_64(hot);
    return 0;
}
