/**
 * Copyright (c) 2026 Shadee
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef TRIGGER_INSTANCE_H
#define TRIGGER_INSTANCE_H

#define MAX_SIMULTANEOUS_OVERLAPS 4

#include "actor_data.h"
#include <stdint.h>

/** Per-instance runtime trigger state and overlap tracker. */
typedef struct {
    uint16_t overlapping_ids_[MAX_SIMULTANEOUS_OVERLAPS]; // Fixed array tracking current overlaps.
    uint16_t shared_trigger_idx; // Points to the shared TriggerActorBehavior.
    uint16_t obj_idx; // Object index associated with this instance.
} TriggerActorInstance; // 12 bytes

#endif // TRIGGER_INSTANCE_H