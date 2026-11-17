/**
 * Copyright (c) 2026 Shadee
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * Scene file layout:
 * Scene count
 * Index entries ([ID | Byte offset] per line)
 * Scene_i data blob 

 * We load the scene using an index (the "ID" part of the index entries)
 * Then we grab the data blob for this scene and parse it into the memory arena.
 */
#include "config.h"
#include "baked_transform.h"
#include "memory.h"
#include "object.h"
#include "transform.h"
#include <stdint.h>
#include <bool.h>
#include <stdio.h>

/** Maps a scene index to its data location in the scenes file. */
typedef struct {
    uint16_t scene_id;
    uint32_t file_offset; // Exact byte offset where the scene data begins
} SceneIndexEntry; // 8 bytes

typedef struct {
    uint16_t count;
    SceneIndexEntry entries[];
} SceneFileContainerHeader; // 4 bytes

SceneIndexEntry* scene_index_table = NULL;
uint16_t scene_count = 0;

/** Holds count data for each component. Used to initialize core component pools. */
typedef struct {
    uint16_t baked_tr_count;
    uint16_t transform_count;
    uint16_t model_count;
    uint16_t object_count;
    uint16_t mesh_collider_count;
    uint16_t aabb_collider_count;
    uint16_t camera_count;
} SceneFileHeader; // 14 bytes

BakedTransform* baked_transforms;
Transform* transforms;
Object* objects;

bool_t try_parse_scene_data(FILE* container) {
    // Unload the previous scene data immediately
    scene_transition_reset();

    // Read the header from the current position
    SceneFileHeader header;
    if (fread(&header, sizeof(SceneFileHeader), 1, container) != 1) {
        return FALSE; // Failed to read scene header
    }

    // Reserve memory for each core component
    baked_transforms = (BakedTransform*)engine_mem_scene_alloc(header.baked_tr_count * sizeof(BakedTransform));
    objects = (Object*)engine_mem_scene_alloc(header.object_count * sizeof(Object));
    transforms = (Transform*)engine_mem_scene_alloc(header.transform_count * sizeof(Transform));

    if (!baked_transforms || !objects || !transforms) {
        return FALSE; // Out of scene memory
    }

    // Stream raw component data straight from disk/ROM into the arena buffers
    if (header.baked_tr_count > 0) {
        fread(baked_transforms, sizeof(BakedTransform), header.baked_tr_count, container);
    }
    if (header.transform_count > 0) {
        fread(transforms, sizeof(Transform), header.transform_count, container);
    }
    if (header.object_count > 0) {
        fread(objects, sizeof(Object), header.object_count, container);
    }

    return TRUE;
}

bool_t scene_init() {
    // Open the scenes file from ROM
    FILE* file = fopen(SCENE_DATA_PATH, "rb");
    if (!file) return FALSE;

    // Read the total number of scenes from the header
    if (fread(&scene_count, sizeof(uint16_t), 1, file) != 1) {
        fclose(file);
        return FALSE;
    }

    // Reserve memory in the global pool for the table
    uint32_t table_sz = scene_count * sizeof(SceneIndexEntry);
    scene_index_table = (SceneIndexEntry*)engine_mem_global_alloc(table_sz);

    if (!scene_index_table) {
        fclose(file);
        return FALSE; // Out of memory
    }

    // Stream the index into persistent RAM
    if (fread(scene_index_table, sizeof(SceneIndexEntry), scene_count, file) != scene_count) {
        fclose(file);
        return FALSE;
    }

    fclose(file);
    return TRUE;
}

bool_t scene_try_load(const uint16_t index) {
    if (index >= scene_count) return FALSE;

    FILE* file = fopen(SCENE_DATA_PATH, "rb");
    if (!file) return FALSE;

    // Direct lookup from persistent RAM table
    uint32_t target_offset = scene_index_table[index].file_offset;

    // Go straight to the scene data block in the file
    fseek(file, target_offset, SEEK_SET);
    return try_parse_scene_data(file);
}
