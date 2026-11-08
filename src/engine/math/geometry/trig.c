#include "trig.h"
#include <math.h>

int16_t sine_table[TRIG_TABLE_SIZE + 1] = {0};

void init_trig_table(void) {
    for (int i = 0; i <= TRIG_TABLE_SIZE; i++) {
        float angle = ((float)i / (float)TRIG_TABLE_SIZE) * (PI / 2.0f);
        sine_table[i] = (int16_t)(sinf(angle) * TRIG_MAX_VAL);
    }
}
