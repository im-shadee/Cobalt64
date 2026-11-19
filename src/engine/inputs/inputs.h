#ifndef INPUTS_H
#define INPUTS_H

#include "bool.h"
#include "vectors.h"
#include <libdragon.h>

#define JOY_DRIFT_THRESHOLD 30
#define JOY_MAX_STICK_MAGNITUDE 80.0f
#define JOY_INV_STICK_MAGNITUDE 0.0125f

typedef enum {
    IN_BTN_A, 
    IN_BTN_B, 
    IN_BTN_START, 
    IN_BTN_L, 
    IN_BTN_R,
    IN_BTN_Z,
    IN_BTN_C_UP,
    IN_BTN_C_DOWN,
    IN_BTN_C_LEFT,
    IN_BTN_C_RIGHT
} input_button_t;

/** 
 * Checks the state of specified button in a given joypad button structure.
 * @return 1 if pressed, 0 otherwise.
 */
static inline int inputs_get_button_state(input_button_t btn, joypad_buttons_t buttons) {
    switch (btn) {
        case IN_BTN_A: return buttons.a;
        case IN_BTN_B: return buttons.b;
        case IN_BTN_START: return buttons.start;
        case IN_BTN_L: return buttons.l;
        case IN_BTN_R: return buttons.r;
        case IN_BTN_Z: return buttons.z;
        case IN_BTN_C_UP: return buttons.c_up;
        case IN_BTN_C_DOWN: return buttons.c_down;
        case IN_BTN_C_LEFT: return buttons.c_left;
        case IN_BTN_C_RIGHT: return buttons.c_right;
        default: return FALSE;
    }
}

/**
 * Checks whether a button on the specified joypad port (1-4) has been pressed this frame.
 * @return 1 if the button is pressed, 0 otherwise.
 */
static inline int inputs_pressed_this_frame(input_button_t btn, joypad_port_t port) {
    joypad_buttons_t pressed = joypad_get_buttons_pressed(port);
    return inputs_get_button_state(btn, pressed);
}

/**
 * Checks whether a button on the specified joypad port (1-4) has been released this frame.
 * @return 1 if the button is released, 0 otherwise.
 */
static inline int inputs_released_this_frame(input_button_t btn, joypad_port_t port) {
    joypad_buttons_t released = joypad_get_buttons_released(port);
    return inputs_get_button_state(btn, released);
}

/**
 * Checks whether a button on the specified joypad port (1-4) was held this frame.
 * @return 1 if the button is held, 0 otherwise.
 */
static inline int inputs_held(input_button_t btn, joypad_port_t port) {
    joypad_buttons_t held = joypad_get_buttons_held(port);
    return inputs_get_button_state(btn, held);
}

/** 
 * Reads the X and Y axis of the controller's joystick on the specified port.
 * @return A vector whose XY components are the normalized joystick's axis.
 * 
 * remarks: Uses squared-magnitude branch optimization to eliminate sqrt and division overhead for 99% of inputs.
 */
static inline vec2f_t inputs_read_joystick(joypad_port_t port) {
    joypad_inputs_t inputs = joypad_get_inputs(port);

    vec2f_t raw = {
        .x = (float)inputs.stick_x,
        .y = (float)inputs.stick_y
    };

    // Check deadzone using squared magnitude
    float magnitude_sqr = vec2f_magnitude_squared(raw);
    if (magnitude_sqr < JOY_DRIFT_THRESHOLD * JOY_DRIFT_THRESHOLD) return vec2f_zero();

    // Normal range check to avoid heavy sqrt/div calculations during gameplay
    if (magnitude_sqr <= JOY_MAX_STICK_MAGNITUDE * JOY_MAX_STICK_MAGNITUDE) {
        const float inv_max = JOY_INV_STICK_MAGNITUDE;
        return vec2f_scale(raw, inv_max);
    }

    // Diagonal overshoot case
    float magnitude = sqrtf(magnitude_sqr);
    float scale = 1.0f / magnitude; // Clamps to unit circle edge
    return vec2f_scale(raw, scale);
}

#endif // INPUTS_H
