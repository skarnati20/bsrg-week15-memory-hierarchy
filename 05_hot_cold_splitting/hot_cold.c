#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "../common/benchmark.h"

#define NUM_NPCS 1000000
#define RUNS 5

// -----------------------------------------------------------------------------
// Unsplit Monolithic Entity (256 bytes per entity)
// In a per-frame combat tick, only health, shield, and cooldown are updated.
// Cold metadata bloats the struct to 256 bytes (4 entire cache lines per entity!).
// -----------------------------------------------------------------------------
typedef struct {
    // HOT DATA (16 bytes)
    float health;
    float shield;
    float cooldown;
    uint32_t status_flags;

    // COLD DATA (240 bytes) - rarely accessed (only on UI open / dialogue / death)
    char name[64];
    char dialogue_tree_id[64];
    char sound_bank_path[64];
    uint32_t inventory_item_ids[12];
} MonolithicNPC;

// -----------------------------------------------------------------------------
// Hot/Cold Split Entities
// Hot struct is only 16 bytes (Fits 4 entities in a single 64B cache line!)
// -----------------------------------------------------------------------------
typedef struct {
    float health;
    float shield;
    float cooldown;
    uint32_t status_flags;
} HotNPC;

typedef struct {
    char name[64];
    char dialogue_tree_id[64];
    char sound_bank_path[64];
    uint32_t inventory_item_ids[12];
} ColdNPC;

// Simulate a combat tick: Regenerate shields and decrement cooldowns
static void tick_monolithic(MonolithicNPC *npcs, size_t count, float dt) {
    for (size_t i = 0; i < count; i++) {
        if (npcs[i].health > 0.0f) {
            npcs[i].shield += 5.0f * dt;
            if (npcs[i].cooldown > 0.0f) {
                npcs[i].cooldown -= dt;
            }
        }
    }
}

static void tick_split(HotNPC *hot_npcs, size_t count, float dt) {
    for (size_t i = 0; i < count; i++) {
        if (hot_npcs[i].health > 0.0f) {
            hot_npcs[i].shield += 5.0f * dt;
            if (hot_npcs[i].cooldown > 0.0f) {
                hot_npcs[i].cooldown -= dt;
            }
        }
    }
}

int main(void) {
    print_header("05 - Hot/Cold Data Splitting");
    printf("NPC Count: %d\n", NUM_NPCS);
    printf("Monolithic NPC struct size: %3zu bytes (4 cache lines per entity)\n", sizeof(MonolithicNPC));
    printf("Hot NPC struct size       : %3zu bytes (4 entities per cache line!)\n\n", sizeof(HotNPC));

    size_t mono_bytes = NUM_NPCS * sizeof(MonolithicNPC);
    size_t hot_bytes = NUM_NPCS * sizeof(HotNPC);
    size_t cold_bytes = NUM_NPCS * sizeof(ColdNPC);

    printf("Total memory (Monolithic) : %.2f MB\n", (double)mono_bytes / (1024.0 * 1024.0));
    printf("Hot buffer memory (Split) : %.2f MB\n\n", (double)hot_bytes / (1024.0 * 1024.0));

    MonolithicNPC *mono = (MonolithicNPC *)aligned_alloc_64(mono_bytes);
    HotNPC *hot = (HotNPC *)aligned_alloc_64(hot_bytes);
    ColdNPC *cold = (ColdNPC *)aligned_alloc_64(cold_bytes);

    for (size_t i = 0; i < NUM_NPCS; i++) {
        mono[i].health = 100.0f;
        mono[i].shield = 50.0f;
        mono[i].cooldown = 1.5f;
        mono[i].status_flags = 1;
        snprintf(mono[i].name, sizeof(mono[i].name), "NPC_Guard_%zu", i);

        hot[i].health = mono[i].health;
        hot[i].shield = mono[i].shield;
        hot[i].cooldown = mono[i].cooldown;
        hot[i].status_flags = mono[i].status_flags;
        snprintf(cold[i].name, sizeof(cold[i].name), "%s", mono[i].name);
    }

    // Benchmark Monolithic
    uint64_t t0 = timer_now_ns();
    for (int r = 0; r < RUNS; r++) {
        tick_monolithic(mono, NUM_NPCS, 0.016f);
    }
    uint64_t t1 = timer_now_ns();
    double mono_ms = ns_to_ms(t1 - t0) / RUNS;
    do_not_optimize(mono);

    // Benchmark Hot/Cold Split
    uint64_t t2 = timer_now_ns();
    for (int r = 0; r < RUNS; r++) {
        tick_split(hot, NUM_NPCS, 0.016f);
    }
    uint64_t t3 = timer_now_ns();
    double split_ms = ns_to_ms(t3 - t2) / RUNS;
    do_not_optimize(hot);

    print_bench_result("Monolithic Entities (256 bytes)", mono_ms, mono_ms, mono_bytes);
    print_bench_result("Hot/Cold Split (16 bytes hot)  ", split_ms, mono_ms, hot_bytes);

    printf("\nExplanation:\n");
    printf("  In the monolithic struct, updating 16 bytes of hot combat data forces the CPU\n");
    printf("  to stream 256 bytes of memory per NPC through the L1/L2 caches (16x cache bloat!).\n");
    printf("  Splitting cold metadata (dialogue, names, sound paths) into a secondary array\n");
    printf("  keeps the tick loop's working set inside fast L1/L2 cache lines.\n");

    aligned_free_64(mono);
    aligned_free_64(hot);
    aligned_free_64(cold);
    return 0;
}
