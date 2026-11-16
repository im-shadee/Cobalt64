/**
 * Copyright (c) 2026 Shadee
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "baked_transform.h"
#include "matrix_pool.h"
#include <stdlib.h>

BakedTransform* baked_tr_create(vec3f_t pos, vec3f_t rot, vec3f_t scale, vec3f_t shr, HierarchyTree hierarchy) {
    BakedTransform* tr = malloc(sizeof(BakedTransform));
    if (!tr) return NULL;

    tr->render_mtx_index = MTX_NONE;
    tr->hierarchy = hierarchy;
    update_render_mtx(transform_get_matrix(tr->render_mtx_index), pos, rot, scale, shr); // Calculate the render matrix once when initialized
    return tr;
}
