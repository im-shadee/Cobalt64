/**
 * Copyright (c) 2026 Shadee
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "vectors.h"
#include "bool.h"
#include "hierarchy_tree.h"
#include "component.h"
#include <stdint.h>
#include <assert.h>

/**
 * Data structure holding the position, rotation and scale of an object as well as its rendering matrix.
 * During reads, the matrix is recalculated only if it's dirty.
 */
typedef struct {
    vec3f_t position, rotation, scale, shear;
    HierarchyTree hierarchy;
    uint16_t render_mtx_index;
    bool_t is_dirty;
} Transform; // 60 bytes

COMP_DECLARE(transform, Transform)

/** Sets the position vector of a transform and marks the render matrix as dirty. */
static inline void transform_set_position(Transform* tr, vec3f_t pos) {
    assert(tr); // Crash if the transform is null

    // Assign the new position to the container inside the transform
    tr->position = pos;

    // Mark the render matrix as dirty so it rebuilds with the new position next rendering pass
    tr->is_dirty = TRUE;
}

/** Sets the rotation vector of a transform and marks the render matrix as dirty. */
static inline void transform_set_rotation(Transform* tr, vec3f_t angles) {
    assert(tr); // Crash if the transform is null

    // Assign the new angles to the container inside the transform
    tr->rotation = angles;

    // Mark the render matrix as dirty so it rebuilds with the new angles next rendering pass
    tr->is_dirty = TRUE;
}

/** Sets the scale vector of a transform and marks the render matrix as dirty. */
static inline void transform_set_scale(Transform* tr, vec3f_t scale) {
    assert(tr); // Crash if the transform is null

    // Assign the new scale to the container inside the transform
    tr->scale = scale;

    // Mark the render matrix as dirty so it rebuilds with the new scale next rendering pass
    tr->is_dirty = TRUE;
}

/** Sets the shear vector of a transform and marks the render matrix as dirty. */
static inline void transform_set_shear(Transform* tr, vec3f_t shear) {
    assert(tr); // Crash if the transform is null

    // Assign the new shear to the container inside the transform
    tr->shear = shear;

    // Mark the render matrix as dirty so it rebuilds with the new shearing next rendering pass
    tr->is_dirty = TRUE;
}

#endif // TRANSFORM_H
