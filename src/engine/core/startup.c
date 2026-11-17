/**
 * Copyright (c) 2026 Shadee
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "startup.h"
#include "config.h"
#include "scenes.h"
#include "object.h"
#include "transform.h"
#include "baked_transform.h"

void engine_run(void) {
    // On Add pass
    
    // Start pass

    // Update pass
    while (1) {
        joypad_poll(); // Poll for inputs first
        rspq_wait();

        
    }
}

void graphics_start_drawing() {
    for (int i = 0; i < MAX_Ob)
}
