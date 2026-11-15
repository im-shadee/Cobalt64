/**
 * Copyright (c) 2026 Shadee
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "vectors.h"
#include "bool.h"
#include "matrix_pool.h"
#include <stdint.h>
#include <assert.h>
#include <t3d/t3d.h>

typedef struct {
    uint16_t parent_idx;
    uint16_t first_child_idx;
    uint16_t next_sibling_idx;
} transform_hierarchy_t; // 6 bytes

/**
 * Data structure holding the position, rotation and scale of an object as well as its rendering matrix.
 * During reads, the matrix is recalculated only if it's dirty.
 */
typedef struct {
    vec3f_t position, rotation, scale, shear;
    transform_hierarchy_t hierarchy;
    uint16_t render_mtx_index;
    uint8_t is_dirty;
} transform_t; // 60 bytes

/** Data structure holding the rendering matrix of a static object, calculated at initialization. */
typedef struct {
    transform_hierarchy_t hierarchy;
    uint16_t render_mtx_index;
} baked_transform_t; // 8 bytes

/** 
 * Initializes a transform with a specified position, rotation, scale and shearing.
 * returns: A pointer to the created transform or NULL in case of errors.
 * remarks: The render matrix is automatically updated when created.
 */
transform_t* transform_create(vec3f_t pos, vec3f_t rot, vec3f_t scale, vec3f_t shr, transform_hierarchy_t hierarchy);

/** Updates a render matrix with new position, rotation, scale and shearing vectors. */
void update_render_mtx(T3DMat4FP* mtx, vec3f_t pos, vec3f_t rot, vec3f_t scale, vec3f_t shr);

/** 
 * Initializes a baked transform with a specified position, rotation, scale and shearing.
 * returns: A pointer to the created transform or NULL in case of errors.
 * remarks: A baked transform never changes once created. Use for static objects/meshes to save on memory.
 */
baked_transform_t* baked_tr_create(vec3f_t pos, vec3f_t rot, vec3f_t scale, vec3f_t shr, transform_hierarchy_t hierarchy);

#pragma region Setters
/** Sets the position vector of a transform and marks the render matrix as dirty. */
static inline void transform_set_position(transform_t* tr, vec3f_t pos) {
    assert(tr); // Crash if the transform is null

    // Assign the new position to the container inside the transform
    tr->position = pos;

    // Mark the render matrix as dirty so it rebuilds with the new position next rendering pass
    tr->is_dirty = TRUE;
}

/** Sets the rotation vector of a transform and marks the render matrix as dirty. */
static inline void transform_set_rotation(transform_t* tr, vec3f_t angles) {
    assert(tr); // Crash if the transform is null

    // Assign the new angles to the container inside the transform
    tr->rotation = angles;

    // Mark the render matrix as dirty so it rebuilds with the new angles next rendering pass
    tr->is_dirty = TRUE;
}

/** Sets the scale vector of a transform and marks the render matrix as dirty. */
static inline void transform_set_scale(transform_t* tr, vec3f_t scale) {
    assert(tr); // Crash if the transform is null

    // Assign the new scale to the container inside the transform
    tr->scale = scale;

    // Mark the render matrix as dirty so it rebuilds with the new scale next rendering pass
    tr->is_dirty = TRUE;
}

/** Sets the shear vector of a transform and marks the render matrix as dirty. */
static inline void transform_set_shear(transform_t* tr, vec3f_t shear) {
    assert(tr); // Crash if the transform is null

    // Assign the new shear to the container inside the transform
    tr->shear = shear;

    // Mark the render matrix as dirty so it rebuilds with the new shearing next rendering pass
    tr->is_dirty = TRUE;
}

/**
 * Retrieves the transform's rendering matrix from the matrix pool.
 * @return A pointer to the transform's rendering matrix, or NULL in case of errors.
 */
static inline T3DMat4FP* transform_get_matrix(uint16_t index) { return matrix_pool_get(index); }
#pragma endregion

#endif // TRANSFORM_H
