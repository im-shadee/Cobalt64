#ifndef MESH_INSTANCE_H
#define MESH_INSTANCE_H

#include "color.h"
#include <stdint.h>

typedef struct {
    uint16_t model_index;
    Color8 base_color;
    Color8 shade_color;
} ModelInstanceData; // 12 bytes

#endif // MESH_INSTANCE_H
