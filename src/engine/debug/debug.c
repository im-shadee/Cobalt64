/**
 * Copyright (c) 2026 Shadee
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifdef DEBUG
#include "config.h"
#include "debug.h"
#include "inputs.h"
#include <display.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define FONT_ID_DEBUG 1
#define CHAR_WIDTH 6

#define MEM_UPDT_TICKS (5 * TICKS_PER_SECOND)
#define FPS_UPDT_TICKS (TICKS_PER_SECOND)

static rdpq_font_t *font;
static bool show_mem = true, show_fps = true; // All debug flags by default

// Placeholder strings
static char mem_str[40]   = "MEM: --% (--/-- B)";
static char fps_str[16]   = "FPS: --";

// Framecount, and interval trackers for FPS and memory updates
static uint32_t last_fps_ticks, last_mem_ticks;

// Linker symbol marking the end of the BSS / static data segment in Libdragon
extern char __bss_end[];

void dbg_show_memory(bool on) { show_mem = on; }
void dbg_show_fps(bool on) { show_fps = on; }

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
    int w = display_get_width();
    // int h = display_get_height();
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
        heap_stats_t st; // Compte dynamic memory usage
        sys_get_heap_stats(&st);

        uint32_t static_used = (uint32_t)__bss_end - 0x80000000; // Compute static memory usage
        uint32_t total_used = static_used + st.used; // Compute total active memory in exact bytes

        // Calculate percentage against TARGET_MEM_BYTES
        int pct = TARGET_MEM_BYTES > 0 ? (int)((total_used * 100LL) / TARGET_MEM_BYTES) : 0;

        // Print exact byte counts without division
        snprintf(mem_str, sizeof(mem_str), "MEM: %d%% (%lu/%lu B)", 
            pct, 
            (unsigned long)total_used, 
            (unsigned long)TARGET_MEM_BYTES
        );

        last_mem_ticks = now;
    }
}
#endif
