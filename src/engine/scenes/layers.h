#ifndef LAYERS_H
#define LAYERS_H

// Packed ensures the enum only takes up 1 byte in memory
typedef enum __attribute__((packed)) {
    RENDER_LAYER_SKYBOX = 0,
    RENDER_LAYER_OPAQUE,
    RENDER_LAYER_TRANSPARENT,
    RENDER_LAYER_UI
} render_layer_t;

#endif // LAYERS_H
