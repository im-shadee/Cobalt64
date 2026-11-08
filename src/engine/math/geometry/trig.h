#ifndef TRIG_H
#define TRIG_H

#include <stdint.h>

#define TRIG_TABLE_SIZE 512 // Table size for a quarter circle (must be a power of 2 for bitmask optimization)
#define TRIG_MASK (TRIG_TABLE_SIZE - 1)

// Automatically compute the bit shift at compile time:
// A quadrant spans 14 bits (16384 entries).
// __builtin_ctz(TRIG_TABLE_SIZE) computes log2(TRIG_TABLE_SIZE) at compile time.
#define TRIG_SHIFT (14 - __builtin_ctz(TRIG_TABLE_SIZE))

#define TRIG_MAX_VAL 32767
#define TRIG_90_DEG_BAM 16384
#define DEG_TO_BAM 182.044444f

#define PI 3.14159265f

extern int16_t sine_table[TRIG_TABLE_SIZE + 1];

/** Fast sine lookup using quarter-circle symmetry and bitmasking. */
static inline int16_t fixed_sin(uint16_t angle) {
    // Extract quadrant and index using bitwise operations
    uint8_t quadrant = (angle >> 14) & 3;
    uint16_t idx = (angle >> TRIG_SHIFT) & TRIG_MASK;

    if (quadrant & 1) idx = TRIG_TABLE_SIZE - idx;

    int16_t val = sine_table[idx];
    return (quadrant >= 2) ? -val : val;
}

/** Cosine is just sine offset by 90 degrees; use the sine lookup function. */
static inline int16_t fixed_cos(uint16_t angle) { return fixed_sin(angle + TRIG_90_DEG_BAM); }

void init_trig_table(void);

#endif // TRIG_H
