/**
 * Copyright (c) 2026 Shadee
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef MODEL_INSTANCE_H
#define MODEL_INSTANCE_H

#include "color.h"
#include <stdint.h>

/** 
 * Describes a model instance pointing to a shared mesh with local properties such as
 * base/shade color if eligible to the combiner and state.
 */
typedef struct  {
    Color8 base_color;
    Color8 shade_color;
    uint16_t model_index;
    uint16_t obj_index;
    uint8_t enabled;
    uint8_t padding_[3]; 
} ModelInstance; // 16 bytes

#endif // MODEL_INSTANCE_H
