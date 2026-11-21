/**
 * Copyright (c) 2026 Shadee
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef MATH_H
#define MATH_H

#include "vectors.h"
#include <t3d/t3d.h>

#define FIXED_TO_FLOAT 0.0000305185f

static inline float f32_abs(const float f) { return (f < 0) ? -f : f; }

void mtx_update(T3DMat4FP* mtx, const vec3f_t pos, const vec3f_t rot, const vec3f_t scale, const vec3f_t shr);

float mtx_read_float(const T3DMat4FP* fp, const int col, const int row);

void mtx_get_forward(const T3DMat4FP* fp, T3DVec3* out);

void mtx_get_position(const T3DMat4FP* fp, T3DVec3* out);

#endif // MATH_H
