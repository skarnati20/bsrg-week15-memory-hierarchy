#include "../05_hot_cold_splitting/exercise.h"

void tick_hot_units(HotUnit * restrict units, size_t count, float dt) {
    for (size_t i = 0; i < count; i++) {
        if (units[i].health > 0.0f) {
            units[i].x += 10.0f * dt;
            units[i].y += 10.0f * dt;
            units[i].health -= 1.0f * dt;
        }
    }
}
