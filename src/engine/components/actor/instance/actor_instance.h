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
 * 
 * remarks: Use 'actor_data' to define per-instance variables such as health, speed, etc.
 */
typedef struct
{
    ActorData actor_data; // Custom data specific to each instance (e.g. per-instance health, speed variables, etc.).
    uint16_t shared_actor_idx; // Points to the shared ActorBehavior.
    uint16_t obj_idx; // Object index associated with this instance.
    bool enabled; // Whether this object is enabled. If not, it does not run the shared actor behavior.
} ActorInstance; // 12 bytes

#endif // ACTOR_INSTANCE_H
