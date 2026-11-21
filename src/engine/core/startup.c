/**
 * Copyright (c) 2026 Shadee
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "config.h"
#include "debug.h"
#include "object.h"
#include "scenes.h"
#include "startup.h"
#include "trig.h"
#include "transform.h"
#include <libdragon.h>
#include <stdbool.h>
#include <t3d/t3d.h>
#include <t3d/t3dmodel.h>

// Camera (TODO: Move to camera component)
#define FOV_DEG 85.0f
#define CAMERA_NEAR 2.0f
#define CAMERA_FAR 1000.0f
#define CAM_SPD 5

void draw_model(T3DMat4FP* mtx, T3DModel* model) {
    t3d_matrix_push(mtx); // Push the matrix for rendering
    t3d_model_draw(model); // Draw the shared model from the batch at the coordinates defined by the matrix
    t3d_matrix_pop(1); // Pop the last matrix after drawing
}

void update_mtx(T3DMat4FP* mtx, vec3f_t pos, vec3f_t rot, vec3f_t scale, vec3f_t shr) {
    // Convert float degrees to 16-bit BAMs
    uint16_t x_bam = (uint16_t)(int32_t)(rot.x * DEG_TO_BAM);
    uint16_t y_bam = (uint16_t)(int32_t)(rot.y * DEG_TO_BAM);
    uint16_t z_bam = (uint16_t)(int32_t)(rot.z * DEG_TO_BAM);

    // Fetch fixed-point values from LUT and scale them
    float cx = fixed_cos(x_bam) * FIXED_TO_FLOAT;
    float sx = fixed_sin(x_bam) * FIXED_TO_FLOAT;
    
    float cy = fixed_cos(y_bam) * FIXED_TO_FLOAT;
    float sy = fixed_sin(y_bam) * FIXED_TO_FLOAT;
    
    float cz = fixed_cos(z_bam) * FIXED_TO_FLOAT;
    float sz = fixed_sin(z_bam) * FIXED_TO_FLOAT;

    // Compute rotation matrix values (compute matrix indexes instead of creating matrices)
    float r00 = cy * cz;
    float r01 = -cy * sz;
    float r02 = sy;
    
    float r10 = sx * sy * cz + cx * sz;
    float r11 = -sx * sy * sz + cx * cz;
    float r12 = -sx * cy;
    
    float r20 = -cx * sy * cz + sx * sz;
    float r21 = cx * sy * sz + sx * cz;
    float r22 = cx * cy;

    // Compute scale and shearing combined
    float s00 = scale.x;
    float s01 = shr.x * scale.y;
    float s02 = shr.y * scale.z;

    float s11 = scale.y;
    float s12 = shr.z * scale.z;

    float s22 = scale.z;

    // T3D uses row-vector convention: translation in m[3][0..2]
    // Matrix below is column-vector, so it is written transposed
    T3DMat4 m;
    float a[4][4] = {
        {s00 * r00 + s01 * r10 + s02 * r20, s00 * r01 + s01 * r11 + s02 * r21, s00 * r02 + s01 * r12 + s02 * r22, pos.x},
        {s11 * r10 + s12 * r20, s11 * r11 + s12 * r21, s11 * r12 + s12 * r22, pos.y},
        {s22 * r20, s22 * r21, s22 * r22, pos.z},
        {0, 0, 0, 1}
    };

    #pragma GCC unroll 4
    for (int r = 0; r < 4; r++)
        for (int c = 0; c < 4; c++)
            m.m[c][r] = a[r][c];

    // Convert standard float matrix to Tiny3D's hardware fixed-point format
    t3d_mat4_to_fixed(mtx, &m);
}

void update_static_batches(void) {
    for (int sb = 0; sb < scene_header.sbatch_count; sb++) {
        StaticBatch* sbatch = &scene_sbatches[sb];
        T3DModel* model = scene_models[sbatch->model_index]; // Shared model for the batch

        for (int i = 0; i < sbatch->object_count; i++) {
            uint16_t obj_idx = sbatch->start_object_index + i;
            Object* o = &scene_objs[obj_idx];
            if (!obj_should_render(o)) continue;

            T3DMat4FP* mtx = &scene_matrices[obj_idx];
            draw_model(mtx, model);
        }
    }
}

void update_dynamic_batches(void) {
    for (int db = 0; db < scene_header.dbatch_count; db++) {
        DynamicBatch* dbatch = &scene_dbatches[db];
        T3DModel* model = scene_models[dbatch->model_index]; // Shared model for the batch

        for (int i = 0; i < dbatch->object_count; i++) {
            uint16_t obj_idx = dbatch->start_object_index + i;
            Object* o = &scene_objs[obj_idx];
            if (!obj_should_render(o)) continue;

            T3DMat4FP* mtx = &scene_matrices[obj_idx];

            // Recompute the render matrix if the object's transform is dirty
            Transform* tr = &scene_transforms[obj_idx];
            if (tr->is_dirty) update_mtx(mtx, tr->position, tr->rotation, tr->scale, tr->shear);

            draw_model(mtx, model);
        }
    }
}

void graphics_start_drawing(void) {
    rdpq_mode_push();

    // Update the static batches first, then dynamic
    update_static_batches();
    update_dynamic_batches();

    rdpq_mode_pop();
}

void setup_lights(void) {
    // Setup ambient light
    t3d_light_set_ambient(scene_ambient.color);

    // Setup direction/points lights
    t3d_light_set_count(scene_header.light_count);

    for (int i = 0; i < scene_header.light_count; i++) {
        LightSource* light = &scene_lights[i];

        // TODO: Derive position/direction from transform
        switch (light->type)
        {
            case LIGHT_TYPE_DIRECTIONAL:
                t3d_light_set_directional(i, light->light_color.color, &(T3DVec3){{0.0f, 1.0f, 0.0f}});
                break;
            
            case LIGHT_TYPE_POINT:
                t3d_light_set_point(i, light->light_color.color, &(T3DVec3){{0.0f, 0.0f, 0.0f}}, light->size, light->ignoreNormals);
                break;

            default:
                break;
        }
    }
}

void libdragon_setup(void) {
    INIT_DEBUG(); // Initialize debug logging
    
    init_trig_table(); // Initialize trig.h sine table
    dfs_init(DFS_DEFAULT_LOCATION); // Initialize the filesystem (allows to read files bundled into the ROM)
    display_init(RESOLUTION_320x240, DEPTH_16_BPP, FRAMEBUFFER_COUNT, GAMMA_NONE, FILTERS_RESAMPLE);

    rdpq_init();
    rdpq_mode_dithering(DITHER_MODE);
    joypad_init();
    t3d_init((T3DInitParams){});
}

void engine_init(void) {
    libdragon_setup(); // Libdragon init
    DBG_OVERLAY_INIT(); // Debug components
    engine_mem_init(); // Initialize engine memory arena
    scene_init(); // Initialize scene header
}

void engine_run(void) {
    // On Add pass
    
    // Start pass
    surface_t depth = surface_alloc(FMT_RGBA16, display_get_width(), display_get_height()); // Allocate Z-buffer in memory
    T3DViewport viewport = t3d_viewport_create();

    // TODO: Replace with main camera
    T3DVec3 cam_pos    = {{0, 0, 0}};
    T3DVec3 cam_target = {{0, 0, -1}}; // Define the 3D coordinate the camera is pointing toward
    T3DVec3 cam_up     = {{0, 1, 0}}; // Define the Y axis as the "up" vector

    setup_lights();

    // Update pass
    while (1) {
        joypad_poll(); // Poll for inputs first

        t3d_viewport_set_projection(&viewport, T3D_DEG_TO_RAD(FOV_DEG), CAMERA_NEAR, CAMERA_FAR);
        t3d_viewport_look_at(&viewport, &cam_pos, &cam_target, &cam_up);
        rdpq_attach(display_get(), &depth);

        t3d_frame_start(); // Start rendering a new frame
        t3d_viewport_attach(&viewport);

        t3d_screen_clear_color(RGBA32(0x20, 0x30, 0x60, 0xFF));
        t3d_screen_clear_depth();

        graphics_start_drawing(); // Draws objects in the scene
        DBG_DRAW(); // Draw debug overlay in debug builds if enabled
        rdpq_detach_show();
        DBG_FRAME_END(); // Mark the frame as ended for the profiler
    }

    surface_free(&depth);
}
