/**
 * Copyright (c) 2026 Shadee
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef ACTOR_INSTANCE_H
#define ACTOR_INSTANCE_H

#include "actor_data.h"
#include <stdbool.h>
#include <stdint.h>

/** 
 * Ties a specific game object to its actor instance data.
 * Stores an index pointing to its ActorBehaviour, an enabled flag, and an actor data pointer.
 */
typedef struct
{
    uint32_t data_byte_offset; // Byte offset to instance-specific actor data (e.g. per-instance health, speed variables, etc.).
    uint16_t shared_actor_idx; // Points to the shared ActorBehavior.
    uint16_t obj_idx; // Object index associated with this instance.
    bool enabled; // Whether this object is enabled. If not, it does not run the shared actor behavior.
    uint8_t padding_[3];
} ActorInstance; // 12 bytes

ActorData get_actor_data(const ActorInstance* instance);

#endif // ACTOR_INSTANCE_H
