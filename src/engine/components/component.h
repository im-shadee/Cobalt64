/**
 * Copyright (c) 2026 Shadee
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef COMPONENT_H
#define COMPONENT_H

#include "config.h"
#include <stdint.h>

#define COMP_NONE 0xFFFFu
#define COMP_TYPE_INVALID 0xFF

typedef uint8_t comp_type_t;

typedef struct {
    uint16_t type_size;
    uint16_t capacity;
    void (*awake)(void* comps, const uint16_t* owners, uint16_t count);
    void (*update)(void* comps, const uint16_t* owners, uint16_t count, float dt);
} comp_desc_t;

/**
 * Registers a component type. Fills obj_to_dense with COMP_NONE.
 * @return The new type ID.
 */
comp_type_t comp_register_static(const comp_desc_t* desc, void* data, uint16_t* dense_to_obj, uint16_t* obj_to_dense);

/** Returns the component of the given type on an object, or NULL if it has none. */
void* comp_get(uint16_t obj_idx, comp_type_t type);

/** Adds a component of the given type to an object. Returns NULL if the pool is full. */
void* comp_add(uint16_t obj_idx, comp_type_t type);

/** Removes a component from an object. */
void comp_remove(uint16_t obj_idx, comp_type_t type);

#define COMP_DECLARE(NAME, T)                                           \
    extern comp_type_t COMP_##NAME;                                     \
    static inline T* NAME##_get(uint16_t obj) {                         \
        return (T*)comp_get(obj, COMP_##NAME);                          \
    }                                                                   \
    static inline T* NAME##_add(uint16_t obj) {                         \
        return (T*)comp_add(obj, COMP_##NAME);                          \
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
