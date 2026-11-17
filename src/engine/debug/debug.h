#ifndef DEBUG_H
#define DEBUG_H

#include <libdragon.h>

#ifdef DEBUG
#define INIT_DEBUG() debug_init(DEBUG_FEATURE_ALL)
#define PRINT_LOG(fmt, ...) debug_printf(fmt, ##__VA_ARGS__)

void dbg_overlay_init(void);
void dbg_overlay_free(void);
void dbg_show_memory(int on);
void dbg_show_fps(int on);
void dbg_overlay_draw(void); // after 3D draw, before rdpq_detach_show()
void dbg_frame_end(void); // right after rdpq_detach_show()

#define DBG_OVERLAY_INIT() dbg_overlay_init()
#define DBG_OVERLAY_FREE() dbg_overlay_free()
#define DBG_DRAW() dbg_overlay_draw()
#define DBG_FRAME_END() dbg_frame_end()
#else
#define INIT_DEBUG() ((void)0)
#define PRINT_LOG(fmt, ...) ((void)0)
#define dbg_show_inputs(on) ((void)0)
#define dbg_show_memory(on) ((void)0)
#define dbg_show_fps(on) ((void)0)
#define DBG_OVERLAY_INIT() ((void)0)
#define DBG_OVERLAY_FREE() ((void)0)
#define DBG_UPDATE(port) ((void)0)
#define DBG_DRAW() ((void)0)
#define DBG_FRAME_END() ((void)0)
#endif

#endif
