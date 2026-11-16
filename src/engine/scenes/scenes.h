/**
 * Copyright (c) 2026 Shadee
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef SCENES_H
#define SCENES_H

#define NULL_IDX_16 0xFFFF
#define NULL_IDX_8 0xFF

#include "transform.h"
#include "config.h"
#include "object.h"
#include <stdio.h>
#include <stdint.h>

typedef struct {
    
} Scene;

void scene_update(Scene* scene);

void scene_load_scene(uint16_t index);

//Object* scene_get_object(uint16_t index);

#endif // SCENES_H
