/**
 * Copyright (c) 2026 Shadee
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef STARTUP_H
#define STARTUP_H

#include "config.h"
#include "components_init.h"
#include "scenes.h"

static inline void engine_init() {
    // Engine startup (TODO)

    // Initialize components
    components_init();

    // Load default scene
    scene_load_scene(DEFAULT_SCENE_INDEX);

    // Awake pass (TODO)
    // Start frame loop with updates (TODO)
}

#endif
