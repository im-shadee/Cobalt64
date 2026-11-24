/**
 * Copyright (c) 2026 Shadee
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "config.h"
#include "memory.h"
#include <stdint.h>
#include <stddef.h>

typedef struct {
    uint8_t global_buffer[GLOBAL_DATA_MEM];
    uint8_t scene_buffer[SCENE_DATA_MEM];

    uint32_t global_offset;
    uint32_t scene_offset;
} EngineMemory;

EngineMemory g_engine_memory;

void engine_mem_init(void) {
    g_engine_memory.global_offset = 0;
    g_engine_memory.scene_offset = 0;
}

void* engine_mem_global_alloc(uint32_t size) {
    size = (size + 15u) & ~15u; // 16-byte alignment matching cache line and DMA

    if (g_engine_memory.global_offset + size > GLOBAL_DATA_MEM) {
        return NULL; // Out of memory
    }

    // Grab a pointer to the area allocated to the component and return it
    void* ptr = &g_engine_memory.global_buffer[g_engine_memory.global_offset];
    g_engine_memory.global_offset += size; // Offset to the next free area
    return ptr;
}

void* engine_mem_scene_alloc(uint32_t size) {
    size = (size + 15u) & ~15u; // 16-byte alignment matching cache line and DMA

    if (g_engine_memory.scene_offset + size > SCENE_DATA_MEM) {
        return NULL; // Out of memory
    }

    // Grab a pointer to the area allocated to the component and return it
    void* ptr = &g_engine_memory.scene_buffer[g_engine_memory.scene_offset];
    g_engine_memory.scene_offset += size; // Offset to the next free area
    return ptr;
}

void engine_mem_reset_scene(void) {
    // Reset the offset so addresses get overwritten instead of wasting time resetting each address
    g_engine_memory.scene_offset = 0;
}
