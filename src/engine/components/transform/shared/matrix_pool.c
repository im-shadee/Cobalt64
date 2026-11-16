/**
 * Copyright (c) 2026 Shadee
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "matrix_pool.h"
#include "trig.h"
#include <t3d/t3d.h>
#include <libdragon.h>
#include <stdint.h>
#include <assert.h>

static T3DMat4FP* matrix_pool;
static uint16_t next_free[MAX_TRANSFORMS];

static uint16_t free_head;
static uint16_t next_unused = 0;

void matrix_pool_init(void) {
    matrix_pool = malloc_uncached(sizeof(T3DMat4FP) * MAX_TRANSFORMS);
    assert(matrix_pool);

    free_head = MTX_NONE;
    next_unused = 0;
}

void matrix_pool_free_pool(void) { 
    free_uncached(matrix_pool);
    matrix_pool = NULL;
}

uint16_t matrix_pool_alloc(void) {
    uint16_t taken;

    // Grab the next free slot
    if (free_head != MTX_NONE) {
        taken = free_head;
        free_head = next_free[taken];
        return taken;
    }

    if (next_unused < MAX_TRANSFORMS) {
        taken = next_unused;
        next_unused++;
        return taken;
    }

    // We can't run out of matrices ever (1:1 transform/matrix), so if this happens, crash
    assert(0);
    return MTX_NONE;
}

void matrix_pool_free(uint16_t index) {
    // A slot that was never handed out can't be returned; verify that index is valid
    assert(index < next_unused);

    next_free[index] = free_head;
    free_head = index;
}

T3DMat4FP* matrix_pool_get(uint16_t index) {
    assert(index < next_unused);
    return &matrix_pool[index];
}

void matrix_pool_update_mtx(uint16_t mtx_idx, vec3f_t pos, vec3f_t rot, vec3f_t scale, vec3f_t shr) {
    // Convert float degrees to 16-bit BAMs
    uint16_t x_bam = (uint16_t)(int32_t)(rot.x * DEG_TO_BAM);
    uint16_t y_bam = (uint16_t)(int32_t)(rot.y * DEG_TO_BAM);
    uint16_t z_bam = (uint16_t)(int32_t)(rot.z * DEG_TO_BAM);

    // Fetch fixed-point values from LUT and scale them
    float cx = fixed_cos(x_bam) * FIXED_TO_FLOAT;
    float sx = fixed_sin(x_bam) * FIXED_TO_FLOAT;
    
    float cy = fixed_cos(y_bam) * FIXED_TO_FLOAT;
    float sy = fixed_sin(y_bam) * FIXED_TO_FLOAT;
    
    float cz = fixed_cos(z_bam) * FIXED_TO_FLOAT;
    float sz = fixed_sin(z_bam) * FIXED_TO_FLOAT;

    // Compute rotation matrix values (compute matrix indexes instead of creating matrices)
    float r00 = cy * cz;
    float r01 = -cy * sz;
    float r02 = sy;
    
    float r10 = sx * sy * cz + cx * sz;
    float r11 = -sx * sy * sz + cx * cz;
    float r12 = -sx * cy;
    
    float r20 = -cx * sy * cz + sx * sz;
    float r21 = cx * sy * sz + sx * cz;
    float r22 = cx * cy;

    // Compute scale and shearing combined
    float s00 = scale.x;
    float s01 = shr.x * scale.y;
    float s02 = shr.y * scale.z;

    float s11 = scale.y;
    float s12 = shr.z * scale.z;

    float s22 = scale.z;

    // T3D uses row-vector convention: translation in m[3][0..2]
    // Matrix below is column-vector, so it is written transposed
    T3DMat4 m;
    float a[4][4] = {
        { s00*r00 + s01*r10 + s02*r20, s00*r01 + s01*r11 + s02*r21, s00*r02 + s01*r12 + s02*r22, pos.x },
        { s11*r10 + s12*r20,           s11*r11 + s12*r21,           s11*r12 + s12*r22,           pos.y },
        { s22*r20,                     s22*r21,                     s22*r22,                     pos.z },
        { 0, 0, 0, 1 }
    };

    #pragma GCC unroll 4
    for (int r = 0; r < 4; r++)
        for (int c = 0; c < 4; c++)
            m.m[c][r] = a[r][c];

    // Convert standard float matrix to Tiny3D's hardware fixed-point format
    t3d_mat4_to_fixed(&matrix_pool[mtx_idx], &m);
}
