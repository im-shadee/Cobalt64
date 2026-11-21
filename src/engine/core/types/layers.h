#ifndef LAYERS_H
#define LAYERS_H

#define RENDER_MASK_NONE 0x00
#define RENDER_MASK_ALL  0xFF

// Packed ensures the enum only takes up 1 byte in memory
typedef enum __attribute__((packed)) {
    RENDER_LAYER_SKYBOX = 0,
    RENDER_LAYER_OPAQUE,
    RENDER_LAYER_TRANSPARENT,
    RENDER_LAYER_UI
} render_layer_t;

typedef enum __attribute__((packed)) {
    LIGHT_LAYER_NONE = 0,
    /*0x01*/ LIGHT_LAYER_0 = 1 << 0,
    /*0x02*/ LIGHT_LAYER_1 = 1 << 1,
    /*0x04*/ LIGHT_LAYER_2 = 1 << 2,
    /*0x08*/ LIGHT_LAYER_3 = 1 << 3,
    /*0x10*/ LIGHT_LAYER_4 = 1 << 4,
    /*0x20*/ LIGHT_LAYER_5 = 1 << 5,
    /*0x40*/ LIGHT_LAYER_6 = 1 << 6,
    /*0x80*/ LIGHT_LAYER_7 = 1 << 7,
    LIGHT_LAYER_ALL  = 0xFF
} light_layer_t;

#endif // LAYERS_H
