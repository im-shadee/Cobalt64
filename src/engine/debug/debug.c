/**
 * Copyright (c) 2026 Shadee
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifdef DEBUG
#include "debug.h"
#include "inputs.h"
#include "bool.h"
#include <stdio.h>
#include <display.h>
#include <string.h>

#define FONT_ID_DEBUG 1
#define CHAR_WIDTH 6

#define MEM_UPDT_TICKS (5 * TICKS_PER_SECOND)
#define FPS_UPDT_TICKS (TICKS_PER_SECOND)

static rdpq_font_t *font;
static bool_t show_mem = TRUE, show_fps = TRUE; // All debug flags by default

// Placeholder strings
static char mem_str[40]   = "MEM: --% (--/-- B)";
static char fps_str[16]   = "FPS: --";

// Framecount, and interval trackers for FPS and memory updates
static uint32_t last_fps_ticks, last_mem_ticks;

void dbg_show_memory(bool_t on) { show_mem = on; }
void dbg_show_fps(bool_t on) { show_fps = on; }

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

void dbg_overlay_draw(void) {
    int w = display_get_width(), h = display_get_height();
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
