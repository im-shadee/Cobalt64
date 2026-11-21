/**
 * Copyright (c) 2026 Shadee
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef RECT_H
#define RECT_H

#include <stdint.h>

typedef union {
    uint64_t raw; // For quick comparisons
    uint16_t coords[4];
    struct { // Grants access to each coordinate individually
        uint16_t x;
        uint16_t y;
        uint16_t w;
        uint16_t h;
    } bounds;
} Rect; // 8 bytes

#endif // RECT_H
