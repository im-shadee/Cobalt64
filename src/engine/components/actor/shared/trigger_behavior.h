/**
 * Copyright (c) 2026 Shadee
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef TRIGGER_BEHAVIOUR_H
#define TRIGGER_BEHAVIOUR_H

#include "actor_data.h"
#include <stdint.h>

typedef void (*TriggerCallback)(uint16_t self_idx, uint16_t other_idx, ActorData actor_data);
// 4 bytes

/** Shared trigger behavior blueprint. Contains function pointers for lifecycle events. */
typedef struct {
    /** Fired once when a new object enters this trigger. */
    TriggerCallback on_trigger_enter;

    /** Fired every frame if an object stays in this trigger. */
    TriggerCallback on_trigger_stay;

    /** Fired once when an object inside the trigger exits it. */
    TriggerCallback on_trigger_exit;
} TriggerActorBehavior; // 12 bytes

#endif // TRIGGER_BEHAVIOUR_H
