/**
 * Copyright (c) 2026 Shadee
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef MATRIX_POOL_H
#define MATRIX_POOL_H

#include "config.h"
#include "stdint.h"
#include <t3d/t3d.h>

#define MAX_TRANSFORMS MAX_OBJECTS // 1 Transform max per object; and all objects have a distinct transform

/** Allocate the matrix pool block. */
void matrix_pool_init(void);

/** 
 * Retrieve the next free slot in the matrix pool.
 * @return The next free slot in the pool to assign to a transform, or 0xFFFFu if it fails.
 */
uint16_t matrix_pool_alloc(void);

/** Free a slot in the matrix pool. */
void matrix_pool_free(uint16_t index);

/** 
 * Retrieves the matrix attached to a transform index.
 * @return A pointer to the tied matrix, or NULL in case of errors.
 */
T3DMat4FP* matrix_pool_get(uint16_t index);

#endif // MATRIX_POOL_H
