#ifndef MATRICES_H
#define MATRICES_H

#include "custom_int.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

/** 32-bit float 4x4 matrix for CPU math and physics */
typedef union {
    float m_flat[16]; // Flat array for hardware / DMA / OpenGL-style passing
    float m[4][4]; // 2D array for readable index-based math (row/col)
} mtx4f_t; // 64 bytes

/** Signed 16.16 fixed-point 4x4 matrix for N64 hardware / RSP pipelines */
typedef union {
    int16_16 m_flat[16];
    int16_16 m[4][4];
} mtx4_t; // 64 bytes

/** Shared, readonly instance of the identity matrix. Initialized once into ROM cache. */
extern const mtx4f_t IDENTITY;

/** Turns an existing matrix into the identity matrix. */
static inline void mat4f_identity(mtx4f_t* mtx) { if (mtx) *mtx = IDENTITY; }

/** Performs a scalar multiplication on a matrix with a factor k. */
static inline void mat4f_scale(mtx4f_t* mtx, const float k) {
    if (!mtx) return;

    #pragma GCC unroll 4
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            mtx->m[i][j] *= k;
        }
    }
}

/** Sets each of a matrix slots to 0. */
static inline void mat4f_clear(mtx4f_t* mtx) { if (mtx) memset(mtx, 0, sizeof(*mtx)); }

/** 
 * Creates a 4x4 matrix and initializes its contents.
 * Returns a pointer to the created matrix, or NULL if any allocation failed.
 */
mtx4f_t* mat4f_create();

/** Multiplies a matrix with another and puts the result into a destination matrix. */
void mat4f_multiply(mtx4f_t* dest, const mtx4f_t* a, const mtx4f_t* b);

#endif // MATRICES_H
