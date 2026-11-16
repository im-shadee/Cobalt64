/**
 * Copyright (c) 2026 Shadee
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef BAKED_TRANSFORM_H
#define BAKED_TRANSFORM_H

#include <stdint.h>

/** Non-dynamic transform. Never moves. Contains an index to a render matrix. */
typedef struct {
    uint16_t render_mtx_index;
} BakedTransform; // 2 bytes

#endif // BAKED_TRANSFORM_H
