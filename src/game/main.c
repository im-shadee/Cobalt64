/**
 * Copyright (c) 2026 Shadee
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include <libdragon.h>
#include <t3d/t3d.h>
#include <t3d/t3dmodel.h>
#include "transform.h"
#include "bool.h"
#include "startup.h"
#include "debug.h"
#include "trig.h"
#include "inputs.h"
#include "config.h"

// Camera
#define FOV_DEG 85.0f
#define CAMERA_NEAR 2.0f
#define CAMERA_FAR 1000.0f
#define CAM_SPD 5

#define NUM_MODELS 5

int main(void) {
    // Initialize engine and debug (debug builds only) components 
    engine_init();
    DBG_OVERLAY_INIT();

    // Allocate Z-buffer in memory
    surface_t depth = surface_alloc(FMT_RGBA16, display_get_width(), display_get_height());

    // Camera setup
    T3DViewport viewport = t3d_viewport_create(); // Create a 3D viewport
    T3DVec3 cam_pos    = {{0, 0, 0}};
    T3DVec3 cam_target = {{0, 0, -1}}; // Define the 3D coordinate the camera is pointing toward
    T3DVec3 cam_up     = {{0, 1, 0}}; // Define the Y axis as the "up" vector

    // Load the cube model and ensure it is not corrupted
    // Call model_free once not in use anymore
    T3DModel* model = t3d_model_load("rom:/models/Chest.t3dm");
    assert(model);

    // Initialize an array of 10 transforms arranged in a row/depth layout
    Transform *cubes[NUM_MODELS];
    for (int i = 0; i < NUM_MODELS; i++) {
        vec3f_t pos = { 
            .x = (i - (NUM_MODELS / 2.0f)) * 200.0f, 
            .y = 0.0f, 
            .z = -200.0f
        };
        vec3f_t rot   = { .x = 0.0f, .y = i * 0.2f, .z = 0.0f };
        vec3f_t scale = { .x = 1.0f, .y = 1.0f, .z = 1.0f };
        vec3f_t shear = { .x = 0.0f, .y = 0.0f, .z = 0.0f };

        cubes[i] = transform_create(pos, rot, scale, shear, (transform_hierarchy_t){0,0,0});
        if (!cubes[i]) return -1;
    }

    while (1) {
        joypad_poll();
        DBG_UPDATE(JOYPAD_PORT_1);
        rspq_wait();

        // Update rotations dynamically every frame
        for (int i = 0; i < NUM_MODELS; i++) {
            
            // Rebuild the render matrices only when the transforms actually change
            if (cubes[i]->is_dirty) {
                update_render_mtx(
                    transform_get_matrix(cubes[i]->render_mtx_index), 
                    cubes[i]->position, 
                    cubes[i]->rotation,         
                    cubes[i]->scale, 
                    cubes[i]->shear
                );

                cubes[i]->is_dirty = FALSE; // The transform is flagged as non-dirty after the update
            }
        }

        // Camera movement
        // C buttons move it in the X/Y plane, holding B/A turns C-up/C-down into Z movement
        float cam_dx = 0.0f, cam_dy = 0.0f, cam_dz = 0.0f;
 
        if (inputs_held(IN_BTN_B, JOYPAD_PORT_1)) cam_dy -= CAM_SPD;
        else if (inputs_held(IN_BTN_A, JOYPAD_PORT_1)) cam_dy += CAM_SPD;

        if (inputs_held(IN_BTN_C_RIGHT, JOYPAD_PORT_1)) cam_dx += CAM_SPD;
        else if (inputs_held(IN_BTN_C_LEFT, JOYPAD_PORT_1)) cam_dx -= CAM_SPD;
        else if (inputs_held(IN_BTN_C_UP, JOYPAD_PORT_1)) cam_dz += CAM_SPD;
        else if (inputs_held(IN_BTN_C_DOWN, JOYPAD_PORT_1)) cam_dz -= CAM_SPD;
 
        cam_pos.v[0] += cam_dx;
        cam_pos.v[1] += cam_dy;
        cam_pos.v[2] += cam_dz;
 
        // The target moves with the camera, so it keeps looking along -Z
        cam_target.v[0] = cam_pos.v[0];
        cam_target.v[1] = cam_pos.v[1];
        cam_target.v[2] = cam_pos.v[2] - 1.0f;

        t3d_viewport_set_projection(&viewport, T3D_DEG_TO_RAD(FOV_DEG), CAMERA_NEAR, CAMERA_FAR);
        t3d_viewport_look_at(&viewport, &cam_pos, &cam_target, &cam_up);

        rdpq_attach(display_get(), &depth);
        t3d_frame_start(); // Start rendering a new frame
        t3d_viewport_attach(&viewport);

        t3d_screen_clear_color(RGBA32(0x20, 0x30, 0x60, 0xFF));
        t3d_screen_clear_depth();

        uint8_t ambient[4]     = {0x40, 0x40, 0x40, 0xFF};
        uint8_t light_color[4] = {0xFF, 0xFF, 0xFF, 0xFF};
        T3DVec3 light_dir      = {{1.0f, 1.0f, 1.0f}};
        t3d_vec3_norm(&light_dir);

        t3d_light_set_ambient(ambient);
        t3d_light_set_directional(0, light_color, &light_dir);
        t3d_light_set_count(1);

        rdpq_mode_push();
        rdpq_mode_dithering(DITHER_NONE_NONE);

        for (int i = 0; i < NUM_MODELS; i++) {
            t3d_matrix_push(transform_get_matrix(cubes[i]->render_mtx_index)); 
            t3d_model_draw(model);
            t3d_matrix_pop(1);
        }

        rdpq_mode_pop();
        DBG_DRAW();
        rdpq_sync_pipe();
        rdpq_detach_show();
        DBG_FRAME_END();
    }

    for (int i = 0; i < NUM_MODELS; i++) {
        free_uncached(transform_get_matrix(cubes[i]->render_mtx_index));
        free(cubes[i]);
    }

    t3d_model_free(model);
    t3d_destroy();
    DBG_OVERLAY_FREE();
    surface_free(&depth);

    return 0;
}
