#include <libdragon.h>
#include <t3d/t3d.h>
#include <t3d/t3dmodel.h>
#include "transform.h"
#include "bool.h"
#include "debug.h"
#include "trig.h"
#include "inputs.h"

// Rendering
#define FB_COUNT 2

// Camera
#define FOV_DEG 85.0f
#define CAMERA_NEAR 10.0f
#define CAMERA_FAR 800.0f

// Misc
#define FONT_ID_DEBUG 1
#define INPUT_BUFFER_SIZE 32

/**
 * Converts the engine's float render matrix to Tiny3D's fixed-point matrix.
 *
 * The engine stores the translation in m[0..2][3] (column-vector convention),
 * Tiny3D expects it in m[3][0..2], so the matrix is transposed while copying.
 */
static void transform_to_t3d(T3DMat4FP *out, const mtx4f_t *in) {
    T3DMat4 m;
    // Map axes: swap X (0) and Y (1), leave Z (2) and W (3) unchanged
    const int map[4] = {1, 0, 2, 3};

    #pragma GCC unroll 4
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            m.m[j][i] = in->m[map[i]][map[j]];
        }
    }
    t3d_mat4_to_fixed(out, &m);
}

static inline void init_console(void) {
    INIT_DEBUG(); // Initialize debug logging
    
    init_trig_table(); // Initialize trig.h sine table
    dfs_init(DFS_DEFAULT_LOCATION); // Initialize the filesystem (allows to read files bundled into the ROM)

    // Initialize display with default N64 resolution, two framebuffers, no gamma correction, 
    // and resampling when scaling
    // Using two framebuffers consume ~307 KB of memory
    display_init(RESOLUTION_320x240, DEPTH_16_BPP, FB_COUNT, GAMMA_NONE, FILTERS_RESAMPLE);

    rdpq_init();
    joypad_init();
    t3d_init((T3DInitParams){});
}

// Helper function using a switch statement to retrieve the button's display name
static const char* get_button_name(input_button_t btn) {
    switch (btn) {
        case IN_BTN_A:      return "Button: A";
        case IN_BTN_B:      return "Button: B";
        case IN_BTN_START:  return "Button: START";
        case IN_BTN_L:      return "Button: L";
        case IN_BTN_R:      return "Button: R";
        case IN_BTN_Z:      return "Button: Z";
        case IN_BTN_C_UP:   return "C-Button: UP";
        case IN_BTN_C_DOWN: return "C-Button: DOWN";
        case IN_BTN_C_LEFT: return "C-Button: LEFT";
        case IN_BTN_C_RIGHT: return "C-Button: RIGHT";
        default:            return "Unknown";
    }
}

static void get_pressed_input(char input_name[], size_t buffer_size, joypad_port_t port) {
    static const input_button_t all_buttons[] = {
        IN_BTN_A, IN_BTN_B, IN_BTN_START, 
        IN_BTN_L, IN_BTN_R, IN_BTN_Z,
        IN_BTN_C_UP, IN_BTN_C_DOWN, 
        IN_BTN_C_LEFT, IN_BTN_C_RIGHT
    };

    for (int i = 0; i < sizeof(all_buttons) / sizeof(all_buttons[0]); i++) {
        input_button_t btn = all_buttons[i];

        if (inputs_held(btn, port)) {
            snprintf(input_name, buffer_size, "%s", get_button_name(btn));
            return;
        }
    }

    vec2f_t stick = inputs_read_joystick(port);

    if (f32_abs(stick.x) > 0.01f || f32_abs(stick.y) > 0.01f) {
        if (f32_abs(stick.x) >= f32_abs(stick.y)) {
            snprintf(input_name, buffer_size, "Axis X: %.2f", stick.x);
        } else {
            snprintf(input_name, buffer_size, "Axis Y: %.2f", stick.y);
        }
        return;
    }

    snprintf(input_name, buffer_size, "None");
}

int main(void) {
    init_console();

    rdpq_font_t *font = rdpq_font_load_builtin(FONT_BUILTIN_DEBUG_MONO);
    rdpq_text_register_font(FONT_ID_DEBUG, font);

    // Persistent allocated text to log inputs
    char latest_input_name[INPUT_BUFFER_SIZE] = "None"; 

    // Allocate Z-buffer in memory
    surface_t depth = surface_alloc(FMT_RGBA16, display_get_width(), display_get_height());

    // Camera setup
    T3DViewport viewport = t3d_viewport_create(); // Create a 3D viewport
    T3DVec3 cam_pos    = {{0, 0, 0}};
    T3DVec3 cam_target = {{0, 0, -1}}; // Define the 3D coordinate the camera is pointing toward
    T3DVec3 cam_up     = {{0, 1, 0}}; // Define the Y axis as the "up" vector

    // One matrix per framebuffer, so we never overwrite one the RDP is still reading
    T3DMat4FP *mtx = malloc_uncached(sizeof(T3DMat4FP) * FB_COUNT);

    // Load the cube model and ensure it is not corrupted
    // Call model_free once not in use anymore
    T3DModel *model = t3d_model_load("rom:/cube.t3dm");
    assert(model);

    // Cube model transform attributes in the scene
    vec3f_t pos   = { .x = 0.0f, .y = 0.0f, .z = -200.0f };
    vec3f_t rot   = { .x = 0.0f, .y = 0.0f, .z = 0.0f };
    vec3f_t scale = { .x = 1.0f, .y = 1.0f, .z = 1.0f };
    vec3f_t shear = { .x = 0.0f, .y = 0.0f, .z = 0.0f };

    transform_t *cube_tr = transform_create(pos, rot, scale, shear);
    if (!cube_tr) return -1;

    // Tracks the buffer to draw the image on this rendering pass
    int framebuffer_index = 0;

    while (1) {
        joypad_poll();
        get_pressed_input(latest_input_name, INPUT_BUFFER_SIZE, JOYPAD_PORT_1);

        // Rotate the cube in the direction of the stick
        vec2f_t input_vector = inputs_read_joystick(JOYPAD_PORT_1);

        // Invert the XY axis to reverse the rotation direction
        vec2f_reverse(input_vector);

        // Rotate the cube by the XY values of the joystick
        vec3f_t angles = vec3f_add(cube_tr->rotation, vec2f_to_vec3f(input_vector));

        // Apply the new angles to the transform
        transform_set_rotation(cube_tr, angles);

        // Rebuild the engine's render matrix only when the transform actually changes
        if (cube_tr->is_dirty) {
            update_render_mtx(
                &cube_tr->render_mtx, 
                cube_tr->position, 
                cube_tr->rotation,         
                cube_tr->scale, 
                cube_tr->shear
            );

            cube_tr->is_dirty = FALSE; // The transform is flagged as non-dirty after the update
        }

        // Convert the engine's matrix into a Tiny3D matrix for the framebuffer
        transform_to_t3d(&mtx[framebuffer_index], &cube_tr->render_mtx);

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

        rdpq_text_print(NULL, FONT_ID_DEBUG, 10, display_get_height() - 10, latest_input_name);

        t3d_light_set_ambient(ambient);
        t3d_light_set_directional(0, light_color, &light_dir);
        t3d_light_set_count(1);

        t3d_matrix_push(&mtx[framebuffer_index]);
        t3d_model_draw(model);
        t3d_matrix_pop(1);

        rdpq_detach_show();
        framebuffer_index = (framebuffer_index + 1) % FB_COUNT; // Draws on the next framebuffer next frame
    }

    free(cube_tr);
    t3d_model_free(model);
    free_uncached(mtx);
    t3d_destroy();
    return 0;
}
