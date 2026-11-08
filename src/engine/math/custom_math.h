#ifndef MATH_H
#define MATH_H

#define FIXED_TO_FLOAT 0.0000305185f

static inline float f32_abs(float f) { return (f < 0) ? -f : f; }

#endif // MATH_H
