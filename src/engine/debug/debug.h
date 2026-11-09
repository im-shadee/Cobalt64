#ifndef DEBUG_H
#define DEBUG_H

#include <libdragon.h>

#ifdef DEBUG
    #define INIT_DEBUG() debug_init(DEBUG_FEATURE_ALL)
    #define PRINT_LOG(fmt, ...) debug_printf(fmt, ##__VA_ARGS__)
#else
    #define INIT_DEBUG() ((void)0)
    #define PRINT_LOG(fmt, ...) ((void)0)
#endif

#endif // DEBUG_H
