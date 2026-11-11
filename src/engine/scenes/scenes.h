#ifndef SCENES_H
#define SCENES_H

#define NULL_IDX_16 0xFFFF
#define NULL_IDX_8 0xFF

#define POOL_TR_PADDING 50

#include <stdint.h>
#include "transform.h"
#include "transform_pool.h"
#include <stdio.h>

typedef struct {
    transform_pool_t transform_pool;
} scene_t;

void scene_update(scene_t* scene);

void scene_load_scene(uint16_t index);

#endif // SCENES_H
