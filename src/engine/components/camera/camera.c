/**
 * Copyright (c) 2026 Shadee
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "camera.h"
#include "object.h"
#include "scenes.h"
#include <stdint.h>

uint8_t camera_main_idx = 0;

Camera* camera_get_main(void) {
    if (scene_header.camera_count == 0) {
        return NULL;
    }

    // Check cached main camera first
    if (camera_main_idx < scene_header.camera_count) {
        Camera* cam = &scene_cameras[camera_main_idx];
        if (cam->enabled && obj_is_active(&scene_objs[cam->matrix_idx])) {
            return cam;
        }
    }

    // Find the first active and enabled camera and update the cache
    for (int i = 0; i < scene_header.camera_count; i++) {
        Camera* cam = &scene_cameras[i];
        if (cam->enabled && obj_is_active(&scene_objs[cam->matrix_idx])) {
            camera_main_idx = i;
            return cam;
        }
    }

    return NULL;
}
