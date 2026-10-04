#ifndef EXERCISE_03_H
#define EXERCISE_03_H

#include <stdint.h>
#include <stddef.h>

// Unoptimized monster struct (48 bytes due to alignment holes)
typedef struct {
    uint8_t   is_alive;        // 1 byte + 7 bytes padding
    uint64_t  monster_id;      // 8 bytes
    uint8_t   element_type;    // 1 byte + 1 byte padding
    uint16_t  level;           // 2 bytes + 4 bytes padding
    double    health;          // 8 bytes
    uint8_t   is_boss;         // 1 byte + 3 bytes padding
    uint32_t  experience;      // 4 bytes
    uint8_t   status_effects;  // 1 byte + 7 bytes tail padding
} UnoptimizedMonster;

// EXERCISE: Reorder fields in OptimizedMonster to eliminate padding holes
typedef struct {
    /* TODO: Reorder fields from largest to smallest alignment requirement */
    uint8_t   is_alive;
    uint64_t  monster_id;
    uint8_t   element_type;
    uint16_t  level;
    double    health;
    uint8_t   is_boss;
    uint32_t  experience;
    uint8_t   status_effects;
} OptimizedMonster;

void process_monsters(const OptimizedMonster *monsters, size_t count, double *out_total_health);

#endif // EXERCISE_03_H
