#include "exercise.h"

/*
 * =============================================================================
 * EXERCISE 03: Memory Layout & Struct Packing (Andrew Kelley DoD Principle)
 * =============================================================================
 * 
 * PROBLEM:
 * C compilers align members to multiples of their natural size. Arbitrary field
 * order creates padding holes. UnoptimizedMonster takes 48 bytes despite having
 * only 26 bytes of actual data (45% wasted space).
 *
 * TASK:
 * 1. Reorder the fields of OptimizedMonster in `exercise.h` so that 8-byte types
 *    come first, followed by 4-byte, 2-byte, and 1-byte types.
 * 2. Shrink the struct size from 48 bytes to 32 bytes or fewer!
 * 3. Implement process_monsters() to sum the health of all alive monsters.
 */

void process_monsters(const OptimizedMonster *monsters, size_t count, double *out_total_health) {
    /* TODO: Iterate through monsters and sum 'health' for all where 'is_alive != 0' */
    double total = 0.0;
    (void)monsters;
    (void)count;
    *out_total_health = total;
}
