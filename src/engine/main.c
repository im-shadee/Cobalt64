#include <libdragon.h>
#include <t3d/t3d.h>
#include <t3d/t3dmodel.h>
#include "transform.h"
#include "trig.h"

#define FB_COUNT 3

/**
 * Converts the engine's float render matrix to Tiny3D's fixed-point matrix.
 *
 * The engine stores the translation in m[0..2][3] (column-vector convention),
 * Tiny3D expects it in m[3][0..2], so the matrix is transposed while copying.
 */
static void transform_to_t3d(T3DMat4FP *out, const mtx4f_t *in) {
    T3DMat4 m;
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            m.m[c][r] = in->m[r][c];
        }
    }
    t3d_mat4_to_fixed(out, &m);
}

int main(void) {
    init_trig_table();

    debug_init_isviewer();
    debug_init_usblog();
    asset_init_compression(2);
    dfs_init(DFS_DEFAULT_LOCATION);

    display_init(RESOLUTION_320x240, DEPTH_16_BPP, FB_COUNT, GAMMA_NONE, FILTERS_RESAMPLE);
    rdpq_init();
    joypad_init();
    t3d_init((T3DInitParams){});

    surface_t depth = surface_alloc(FMT_RGBA16, display_get_width(), display_get_height());

    T3DViewport viewport = t3d_viewport_create();
    T3DVec3 cam_pos    = {{0, 0, 0}};
    T3DVec3 cam_target = {{0, 0, -1}};
    T3DVec3 cam_up     = {{0, 1, 0}};

    T3DModel *model = t3d_model_load("rom:/cube.t3dm");
    assert(model);

    // One matrix per framebuffer, so we never overwrite one the RDP is still reading
    T3DMat4FP *mtx = malloc_uncached(sizeof(T3DMat4FP) * FB_COUNT);

    vec3f_t pos   = { .x = 0.0f, .y = 0.0f, .z = -200.0f };
    vec3f_t rot   = { .x = 0.0f, .y = 0.0f, .z = 0.0f };   // degrees
    vec3f_t scale = { .x = 1.0f, .y = 1.0f, .z = 1.0f };
    vec3f_t shear = { .x = 0.0f, .y = 0.0f, .z = 0.0f };

    transform_t *cube_tr = transform_create(pos, rot, scale, shear);
    if (!cube_tr) return -1;

    // Build the render matrix once up front, so the first frame doesn't use the identity
    update_render_mtx(&cube_tr->render_mtx, cube_tr->position, cube_tr->rotation,
                      cube_tr->scale, cube_tr->shear);

    float rotation_speed = 1.0f; // degrees per frame
    int fb = 0;

    while (1) {
        joypad_poll();

        rot.y += rotation_speed;
        transform_set_rotation(cube_tr, rot);

        // Rebuild the engine's render matrix only when the transform changed
        if (cube_tr->is_dirty) {
            update_render_mtx(&cube_tr->render_mtx, cube_tr->position, cube_tr->rotation,
                              cube_tr->scale, cube_tr->shear);
            cube_tr->is_dirty = FALSE;
        }

        // Engine matrix -> Tiny3D fixed-point matrix for this framebuffer
        transform_to_t3d(&mtx[fb], &cube_tr->render_mtx);

        t3d_viewport_set_projection(&viewport, T3D_DEG_TO_RAD(85.0f), 10.0f, 1000.0f);
        t3d_viewport_look_at(&viewport, &cam_pos, &cam_target, &cam_up);

        rdpq_attach(display_get(), &depth);
        t3d_frame_start();
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

        t3d_matrix_push(&mtx[fb]);
        t3d_model_draw(model);
        t3d_matrix_pop(1);

        rdpq_detach_show();
        fb = (fb + 1) % FB_COUNT;
    }

    free(cube_tr);
    t3d_model_free(model);
    free_uncached(mtx);
    t3d_destroy();
    return 0;
}
