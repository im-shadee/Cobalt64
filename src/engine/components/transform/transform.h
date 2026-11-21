/**
 * Copyright (c) 2026 Shadee
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "vectors.h"
#include <stdint.h>
#include <stdbool.h>

/**
 * Data structure holding the position, rotation and scale of an object as well as an index to its rendering matrix.
 * During reads, the matrix is recalculated only if it's dirty.
 */
typedef struct {
    vec3f_t position, rotation, scale, shear;

    struct {
        uint16_t parent_idx;
        uint16_t first_child_idx;
        uint16_t next_sibling_idx;
    } hierarchy_tree; // 6 bytes

    bool is_dirty;
    uint8_t padding;
} Transform; // 56 bytes

/** Sets the position vector of a transform and marks it as dirty. */
void transform_set_position(Transform* tr, const vec3f_t pos);

/** Sets the rotation vector of a transform and marks it as dirty. */
void transform_set_rotation(Transform* tr, const vec3f_t angles);

/** Sets the scale vector of a transform and marks it as dirty. */
void transform_set_scale(Transform* tr, const vec3f_t scale);

/** Sets the shear vector of a transform and marks it as dirty. */
void transform_set_shear(Transform* tr, const vec3f_t shear);

#endif // TRANSFORM_H
