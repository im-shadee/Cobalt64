#ifndef TRANSFORM_POOL_H
#define TRANSFORM_POOL_H

#include "transform.h"
#include <stdint.h>

typedef struct {
    transform_t* transforms; // Pointer to the bulk-allocated buffer of transforms
    uint16_t capacity; // Current active count
    uint16_t count; // Total allocated slots
} transform_pool_t; // 16 bytes

#endif // TRANSFORM_POOL_H
