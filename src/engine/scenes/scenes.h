/**
 * Copyright (c) 2026 Shadee
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef SCENES_H
#define SCENES_H

#include "object.h"
#include "transform.h"
#include "model_instance.h"
#include <t3d/t3d.h>
#include <t3d/t3dmodel.h>

/** Holds count data for each component. Used to initialize core component pools. */
typedef struct {
    uint16_t transform_count;
    uint16_t model_count;
    uint16_t object_count; // Also used for the matrix count (since all objects have a type of transform)
    uint16_t mesh_collider_count;
    uint16_t aabb_collider_count;
    uint16_t camera_count;
    uint16_t sbatch_count;
    uint16_t dbatch_count;
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

// Core components
extern Object* scene_objs;
extern Transform* scene_transforms;
extern T3DMat4FP* scene_matrices;

extern T3DModel* scene_models;
extern ModelInstanceData* scene_models_data;

extern StaticBatch* scene_sbatches;
extern DynamicBatch* scene_dbatches;

bool_t scene_init(void);
bool_t scene_try_load(const uint16_t index);

#endif // SCENES_H
