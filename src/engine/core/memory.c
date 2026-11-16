/**
 * Copyright (c) 2026 Shadee
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "config.h"
#include <stdint.h>

typedef struct {
    uint8_t global_buffer[GLOBAL_DATA_MEM];
    uint8_t scene_buffer[SCENE_DATA_MEM];

    uint32_t global_offset;
    uint32_t scene_offset;
} SceneArena;
