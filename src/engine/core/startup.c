/**
 * Copyright (c) 2026 Shadee
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "actors.h"
#include "config.h"
#include "cobalt_math.h"
#include "debug.h"
#include "layers.h"
#include "models.h"
#include "object.h"
#include "scenes.h"
#include "startup.h"
#include "trig.h"
#include "transform.h"
#include <assert.h>
#include <libdragon.h>
#include <stdbool.h>
#include <t3d/t3d.h>

uint64_t last_ticks = 0;

void draw_model(T3DMat4FP* mtx, T3DModel* model, Color8 base_color, Color8 shade_color) {
    t3d_matrix_push(mtx); // Push the matrix for rendering

    // Set the RDP primitive/env color register right before drawing, if the combiner setting allows it
    rdpq_set_prim_color(base_color.lc);
    rdpq_set_env_color(shade_color.lc);

    t3d_model_draw(model); // Draw the shared model at the coordinates defined by the matrix
    t3d_matrix_pop(1); // Pop the last matrix after drawing
}

void scene_draw_models(uint8_t culling_mask) {
    for (int i = 0; i < scene_header.model_instances_count; i++) {
        ModelInstance* md_instance = &scene_model_data[i];
        if (!md_instance->enabled) continue; // Skip if the component is disabled

        Object* o = &scene_objs[md_instance->obj_index];
        if (!obj_should_render(o)) continue; // Skip if the object is disabled, free or destroyed
        if ((culling_mask & (1 << o->layer)) == 0) continue; // Skip if this object's render_layer is culled by camera

        // Get the shared model and the object's matrix to pass to hand for drawing
        T3DModel* md_shared = scene_shared_models[md_instance->model_index];
        T3DMat4FP* md_render_mtx = &scene_matrices[md_instance->obj_index];

        if (!obj_is_static(o)) {
            Transform* tr = &scene_transforms[md_instance->obj_index];

            // Recompute the render matrix if the object's transform is dirty
            if (tr->is_dirty){
                mtx_update(md_render_mtx, tr->position, tr->rotation, tr->scale, tr->shear);
                data_cache_hit_writeback(md_render_mtx, sizeof(T3DMat4FP));
                tr->is_dirty = false;
            }
        }

        draw_model(md_render_mtx, md_shared, 
            md_instance->base_color, md_instance->shade_color);   
    }
}

void graphics_start_drawing(uint8_t culling_mask) {
    rdpq_mode_push();
    scene_draw_models(culling_mask);
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

void actors_on_add() {
    for (int i = 0; i < scene_header.actor_instances_count; i++) {
        ActorInstance* instance = &scene_actor_instances[i];

        // Skip inactive/free slots
        if (!instance->enabled || !obj_should_render(&scene_objs[instance->obj_idx])) continue;
        assert(instance->behavior_id < G_ACTOR_BEHAVIOR_COUNT);

        // Look up the shared behavior blueprint using the index
        const ActorBehavior* behavior = &g_actor_behavior_reg[instance->behavior_id];

        // Call the add function pointer if it exists
        if (behavior->on_add) behavior->on_add(instance->obj_idx, get_actor_data(instance));
    }
}

void actors_on_destroy() {
    for (int i = 0; i < scene_header.actor_instances_count; i++) {
        ActorInstance* instance = &scene_actor_instances[i];

        if (!obj_is_destroyed(&scene_objs[instance->obj_idx])) continue;
        assert(instance->behavior_id < G_ACTOR_BEHAVIOR_COUNT);

        // Look up the shared behavior blueprint using the index
        const ActorBehavior* behavior = &g_actor_behavior_reg[instance->behavior_id];

        // Call the destroy function pointer if it exists
        if (behavior->on_destroy) behavior->on_destroy(instance->obj_idx, get_actor_data(instance));
    }
}

void actors_start() {
    for (int i = 0; i < scene_header.actor_instances_count; i++) {
        ActorInstance* instance = &scene_actor_instances[i];

        // Skip inactive/free slots
        if (!instance->enabled || !obj_should_render(&scene_objs[instance->obj_idx])) continue;
        assert(instance->behavior_id < G_ACTOR_BEHAVIOR_COUNT);

        // Look up the shared behavior blueprint using the index
        const ActorBehavior* behavior = &g_actor_behavior_reg[instance->behavior_id];

        // Call the start function pointer if it exists
        if (behavior->start) behavior->start(instance->obj_idx, get_actor_data(instance));
    }
}

void actors_update(float dt) {
    for (int i = 0; i < scene_header.actor_instances_count; i++) {
        ActorInstance* instance = &scene_actor_instances[i];

        // Skip inactive/free slots
        if (!instance->enabled || !obj_should_render(&scene_objs[instance->obj_idx])) continue;
        assert(instance->behavior_id < G_ACTOR_BEHAVIOR_COUNT);

        // Look up the shared behavior blueprint using the index
        const ActorBehavior* behavior = &g_actor_behavior_reg[instance->behavior_id];

        // Call the update function pointer if it exists
        if (behavior->update) behavior->update(instance->obj_idx, get_actor_data(instance), dt);
    }
}

void time_init(void) {
    last_ticks = timer_ticks();
}

float time_tick(void) {
    uint64_t current_ticks = timer_ticks();
    uint64_t delta_ticks = current_ticks - last_ticks;
    last_ticks = current_ticks;

    return (float)delta_ticks / (float)TICKS_PER_SECOND; // Convert ticks into fractional seconds
}

void libdragon_setup(void) {
    INIT_DEBUG(); // Initialize debug logging
    timer_init();
    
    dfs_init(DFS_DEFAULT_LOCATION); // Initialize the filesystem (allows to read files bundled into the ROM)
    display_init(
        (resolution_t){
            .height = ENGINE_RES_HEIGHT, 
            .width = ENGINE_RES_WIDTH, 
            .interlaced = ((ENGINE_RES_HEIGHT) > 288) ? INTERLACE_HALF : INTERLACE_OFF,
        }, 
        DEPTH_16_BPP, 
        FRAMEBUFFER_COUNT, 
        GAMMA_NONE, 
        FILTERS_RESAMPLE
    );

    rdpq_init();
    rdpq_mode_dithering(DITHER_MODE);
    joypad_init();
    t3d_init((T3DInitParams){});
}

void engine_init(void) {
    libdragon_setup(); // Libdragon init
    init_trig_table(); // Initialize trig.h sine table
    DBG_OVERLAY_INIT(); // Debug components
    engine_mem_init(); // Initialize engine memory arena
    scene_init(); // Initialize scene header
}

void engine_run(void) {
    // On Add pass
    actors_on_add(); // TODO: Handle what happens when objects/components aren't active at this state
    
    // Start pass
    actors_start(); // TODO: Handle what happens when objects/components aren't active at this state
    setup_lights();

    surface_t depth = surface_alloc(FMT_RGBA16, display_get_width(), display_get_height()); // Allocate Z-buffer in memory
    T3DViewport viewport = t3d_viewport_create();

    // Update pass
    while (1) {
        float delta_time = time_tick(); // Compute delta time for this frame
        if (delta_time > 0.1f) delta_time = 0.1f; // Avoid massive dt spikes if the engine hitches

        joypad_poll(); // Poll for inputs first
        actors_update(delta_time); // Update actors
        
        rdpq_attach(display_get(), &depth);

        t3d_frame_start(); // Start rendering a new frame
        t3d_viewport_attach(&viewport);

        t3d_screen_clear_color(RGBA32(0x20, 0x30, 0x60, 0xFF));
        t3d_screen_clear_depth();

        // Update camera
        Camera* main = camera_get_main();
        uint8_t mask = main ? main->culling_mask : RENDER_MASK_ALL;

        if (main) camera_update(camera_get_main(), &viewport);
        
        graphics_start_drawing(mask);
        DBG_DRAW(); // Draw debug overlay in debug builds if enabled
        rdpq_detach_show();

        // Destroy pass (TODO)
        actors_on_destroy();

        DBG_FRAME_END(); // Mark the frame as ended for the profiler
    }

    surface_free(&depth);
}
