#include "exercise.h"

/*
 * =============================================================================
 * EXERCISE 05: Hot / Cold Data Splitting
 * =============================================================================
 * 
 * PROBLEM:
 * A game unit struct containing sound paths, icon names, and lore strings takes
 * 216 bytes. During combat ticks, only position (x, y) and health are modified.
 * Iterating through 1,000,000 units floods L1/L2/L3 caches with 200MB+ of unused strings!
 *
 * TASK:
 * Implement tick_hot_units() on the 16-byte HotUnit array.
 *
 * REQUIREMENTS:
 * 1. For each unit with health > 0:
 *       x += 10.0f * dt;
 *       y += 10.0f * dt;
 *       health -= 1.0f * dt;
 * 2. 16-byte struct fits 4 units per 64-byte cache line (100% cache density!).
 * 3. Target speedup: >= 5.0x faster than monolithic 216-byte entity.
 */

void tick_hot_units(HotUnit *units, size_t count, float dt) {
    /* TODO: Implement fast simulation loop over hot unit state */
    (void)units;
    (void)count;
    (void)dt;
}
