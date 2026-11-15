/**
 * Copyright (c) 2026 Shadee
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "scenes.h"
#include "transform.h"
#include "config.h"
#include "object.h"
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <stdio.h>
#include <stddef.h>

Object object_list[MAX_OBJECTS];

/**
 * Deserializes scene data from a scene file. Initializes scene pools and fill them with objects in the file.
 * @return A pointer to the created scene structure, or NULL in case of failure.
 */
Scene* deserialize_scene_data(FILE* file);

/** 
 * Retrieves the file name associated to the index and tries reading the file.
 * @return A pointer to the opened file, or NULL in case of errors
 * 
 * remarks: Make sure to close the file once the scene data has been serialized into a scene structure.
 */
FILE* get_scene_file_from_index(uint16_t index);

Scene* init_scene(uint16_t index);

// Scene functions
void scene_load_scene(uint16_t index) {
    // Initialize the scene
    //scene_t* scene = init_scene(index);

    // TODO: Unload the previous scene
    // TODO: Run the 'start' loop for all objects
    // TODO: Setup the update loop
}

Scene* init_scene(uint16_t index) {
    // TODO: Read and initialize
    FILE* scene_file = get_scene_file_from_index(index);
    assert(scene_file);
    
    // Creates a scene struct from scene file data
    // Initializes the scene pools and fill them with the objects placed in the file
    Scene* scene = deserialize_scene_data(scene_file);
    assert(scene);

    return scene;
}
/*
Object* scene_get_object(uint16_t index) {
    return NULL;
}*/

