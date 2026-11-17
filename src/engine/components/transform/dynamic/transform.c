/**
 * Copyright (c) 2026 Shadee
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "transform.h"
#include "matrix_pool.h"
#include "component.h"
#include <stdint.h>

void transform_update(void* comps, const uint16_t* owners, uint16_t count, float dt) {
    Transform** transforms = (Transform**)comps;
    Transform* tr;
    
    // Update render matrices for all transforms if dirty
    for (int i = 0; i < count; i++) {
        tr = transforms[i];
        if (!tr->is_dirty) continue;

        matrix_pool_update_mtx(tr->render_mtx_index, tr->position, tr->rotation, tr->scale, tr->shear);
        tr->is_dirty = FALSE;
    }
}

void transform_on_add(uint16_t obj_idx, void* comp) {
    Transform* tr = (Transform*)comp;
    tr->render_mtx_index = matrix_pool_alloc();
    tr->scale = (vec3f_t){0, 0, 0};
    tr->is_dirty = TRUE;
}

void transform_on_remove(uint16_t obj_idx, void* comp) {
    Transform* tr = (Transform*)comp;
    matrix_pool_free(tr->render_mtx_index);
    tr->render_mtx_index = MTX_NONE;
}

void transform_set_position(Transform* tr, const vec3f_t pos) {
    assert(tr); // Crash if the transform is null

    // Assign the new position to the container inside the transform
    tr->position = pos;

    // Mark the render matrix as dirty so it rebuilds with the new position next rendering pass
    tr->is_dirty = TRUE;
}

void transform_set_rotation(Transform* tr, const vec3f_t angles) {
    assert(tr); // Crash if the transform is null

    // Assign the new angles to the container inside the transform
    tr->rotation = angles;

    // Mark the render matrix as dirty so it rebuilds with the new angles next rendering pass
    tr->is_dirty = TRUE;
}

void transform_set_scale(Transform* tr, const vec3f_t scale) {
    assert(tr); // Crash if the transform is null

    // Assign the new scale to the container inside the transform
    tr->scale = scale;

    // Mark the render matrix as dirty so it rebuilds with the new scale next rendering pass
    tr->is_dirty = TRUE;
}

void transform_set_shear(Transform* tr, const vec3f_t shear) {
    assert(tr); // Crash if the transform is null

    // Assign the new shear to the container inside the transform
    tr->shear = shear;

    // Mark the render matrix as dirty so it rebuilds with the new shearing next rendering pass
    tr->is_dirty = TRUE;
}
