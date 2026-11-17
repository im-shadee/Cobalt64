/**
 * Copyright (c) 2026 Shadee
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include <stdint.h>

/** Initialize engine memory once at boot. */
void engine_mem_init(void);

/**
 * Allocates a memory slot in the global pool for a persistent actor.
 * @return A pointer to the area allocated for the actor, or NULL if the buffer is out of memory.
 */
void* engine_mem_global_alloc(uint32_t size);

/**
 * Allocates a memory slot in the global pool for a scene-specific actor.
 * @return A pointer to the area allocated for the actor, or NULL if the buffer is out of memory.
 */
void* engine_mem_scene_alloc(uint32_t size);

/** Resets the scene pool. Does not reset the persistent actors pool. */
void engine_mem_reset_scene(void);
