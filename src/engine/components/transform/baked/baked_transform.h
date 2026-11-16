/**
 * Copyright (c) 2026 Shadee
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef BAKED_TRANSFORM_H
#define BAKED_TRANSFORM_H

#include "vectors.h"
#include "hierarchy_tree.h"

/** Data structure holding the rendering matrix of a static object, calculated at initialization. */
typedef struct {
    HierarchyTree hierarchy;
    uint16_t render_mtx_index;
} BakedTransform; // 8 bytes

/** 
 * Initializes a baked transform with a specified position, rotation, scale and shearing.
 * returns: A pointer to the created transform or NULL in case of errors.
 * remarks: A baked transform never changes once created. Use for static objects/meshes to save on memory.
 */
BakedTransform* baked_tr_create(vec3f_t pos, vec3f_t rot, vec3f_t scale, vec3f_t shr, HierarchyTree hierarchy);

#pragma region Cobalt Lifecycle

void transform_on_add(uint16_t obj_idx, void* comp);

#pragma endregion

#endif // BAKED_TRANSFORM_H
