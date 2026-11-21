/**
 * Copyright (c) 2026 Shadee
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "transform.h"
#include "vectors.h"
#include <assert.h>
#include <stdbool.h>

void transform_set_rotation(Transform* tr, const vec3f_t angles) {
    assert(tr); // Crash if the transform is null

    // Assign the new angles to the container inside the transform
    tr->rotation = angles;

    // Mark the render matrix as dirty so it rebuilds with the new angles next rendering pass
    tr->is_dirty = true;
}

void transform_set_scale(Transform* tr, const vec3f_t scale) {
    assert(tr); // Crash if the transform is null

    // Assign the new scale to the container inside the transform
    tr->scale = scale;

    // Mark the render matrix as dirty so it rebuilds with the new scale next rendering pass
    tr->is_dirty = true;
}

void transform_set_shear(Transform* tr, const vec3f_t shear) {
    assert(tr); // Crash if the transform is null

    // Assign the new shear to the container inside the transform
    tr->shear = shear;

    // Mark the render matrix as dirty so it rebuilds with the new shearing next rendering pass
    tr->is_dirty = true;
}
