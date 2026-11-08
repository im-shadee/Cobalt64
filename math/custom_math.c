#include "custom_math.h"
#include <math.h>

float sin_lut[LUT_SIZE];
float cos_lut[LUT_SIZE];

void init_trigonometry_tables() {
    for (int i = 0; i < LUT_SIZE; i++) {
        float rad = ((float)i / LUT_SIZE) * 2 * PI;
        sin_lut[i] = sinf(rad);
        cos_lut[i] = cosf(rad);
    }
}
