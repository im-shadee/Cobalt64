#ifndef OBJ_H
#define OBJ_H

#include <stdint.h>
#include "layers.h"

// Packed ensures the enum only takes up 1 byte in memory
typedef enum __attribute__((packed)) {
    OBJ_TYPE_DEFAULT = 0,
    OBJ_TYPE_PLAYER,
    OBJ_TYPE_PROP,
    OBJ_TYPE_TRIGGER,
    OBJ_TYPE_CAMERA
} object_tags_t;

typedef struct {
    /** Set of bit flags representing object state. */
    union {
        uint8_t bits;
        struct {
            uint8_t active : 1; // 0x01: Whether this object is active (participating in the game loop)
            uint8_t visible : 1; // 0x02: Whether this object is visible (rendered by the graphics pipeline)
            uint8_t destroy : 1; // 0x04: Whether this object is pending destruction
            uint8_t has_mesh : 1;
            uint8_t has_collider : 1;
            uint8_t unused : 3;
        };
    } flags; // 1 byte

    render_layer_t layer; // Rendering layer of this object
    object_tags_t tag; // Tag of this object
    uint8_t unused; // Extra byte for padding
} object_t; // 4 bytes

/** 
 * Initializes an object in the object pool with a transform, set flags, a layer and a tag.
 * @return The index of that object in the object pool.
 */
uint16_t obj_init(uint16_t transform_index, uint8_t flags, render_layer_t layer, object_tags_t tag);

/** 
 * Sets an object from the object pool as destroyed. Its index in the pool becomes invalid.
 * remarks: It is not recommended to call it outside of the scene manager. 
 */
void obj_destroy(uint16_t index);

/** 
 * Checks whether the object is active.
 * @return 1 if active, 0 otherwise.
 */
static inline int obj_is_active(object_t* o) { return o->flags.active; }

/** 
 * Checks whether the object is visible.
 * @return 1 if visible, 0 otherwise.
 */
static inline int obj_is_visible(object_t* o) { return o->flags.visible; }

#endif // OBJ_H
