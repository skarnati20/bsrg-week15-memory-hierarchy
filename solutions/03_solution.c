#include "03_exercise.h"

void process_monsters(const OptimizedMonster *monsters, size_t count, double *out_total_health) {
    double total = 0.0;
    for (size_t i = 0; i < count; i++) {
        if (monsters[i].is_alive) {
            total += monsters[i].health;
        }
    }
    *out_total_health = total;
}
