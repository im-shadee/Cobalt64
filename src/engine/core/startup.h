/**
 * Copyright (c) 2026 Shadee
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef STARTUP_H
#define STARTUP_H

#include "config.h"
#include "debug.h"
#include "trig.h"
#include "inputs.h"
#include "components_init.h"
#include "scenes.h"
#include <assert.h>
#include <libdragon.h>
#include <t3d/t3d.h>
#include <t3d/t3dmodel.h>

static inline void libdragon_setup(void) {
    INIT_DEBUG(); // Initialize debug logging
    
    init_trig_table(); // Initialize trig.h sine table
    dfs_init(DFS_DEFAULT_LOCATION); // Initialize the filesystem (allows to read files bundled into the ROM)

    // Initialize display with default N64 resolution, framebuffer count, no gamma correction, 
    // and resampling when scaling
    // Using two framebuffers consume ~307 KB of memory
    display_init(RESOLUTION_320x240, DEPTH_16_BPP, FRAMEBUFFER_COUNT, GAMMA_NONE, FILTERS_RESAMPLE);

    rdpq_init();
    joypad_init();
    t3d_init((T3DInitParams){});
}

static inline void engine_init(void) {
    libdragon_setup(); // Engine startup
    DBG_OVERLAY_INIT(); // Debug components

    // Initialize components
    //components_init();

    // Initialize the file container and load the default scene
    // assert(scene_init_file_container(SCENE_DATA_PATH));
    // assert(scene_try_load(DEFAULT_SCENE_INDEX));

    // Start game loop
}

void engine_run(void);

#endif // STARTUP_H
