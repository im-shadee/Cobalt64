/**
 * Copyright (c) 2026 Shadee
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "camera.h"
#include "color.h"
#include "cobalt_math.h"
#include "object.h"
#include "scenes.h"
#include <assert.h>
#include <stdint.h>
#include <t3d/t3d.h>

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

void camera_set_main(uint8_t idx) {
    if (idx < scene_header.camera_count) camera_main_idx = idx;
}

void camera_set_mode(const Camera* cam, T3DViewport* vp) {
    assert(cam); assert(vp);
    float aspect = (float)cam->viewport.bounds.w / (float)cam->viewport.bounds.h;
    switch (cam->projection) {
        case PROJ_MODE_ORTHOGRAPHIC:
            
            float half_h = cam->fov * 0.5f;
            float half_w = half_h * aspect;
            t3d_viewport_set_ortho(vp, -half_w, half_w, -half_h, half_h, cam->near, cam->far);
            break;

        case PROJ_MODE_PERSPECTIVE:
            t3d_viewport_set_perspective(vp, T3D_DEG_TO_RAD(cam->fov), aspect, cam->near, cam->far);
        
        default:
            break;
    }
}

void camera_handle_flags(const Camera* cam) {
    assert(cam);

    switch (cam->clear_flags) {
        case CAM_BACKGROUND_SOLID:
            t3d_screen_clear_color(cam->background_color.lc);
            t3d_screen_clear_depth();
            break;

        case CAM_BACKGROUND_SKYBOX:
            t3d_screen_clear_depth();
            // skybox_draw(&viewport);
            break;

        case CAM_BACKGROUND_OVERLAY:
            t3d_screen_clear_depth(); // Only resets Z-buffer so overlay 3D meshes render on top
            break;

        default:
            break;
    }
}

void camera_update(const Camera* cam, T3DViewport* vp) {
    assert(cam); assert(vp);

    // Set viewport area from rect and exposure
    t3d_light_set_exposure(cam->exposure);
    t3d_viewport_set_area(vp, cam->viewport.bounds.x, cam->viewport.bounds.y, 
        cam->viewport.bounds.w, cam->viewport.bounds.h);

    camera_set_mode(cam, vp); // Set projection mode

    // Extract world position & forward direction from the camera's FP matrix
    const T3DMat4FP* cam_mtx = &scene_matrices[cam->matrix_idx];

    T3DVec3 cam_pos, cam_forward;
    mtx_get_position(cam_mtx, &cam_pos);
    mtx_get_forward(cam_mtx, &cam_forward);

    T3DVec3 cam_target = {{
        cam_pos.x + cam_forward.x,
        cam_pos.y + cam_forward.y,
        cam_pos.z + cam_forward.z
    }};
    T3DVec3 cam_up = {{ 0.0f, 1.0f, 0.0f }}; // Set up as +Y

    t3d_viewport_look_at(vp, &cam_pos, &cam_target, &cam_up);
}
