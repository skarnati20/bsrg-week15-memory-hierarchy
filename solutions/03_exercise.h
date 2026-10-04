#ifndef SOLUTIONS_03_EXERCISE_H
#define SOLUTIONS_03_EXERCISE_H

#include <stdint.h>
#include <stddef.h>

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

// SOLUTION: Packed struct layout (32 bytes total)
// Ordered by natural alignment: 8-byte -> 4-byte -> 2-byte -> 1-byte
typedef struct {
    // 8-byte types
    uint64_t  monster_id;      // 8 bytes (offset 0)
    double    health;          // 8 bytes (offset 8)
    // 4-byte types
    uint32_t  experience;      // 4 bytes (offset 16)
    // 2-byte types
    uint16_t  level;           // 2 bytes (offset 20)
    // 1-byte types
    uint8_t   is_alive;        // 1 byte  (offset 22)
    uint8_t   element_type;    // 1 byte  (offset 23)
    uint8_t   is_boss;         // 1 byte  (offset 24)
    uint8_t   status_effects;  // 1 byte  (offset 25)
    // 6 bytes tail padding to maintain 8-byte struct alignment
} OptimizedMonster;

void process_monsters(const OptimizedMonster *monsters, size_t count, double *out_total_health);

#endif // SOLUTIONS_03_EXERCISE_H
