/**
 * Copyright (c) 2026 Shadee
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "config.h"
#include "cobalt_math.h"
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

void draw_model(T3DMat4FP* mtx, T3DModel* model) {
    t3d_matrix_push(mtx); // Push the matrix for rendering
    t3d_model_draw(model); // Draw the shared model from the batch at the coordinates defined by the matrix
    t3d_matrix_pop(1); // Pop the last matrix after drawing
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
            if (tr->is_dirty) mtx_update(mtx, tr->position, tr->rotation, tr->scale, tr->shear);

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

void update_camera(const Camera* cam, T3DViewport* vp) {
    // Set viewport area from rect and exposure
    t3d_viewport_set_area(vp, cam->viewport.bounds.x, cam->viewport.bounds.y, 
        cam->viewport.bounds.w, cam->viewport.bounds.h);
    
    t3d_light_set_exposure(cam->exposure);

    // Set projection mode
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

void setup_lights(void) {
    // Setup ambient light
    t3d_light_set_ambient(scene_ambient.color);

    // Setup direction/points lights
    t3d_light_set_count(scene_header.light_count);

    for (int i = 0; i < scene_header.light_count; i++) {
        LightSource* light = &scene_lights[i];

        // TODO: Derive position/direction from transform
        switch (light->type) {
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

    setup_lights();

    // Update pass
    while (1) {
        joypad_poll(); // Poll for inputs first

        Camera* main = camera_get_main();
        if (main) update_camera(camera_get_main(), &viewport);
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
