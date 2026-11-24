/**
 * Copyright (c) 2026 Shadee
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * Scene file layout:
 * Scene count
 * Index entries ([ID | Byte offset] per line)
 * Scene_i data blob 
 *
 * We load the scene using an index (the "ID" part of the index entries)
 * Then we grab the data blob for this scene and parse it into the memory arena.
 */
#include "actors.h"
#include "camera.h"
#include "config.h"
#include "memory.h"
#include "models.h"
#include "object.h"
#include "scenes.h"
#include "transform.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

/** Maps a scene index to its data location in the scenes file. */
typedef struct {
    uint16_t scene_id;
    uint32_t file_offset; // Exact byte offset where the scene data begins
} SceneIndexEntry; // 8 bytes

SceneFileHeader scene_header;
AmbientLight scene_ambient = COLOR_WHITE;

SceneIndexEntry* scene_index_table = NULL;
uint16_t scene_count = 0;

Object* scene_objs = NULL;
Transform* scene_transforms = NULL;
T3DMat4FP* scene_matrices = NULL;

LightSource* scene_lights = NULL;
Camera* scene_cameras = NULL;

T3DModel** scene_shared_models = NULL;
ModelInstance* scene_model_data = NULL;

ActorInstance* scene_actor_instances = NULL;
TriggerActorInstance* scene_trigger_instances = NULL;
uint8_t* scene_actor_data_blob = NULL;

#pragma region Scene Loading

bool try_parse_scene_data(FILE* container) {
    // Read the scene_header from the current position
    if (fread(&scene_header, sizeof(SceneFileHeader), 1, container) != 1) {
        return false; // Failed to read scene scene_header
    }

    // Reserve memory for each core component (TODO: complete with types)
    scene_objs = (Object*)engine_mem_scene_alloc(scene_header.object_count * sizeof(Object));
    scene_transforms = (Transform*)engine_mem_scene_alloc(scene_header.transform_count * sizeof(Transform));

    if (!scene_objs || !scene_transforms) {
        return false; // Out of scene memory
    }

    // Stream raw component data straight from disk/ROM into the arena buffers
    if (scene_header.transform_count > 0) {
        fread(scene_transforms, sizeof(Transform), scene_header.transform_count, container);
    }
    if (scene_header.object_count > 0) {
        fread(scene_objs, sizeof(Object), scene_header.object_count, container);
    }

    return true;
}

void scene_free_models(void) {
    for (int i = 0; i < scene_header.shared_model_count; i++) {
        t3d_model_free(scene_shared_models[i]);
    }
}

bool scene_init(void) {
    // Open the scenes file from ROM
    FILE* file = fopen(SCENE_DATA_PATH, "rb");
    if (!file) return false;

    // Read the total number of scenes from the scene_header
    if (fread(&scene_count, sizeof(uint16_t), 1, file) != 1) {
        fclose(file);
        return false;
    }

    // Reserve memory in the global pool for the table
    uint32_t table_sz = scene_count * sizeof(SceneIndexEntry);
    scene_index_table = (SceneIndexEntry*)engine_mem_global_alloc(table_sz);

    if (!scene_index_table) {
        fclose(file);
        return false; // Out of memory
    }

    // Stream the index into persistent RAM
    if (fread(scene_index_table, sizeof(SceneIndexEntry), scene_count, file) != scene_count) {
        fclose(file);
        return false;
    }

    fclose(file);
    return true;
}

bool scene_try_load(const uint16_t index) {
    if (index >= scene_count) return false;
    scene_free_models(); // Free all models before resetting the memory arena
    engine_mem_reset_scene(); // Unload the previous scene actors
    camera_set_main(0); // Reset main camera to 0

    FILE* file = fopen(SCENE_DATA_PATH, "rb");
    if (!file) return false;

    // Direct lookup from persistent RAM table
    uint32_t target_offset = scene_index_table[index].file_offset;

    // Go straight to the scene data block in the file
    fseek(file, target_offset, SEEK_SET);
    return try_parse_scene_data(file);
}

#pragma endregion

#pragma region Component Search

// TODO: Make searches in both persistent/scene objects and make a persistent component manager. 
// Also split scene functions into multiple headers
ActorInstance* find_actor_instance_by_idx(uint16_t obj_idx) {
    for (uint16_t i = 0; i < scene_header.actor_instances_count; i++) {
        if (scene_actor_instances[i].obj_idx == obj_idx) {
            return &scene_actor_instances[i];
        }
    }
    return NULL;
}

#pragma endregion
