// Compiled into .rodata (ROM)

/**
 * Copyright (c) 2026 Shadee
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef COBALT64_CONFIG_DATA_H
#define COBALT64_CONFIG_DATA_H

#include <stdint.h>

typedef struct {
    uint32_t dither_mode;

    uint32_t target_mem_bytes; // The target memory the game is expected to run on (4MB or 8MB (4MB + Expansion Pak)).
    uint32_t scene_data_mem; // Amount of memory bytes allocated for scene-specific data at boot.
    uint32_t global_data_mem; // Amount of memory bytes allocated for persistent data at boot.

    uint16_t res_width;
    uint16_t res_height;

    uint16_t default_scene_index; // The index of the scene the game starts on.
    uint8_t fb_count; // The amount of framebuffers to use (1-3).
    uint8_t target_fps; // The target framerate to cap the game at.

    const char* game_name;
    const char* scene_data_path;
} CobaltEngineConfig; // 32 bytes

extern const CobaltEngineConfig g_cobalt_conf;

#endif // COBALT64_CONFIG_DATA_H
