#ifndef MATH_H
#define MATH_H

#define LUT_SIZE 512
#define DEG_TO_INDEX 1.42222222f // LUT_SIZE / 360
#define PI 3.14159265f

#include <math.h>

static inline float f32_abs(float f) { return (f < 0) ? -f : f; }

extern float sin_lut[LUT_SIZE];
extern float cos_lut[LUT_SIZE];

/** Maps and stores sinus and cosinus values from a range of angles between 0 and 512. */
void init_trigonometry_tables();

static inline int angle_to_lut_idx(float deg) {
    // Normalize degrees to the [0, 360] range
    while (deg < 0) deg += 360;
    while (deg >= 360) deg -= 360;

    // Index is calculated using the formula angle / 360 * LUT_SIZE
    deg *= DEG_TO_INDEX;

    // Round up by adding 0.5 so int truncation doesn't drop data
    deg += 0.5f;

    // Safety clamp to ensure the index never exceeds LUT_SIZE - 1
    return (int)deg & (LUT_SIZE - 1);
}

#endif // MATH_H
