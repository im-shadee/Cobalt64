/**
 * Copyright (c) 2026 Shadee
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef COMPONENT_H
#define COMPONENT_H

#include "config.h"
#include "bool.h"
#include <stdint.h>

#define COMP_NONE 0xFFFFu
#define COMP_TYPE_INVALID 0xFF

typedef uint8_t comp_type_t;

typedef struct {
    /**
     * Runs when the component is being initialized.
     * remarks: Only runs once the corresponding object is active.
     */
    void (*start)(uint16_t obj_idx, void* self);

    /** 
     * Runs once every frame. 
     * remarks: Only runs once the corresponding object is active.
     */
    void (*update)(uint16_t obj_idx, void* self, float dt);

    /**
     * Runs once before the update loop.
     * remarks: Only runs once the corresponding object is active.
     */
    void (*on_add)(uint16_t obj_idx, void* self);

    /** Runs right before object destruction. */
    void (*on_remove)(uint16_t obj_idx, void* self);

    uint16_t type_size;
    uint16_t capacity;
    bool_t destroy;
} comp_desc_t; // 24 bytes (padded)

/**
 * Registers a component type. Fills obj_to_dense with COMP_NONE.
 * @return The new type ID.
 */
comp_type_t comp_register_static(const comp_desc_t* desc, void* data, uint16_t* dense_to_obj, uint16_t* obj_to_dense);

/** Returns the component of the given type on an object, or NULL if it has none. */
void* comp_get(uint16_t obj_idx, comp_type_t type);

/** Adds a component of the given type to an object. Returns NULL if the pool is full. */
void* comp_add(uint16_t obj_idx, comp_type_t type);

/** 
 * Removes a component from an object. 
 * remarks: Never call directly. Reserved for engine lifecycle.
*/
void comp_remove(uint16_t obj_idx, comp_type_t type);

/** 
 * Marks the component as pending destruction. 
 * The component will be destroyed at the end of the frame 
 */
void comp_destroy(uint16_t obj_idx, comp_type_t type);

#define COMP_DECLARE(NAME, T)                                           \
    extern comp_type_t COMP_##NAME;                                     \
    static inline T* NAME##_get(uint16_t obj) {                         \
        return (T*)comp_get(obj, COMP_##NAME);                          \
    }                                                                   \
    static inline T* NAME##_add(uint16_t obj) {                         \
        return (T*)comp_add(obj, COMP_##NAME);                          \
    }                                                                   \
    static inline void NAME##_destroy(uint16_t obj) {                   \
        comp_destroy(obj, COMP_##NAME);                                 \
    }                                                                   \
    void NAME##_register(void);

#define COMP_DEFINE(NAME, T, CAP, ...)                                  \
    comp_type_t COMP_##NAME;                                            \
    static T NAME##_storage[CAP] __attribute__((aligned(8)));           \
    static uint16_t NAME##_dense_to_obj[CAP];                           \
    static uint16_t NAME##_obj_to_dense[MAX_OBJECTS];                   \
    void NAME##_register(void) {                                        \
        COMP_##NAME = comp_register_static(&(comp_desc_t){              \
            .type_size = sizeof(T), .capacity = CAP, __VA_ARGS__ },     \
            NAME##_storage, NAME##_dense_to_obj, NAME##_obj_to_dense);  \
    }

#endif // COMPONENT_H
