/**
 * Copyright (c) 2026 Shadee
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef OBJ_H
#define OBJ_H

#include "bool.h"
#include "layers.h"
#include <stdint.h>

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
            /*0x01*/ uint8_t active_self : 1; // Whether this object is active (participating in the game loop)
            /*0x02*/ uint8_t active_in_hierarchy : 1; // Whether this object is active in the hierarchy
            /*0x04*/ uint8_t visible : 1; // Whether this object is visible (rendered by the graphics pipeline)
            /*0x08*/ uint8_t destroy : 1; // Whether this object is pending destruction
            /*0x10*/ uint8_t free : 1; // Whether this object is free, i.e. a free slot in the object pool
            /*0x18*/ uint8_t is_static : 1; // Whether this object is static (no transform)
            uint8_t unused : 2;
        };
    } flags; // 1 byte

    render_layer_t layer; // Rendering layer of this object
    object_tags_t tag; // Tag of this object
    uint8_t padding; // Extra byte for padding
} Object; // 4 bytes

_Static_assert(sizeof(Object) == 4, "Object must be 4 bytes");

/** 
 * Initializes an object in the object pool with set flags, a layer and a tag.
 * @return The index of that object in the object pool.
 */
uint16_t obj_init(uint8_t flags, render_layer_t layer, object_tags_t tag);

/**
 * Sets an object from the object pool as destroyed. Its index in the pool becomes invalid.
 * This runs on_remove hooks on all of the object's components.
 */
void obj_destroy(uint16_t index);

/**
 * Retrieves the object at position 'index' in the object array.
 * @return A pointer to the object, or NULL if free/destroyed.
 */
//static inline Object* obj_get(uint16_t index) { return scene_get_object(index); }

/** Toggles the object's active state. */
void obj_set_active(Object* o, bool_t value);

/** Toggles the object's visibility state. */
static inline void obj_set_visible(Object* o, bool_t value) { o->flags.visible = value; }

/** 
 * Checks whether the object is active.
 * @return 1 if active, 0 otherwise.
 * 
 * remarks: An object is "active" if it is locally active and its hierarchy path is active.
 */
static inline bool_t obj_is_active(Object* o) { 
    return o->flags.active_self && o->flags.active_in_hierarchy; 
}

/** 
 * Checks whether the object is visible.
 * @return 1 if visible, 0 otherwise.
 */
static inline bool_t obj_is_visible(Object* o) { return o->flags.visible; }

/** 
 * Checks whether the object is pending destruction.
 * @return 1 if destroyed, 0 otherwise.
 */
static inline bool_t obj_is_destroyed(Object* o) { return o->flags.destroy; }

/** 
 * Checks whether the object is static.
 * @return 1 if static, 0 otherwise.
 */
static inline bool_t obj_is_static(Object* o) { return o->flags.is_static; }

/**
 * Checks whether an object should render this frame.
 * @return 1 if true, 0 otherwise.
 */
static inline bool_t obj_should_render(Object* o) { 
    return obj_is_active(o) && o->flags.visible && !o->flags.free && !o->flags.destroy; 
}

#endif // OBJ_H
