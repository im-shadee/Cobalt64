/**
 * Copyright (c) 2026 Shadee
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "matrix_pool.h"
#include <t3d/t3d.h>
#include <libdragon.h>
#include <stdint.h>
#include <assert.h>

#define MTX_NONE 0xFFFFu

static T3DMat4FP* matrix_pool;
static uint16_t next_free[MAX_TRANSFORMS];

static uint16_t free_head;
static uint16_t next_unused = 0;

void matrix_pool_init(void) {
    matrix_pool = malloc_uncached(sizeof(T3DMat4FP) * MAX_TRANSFORMS);
    assert(matrix_pool);

    free_head = MTX_NONE;
    next_unused = 0;
}

uint16_t matrix_pool_alloc(void) {
    uint16_t taken;

    // Grab the next free slot
    if (free_head != MTX_NONE) {
        taken = free_head;
        free_head = next_free[taken];
        return taken;
    }

    if (next_unused < MAX_TRANSFORMS) {
        taken = next_unused;
        next_unused++;
        return taken;
    }

    // We can't run out of matrices ever (1:1 transform/matrix), so if this happens, crash
    assert(0);
    return MTX_NONE;
}

void matrix_pool_free(uint16_t index) {
    // A slot that was never handed out can't be returned; verify that index is valid
    assert(index < next_unused);

    next_free[index] = free_head;
    free_head = index;
}

T3DMat4FP* matrix_pool_get(uint16_t index) {
    assert(index < next_unused);
    return &matrix_pool[index];
}
