/**
 * Copyright (c) 2026 Shadee
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "cobalt_math.h"
#include "trig.h"
#include "vectors.h"
#include <t3d/t3d.h>

void mtx_update(T3DMat4FP* mtx, const vec3f_t pos, const vec3f_t rot, const vec3f_t scale, const vec3f_t shr) {
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
        {s00 * r00 + s01 * r10 + s02 * r20, s00 * r01 + s01 * r11 + s02 * r21, s00 * r02 + s01 * r12 + s02 * r22, pos.x},
        {s11 * r10 + s12 * r20, s11 * r11 + s12 * r21, s11 * r12 + s12 * r22, pos.y},
        {s22 * r20, s22 * r21, s22 * r22, pos.z},
        {0, 0, 0, 1}
    };

    #pragma GCC unroll 4
    for (int r = 0; r < 4; r++)
        for (int c = 0; c < 4; c++)
            m.m[c][r] = a[r][c];

    // Convert standard float matrix to Tiny3D's hardware fixed-point format
    t3d_mat4_to_fixed(mtx, &m);
}

float mtx_read_float(const T3DMat4FP* fp, const int col, const int row) {
    int32_t raw = ((int32_t)fp->m[row].i[col] << 16) | (uint32_t)fp->m[row].f[col];
    return (float)raw * 0.00001525878f;
}

void mtx_get_forward(const T3DMat4FP* fp, T3DVec3* out) {
    // Row 2 holds the local +Z basis vector (negated for -Z forward)
    out->v[0] = -mtx_read_float(fp, 2, 0);
    out->v[1] = -mtx_read_float(fp, 2, 1);
    out->v[2] = -mtx_read_float(fp, 2, 2);

    t3d_vec3_norm(out); // Normalize in case the parent had scale
}

void mtx_get_position(const T3DMat4FP* fp, T3DVec3* out) {
    // Row 3 holds world translation in T3DMat4FP
    out->x = mtx_read_float(fp, 0, 3);
    out->y = mtx_read_float(fp, 1, 3);
    out->z = mtx_read_float(fp, 2, 3);
}
