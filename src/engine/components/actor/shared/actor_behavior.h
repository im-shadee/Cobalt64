/**
 * Copyright (c) 2026 Shadee
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef ACTOR_BEHAVIOR_H
#define ACTOR_BEHAVIOR_H

#include "actor_data.h"
#include <stdint.h>

/** Shared actor behavior blueprint. Contains function pointers for lifecycle events. */
typedef struct {
    /** Runs once at scene load or when dynamically spawned in the scene before everything else. */
    void (*on_add)(uint16_t obj_idx, ActorData actor_data);

    /** Runs custom behavior right before the object gets destroyed. */
    void (*on_destroy)(uint16_t obj_idx, ActorData actor_data);

    /** Runs once before update. */
    void (*start)(uint16_t obj_idx, ActorData actor_data);

    /** Runs once every frame. */
    void (*update)(uint16_t obj_idx, ActorData actor_data, float dt);
} ActorBehavior; // 16 bytes

extern const ActorBehavior g_actor_behavior_reg[];
extern const uint16_t G_ACTOR_BEHAVIOR_COUNT;

#endif // ACTOR_BEHAVIOR_H
