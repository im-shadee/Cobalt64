/**
 * Copyright (c) 2026 Shadee
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef CAMERA_H
#define CAMERA_H

#include "color.h"
#include "layers.h"
#include "rect.h"
#include <stdbool.h>
#include <stdint.h>
#include <t3d/t3d.h>

typedef enum __attribute__((packed)) {
    PROJ_MODE_ORTHOGRAPHIC = 0,
    PROJ_MODE_PERSPECTIVE = 1
} cam_projection_mode_t;

typedef enum __attribute__((packed)) {
    CAM_BACKGROUND_SKYBOX = 0,
    CAM_BACKGROUND_SOLID = 1,
    CAM_BACKGROUND_OVERLAY = 2
} cam_clear_flags_t;

typedef struct {
    Rect viewport;
    float exposure, fov, near, far;
    Color8 background_color;
    uint16_t matrix_idx;
    
    cam_clear_flags_t clear_flags;
    cam_projection_mode_t projection;

    uint8_t culling_mask;
    bool enabled;
    uint8_t _padding;
} Camera; // 36 bytes

Camera* camera_get_main(void);
void camera_set_main(uint8_t idx);
void camera_update(const Camera* cam, T3DViewport* vp);

#endif // CAMERA_H
