/**
 * Copyright (c) 2026 Shadee
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef COLOR_H
#define COLOR_H

#include <stdint.h>
#include <libdragon.h>

typedef union {
    uint32_t raw; // For fast 32-bit copies, clears, comps
    uint8_t color[4]; // Raw byte streams into DMA
    color_t lc; // Libdragon color format
    struct { // Grants access to each channel individually
        uint8_t r;
        uint8_t g;
        uint8_t b;
        uint8_t a;
    } channels; 
} Color8; // 4 bytes

// Primaries
#define COLOR_RED ((Color8){.color = {255, 0, 0, 255}})
#define COLOR_BLUE ((Color8){.color = {0, 0, 255, 255}})
#define COLOR_GREEN ((Color8){.color = {0, 255, 0, 255}})

// Secondaries
#define COLOR_YELLOW ((Color8){.color = {255, 255, 0, 255}})
#define COLOR_MAGENTA ((Color8){.color = {255, 0, 255, 255}})
#define COLOR_CYAN ((Color8){.color = {0, 255, 255, 255}})

// Tints
#define COLOR_ORANGE ((Color8){.color = {255, 128, 0, 255}})
#define COLOR_PURPLE ((Color8){.color = {128, 0, 255, 255}})
#define COLOR_BROWN ((Color8){.color = {139, 69, 19, 255}})
#define COLOR_PINK ((Color8){.color = {255, 192, 203, 255}})

// Neutrals and alpha
#define COLOR_WHITE ((Color8){.color = {255, 255, 255, 255}})
#define COLOR_LIGHT_GRAY ((Color8){.color = {192, 192, 192, 255}})
#define COLOR_GRAY ((Color8){.color = {128, 128, 128, 255}})
#define COLOR_DARK_GRAY ((Color8){.color = {64, 64, 64, 255}})
#define COLOR_BLACK ((Color8){.color = {0, 0, 0, 255}})
#define COLOR_TRANSPARENT ((Color8){.color = {0, 0, 0, 0}})

#endif // COLOR_H
