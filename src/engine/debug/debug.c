/**
 * Copyright (c) 2026 Shadee
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifdef DEBUG
#include "debug.h"
#include "inputs.h"
#include "custom_math.h"
#include "bool.h"
#include <stdio.h>
#include <display.h>
#include <string.h>

#define FONT_ID_DEBUG 1
#define CHAR_WIDTH 6

#define MEM_UPDT_TICKS (5 * TICKS_PER_SECOND)
#define FPS_UPDT_TICKS (TICKS_PER_SECOND)

static rdpq_font_t *font;
static int show_inputs = TRUE, show_mem = TRUE, show_fps = TRUE; // All debug flags by default (TODO: change to enum?)

// Placeholder strings
static char input_str[32] = "None";
static char mem_str[40]   = "MEM: --% (--/-- B)";
static char fps_str[16]   = "FPS: --";

// Framecount, and interval trackers for FPS and memory updates
static uint32_t last_fps_ticks, last_mem_ticks;

void dbg_show_inputs(int on) { show_inputs = on; }
void dbg_show_memory(int on) { show_mem = on; }
void dbg_show_fps(int on) { show_fps = on; }

void dbg_overlay_init(void) {
     font = rdpq_font_load_builtin(FONT_BUILTIN_DEBUG_MONO);
    rdpq_text_register_font(FONT_ID_DEBUG, font);

    uint32_t now = get_ticks();
    last_fps_ticks = now - FPS_UPDT_TICKS; // refresh on first frame
    last_mem_ticks = now - MEM_UPDT_TICKS;
}

void dbg_overlay_free(void) {
    rdpq_text_unregister_font(FONT_ID_DEBUG);
    rdpq_font_free(font);
}

static const char* button_name(input_button_t btn) {
    switch (btn) {
        case IN_BTN_A:       return "Button: A";
        case IN_BTN_B:       return "Button: B";
        case IN_BTN_START:   return "Button: START";
        case IN_BTN_L:       return "Button: L";
        case IN_BTN_R:       return "Button: R";
        case IN_BTN_Z:       return "Button: Z";
        case IN_BTN_C_UP:    return "C-Button: UP";
        case IN_BTN_C_DOWN:  return "C-Button: DOWN";
        case IN_BTN_C_LEFT:  return "C-Button: LEFT";
        case IN_BTN_C_RIGHT: return "C-Button: RIGHT";
        default:             return "Unknown";
    }
}

static void read_input(joypad_port_t port) {
    static const input_button_t btns[] = {
        IN_BTN_A, IN_BTN_B, IN_BTN_START, IN_BTN_L, IN_BTN_R, IN_BTN_Z,
        IN_BTN_C_UP, IN_BTN_C_DOWN, IN_BTN_C_LEFT, IN_BTN_C_RIGHT
    };

    for (unsigned i = 0; i < sizeof(btns) / sizeof(btns[0]); i++) {
        if (inputs_held(btns[i], port)) {
            snprintf(input_str, sizeof(input_str), "%s", button_name(btns[i]));
            return;
        }
    }

    vec2f_t s = inputs_read_joystick(port);
    if (f32_abs(s.x) > 0.01f || f32_abs(s.y) > 0.01f)
    {
        if (f32_abs(s.x) >= f32_abs(s.y)) snprintf(input_str, sizeof(input_str), "Axis X: %.2f", s.x);
        else snprintf(input_str, sizeof(input_str), "Axis Y: %.2f", s.y);
        return;
    }
    strcpy(input_str, "None");
}

void dbg_overlay_update(joypad_port_t port) { if (show_inputs) read_input(port); }

void dbg_overlay_draw(void) {
    int w = display_get_width(), h = display_get_height();
    if (show_inputs) rdpq_text_print(NULL, FONT_ID_DEBUG, 10, h - 10, input_str);
    if (show_mem) rdpq_text_print(NULL, FONT_ID_DEBUG, w - (int)strlen(mem_str) * CHAR_WIDTH - 10, 10, mem_str);
    if (show_fps) rdpq_text_print(NULL, FONT_ID_DEBUG, w - (int)strlen(fps_str) * CHAR_WIDTH - 10, 20, fps_str);
}

void dbg_frame_end(void) {
    uint32_t now = get_ticks();

    if (show_fps && TICKS_DISTANCE(last_fps_ticks, now) >= FPS_UPDT_TICKS) {
        snprintf(fps_str, sizeof(fps_str), "FPS: %.1f", display_get_fps());
        last_fps_ticks = now;
    }

    if (show_mem && TICKS_DISTANCE(last_mem_ticks, now) >= MEM_UPDT_TICKS) {
        heap_stats_t st;
        sys_get_heap_stats(&st);
        int pct = st.total > 0 ? (int)((st.used * 100LL) / st.total) : 0;
        snprintf(mem_str, sizeof(mem_str), "MEM: %d%% (%d/%d B)", pct, st.used, st.total);
        last_mem_ticks = now;
    }
}
#endif
