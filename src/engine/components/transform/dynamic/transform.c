/**
 * Copyright (c) 2026 Shadee
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "transform.h"
#include "matrix_pool.h"
#include "component.h"
#include <stdint.h>

static void transform_update(void* comps, const uint16_t* owners, uint16_t count, float dt) {
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

static void transform_on_add(uint16_t obj_idx, void* comp) {
    Transform* tr = (Transform*)comp;
    tr->render_mtx_index = matrix_pool_alloc();
    tr->scale = (vec3f_t){0, 0, 0};
    tr->is_dirty = TRUE;
}

static void transform_on_remove(uint16_t obj_idx, void* comp) {
    Transform* tr = (Transform*)comp;
    matrix_pool_free(tr->render_mtx_index);
    tr->render_mtx_index = MTX_NONE;
}

COMP_DEFINE(
    transform, 
    Transform, 
    MAX_TRANSFORMS, 
    .update = transform_update,
    .on_add = transform_on_add, 
    .on_remove = transform_on_remove
)
