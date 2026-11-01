#ifndef MATRICES_H
#define MATRICES_H

#include "custom_int.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "vectors.h"

/** 32-bit float 4x4 matrix for CPU math and physics */
typedef union {
    float m_flat[16]; // Flat array for hardware / DMA / OpenGL-style passing
    float m[4][4]; // 2D array for readable index-based math (row/col)
} mat4f; // 64 bytes

/** Signed 16.16 fixed-point 4x4 matrix for N64 hardware / RSP pipelines */
typedef union {
    int16_16 m_flat[16];
    int16_16 m[4][4];
} mat4; // 64 bytes

/** Shared, readonly instance of the identity matrix. Initialized once into ROM cache. */
extern const mat4f IDENTITY;

/** Turns an existing matrix into the identity matrix. */
static inline void mat4f_identity(mat4f* mtx) { if (mtx) *mtx = IDENTITY; }

/** Performs a scalar multiplication on a matrix with a factor k. */
static inline void mat4f_scale(mat4f* mtx, const float k) {
    if (!mtx) return;

    #pragma GCC unroll 4
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            mtx->m[i][j] *= k;
        }
    }
}

/** Sets each of a matrix slots to 0. */
static inline void mat4f_clear(mat4f* mtx) { if (mtx) memset(mtx, 0, sizeof(*mtx)); }

/** 
 * Creates a 4x4 matrix and initializes its contents.
 * Returns a pointer to the created matrix, or NULL if any allocation failed.
 */
mat4f* mat4f_create();

/** Multiplies a matrix with another and puts the result into a destination matrix. */
void mat4f_multiply(mat4f* dest, const mat4f* a, const mat4f* b);

#endif // MATRICES_H
