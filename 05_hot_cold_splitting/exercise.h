#ifndef EXERCISE_05_H
#define EXERCISE_05_H

#include <stddef.h>
#include <stdint.h>

// Hot simulation state (16 bytes)
typedef struct {
    float x, y;
    float health;
    uint32_t active_target_id;
} HotUnit;

// Cold metadata (stored in a separate array, untouched during tick loops)
typedef struct {
    char unit_type_name[64];
    char sound_on_death[64];
    char icon_path[64];
    int kill_count;
    int rank;
} ColdUnit;

void tick_hot_units(HotUnit *units, size_t count, float dt);

#endif // EXERCISE_05_H
