/**
 * Copyright (c) 2026 Shadee
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "actor_data.h"
#include "actor_instance.h"
#include "scenes.h"

ActorData get_actor_data(const ActorInstance* instance) {
    return (ActorData)(scene_actor_data_blob + instance->data_byte_offset);
}
