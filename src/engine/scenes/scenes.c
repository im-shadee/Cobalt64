#include "scenes.h"
#include "transform.h"
#include "transform_pool.h"
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <stdio.h>

/* 
    On scene files:
    Scene files will always have this structure:
    Line 1-4: pool capacities (Object, Transform, Mesh, Collider)
    Line 5+: buffer entries (Object, Transform, Mesh, Collider)
*/

// Internal functions definition
/**
 * Deserializes scene data from a scene file. Initializes scene pools and fill them with objects in the file.
 * @return A pointer to the created scene structure, or NULL in case of failure.
 */
scene_t* deserialize_scene_data(FILE* file);

/** 
 * Retrieves the file name associated to the index and tries reading the file.
 * @return A pointer to the opened file, or NULL in case of errors
 * 
 * remarks: Make sure to close the file once the scene data has been serialized into a scene structure.
 */
FILE* get_scene_file_from_index(uint16_t index);

scene_t* init_scene(uint16_t index);
void scene_init_transform_pool(transform_pool_t* pool);

// Scene functions
void scene_load_scene(uint16_t index) {
    // Initialize the scene
    scene_t* scene = init_scene(index);

    // TODO: Unload the previous scene
    // TODO: Run the 'start' loop for all objects
    // TODO: Setup the update loop
}

// Internal functions
scene_t* init_scene(uint16_t index) {
    // TODO: Read and initialize
    FILE* scene_file = get_scene_file_from_index(index);
    assert(scene_file);
    
    // Creates a scene struct from scene file data
    // Initializes the scene pools and fill them with the objects placed in the file
    scene_t* scene = deserialize_scene_data(scene_file);
    assert(scene);

    return scene;
}

void scene_init_transform_pool(transform_pool_t* pool) {
    // Allocate transform buffer
    transform_t* buffer = malloc(sizeof(transform_t) * pool->capacity);
    if (!buffer) return;

    pool->transforms = buffer;
}
