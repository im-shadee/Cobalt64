/**
 * Copyright (c) 2026 Shadee
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef SCENES_H
#define SCENES_H

#include "actors.h"
#include "camera.h"
#include "model_instance.h"
#include "lights.h"
#include "object.h"
#include "transform.h"
#include <t3d/t3d.h>
#include <t3d/t3dmodel.h>
#include <stdbool.h>

/** Holds count data for each component. Used to initialize core component pools. */
typedef struct {
    uint16_t object_count; // Also used for the matrix count (since all objects have a type of transform)
    uint16_t transform_count;

    uint16_t model_count;
    uint16_t unique_model_count;
    
    uint16_t mesh_collider_count;
    uint16_t aabb_collider_count;
    
    uint16_t actor_behaviour_count;
    uint16_t actor_instances_count;
    uint16_t trigger_behaviour_count;
    uint16_t trigger_instances_count;
    
    uint16_t sbatch_count;
    uint16_t dbatch_count;

    uint8_t camera_count;
    uint8_t light_count;
} SceneFileHeader;

/** Holds batching data for models (static). Objects are sorted by batch, making game loop updates more performant. */
typedef struct {
    uint16_t model_index; // Index of the model used by this batch
    uint16_t start_object_index; // Where this batch starts in the object pool
    uint16_t object_count; // How many objects share this model contiguously
} StaticBatch; // 6 bytes

/** Holds batching data for models. Objects are sorted by batch, making game loop updates more performant. */
typedef struct {
    uint16_t model_index; // Index of the model used by this batch
    uint16_t start_object_index; // Where this batch starts in the object pool
    uint16_t object_count; // How many objects share this model contiguously
} DynamicBatch; // 6 bytes

extern SceneFileHeader scene_header;
extern AmbientLight scene_ambient;

// Core components
extern Object* scene_objs;
extern Transform* scene_transforms;
extern T3DMat4FP* scene_matrices;

extern LightSource* scene_lights;
extern Camera* scene_cameras;

extern T3DModel** scene_models;
extern ModelInstanceData* scene_models_data;

extern StaticBatch* scene_sbatches;
extern DynamicBatch* scene_dbatches;

extern ActorBehavior* scene_shared_actors;
extern ActorInstance* scene_actor_instances;

extern TriggerActorBehavior* scene_trigger_behaviors;
extern TriggerActorInstance* scene_trigger_instances;

bool scene_init(void);
bool scene_try_load(const uint16_t index);

#pragma region Component Search

/**
 * Retrieves an ActorInstance component using its object ID.
 * 
 * @param obj_idx The unique object ID to search for.
 * @return A pointer to the ActorInstance, or NULL if not found.
 * 
 * remarks: This performs an O(n) linear scan. Use with caution in high-frequency loops.
 */
ActorInstance* find_actor_instance_by_obj_idx(uint16_t obj_idx);

#pragma endregion

#endif // SCENES_H
