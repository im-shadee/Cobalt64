#ifndef SCENES_H
#define SCENES_H

#define NULL_IDX_16
#define NULL_IDX_8

#include <stdint.h>
#include "transform.h"

#define MAX_TRANSFORMS sizeof(uint16_t) - 1

typedef struct {
    transform_t transforms[MAX_TRANSFORMS];
    uint16_t active_count;
} scene_t;

void scene_update(scene_t* scene);

#endif // SCENES_H
