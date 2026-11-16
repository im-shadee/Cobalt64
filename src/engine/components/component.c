/**
 * Copyright (c) 2026 Shadee
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "component.h"
#include "config.h"
#include <stdint.h>
#include <assert.h>
#include <stddef.h>
#include <string.h>

typedef struct {
    // Component data
    comp_desc_t component_description;
    void* data;

    // Object/Component LUTs
    uint16_t* component_to_obj;
    uint16_t* obj_to_component;
    uint16_t count;
} ComponentPool; // 36 bytes

static ComponentPool pools[MAX_COMPONENT_TYPES];
static comp_type_t registered_types = 0;

static inline void* compute_component_address(uint16_t comp_slot, const ComponentPool* pool) {
    return (void*)((uint8_t*)pool->data + comp_slot * pool->component_description.type_size);
}

comp_type_t comp_register_static(const comp_desc_t* desc, void* data, uint16_t* dense_to_obj, uint16_t* obj_to_dense) {
    // Ensure that the amount of types never exceeds the maximum
    assert(registered_types < MAX_COMPONENT_TYPES);

    // Ensure passed pointers are not null pointers
    assert(desc);
    assert(data);

    ComponentPool* current = &pools[registered_types];

    current->count = 0;
    current->component_description = *desc;
    current->data = data;
    current->component_to_obj = dense_to_obj;
    current->obj_to_component = obj_to_dense;

    // Give none component value to each object (an object has no component by default)
    for (int i = 0; i < MAX_OBJECTS; i++) {
        obj_to_dense[i] = COMP_NONE;
    }

    // Return the current type index and then increases it
    return registered_types++;
}

void* comp_get(uint16_t obj_idx, comp_type_t type) {
    assert(obj_idx < MAX_OBJECTS);
    assert(type < registered_types);

    ComponentPool* pool = &pools[type]; // Get the pool for type

    // Read the component slot from the object index
    uint16_t component_slot = pool->obj_to_component[obj_idx];
    if (component_slot == COMP_NONE) return NULL;
    
    return compute_component_address(component_slot, pool);
}

void* comp_add(uint16_t obj_idx, comp_type_t type) {
    assert(obj_idx < MAX_OBJECTS);
    assert(type < registered_types);

    ComponentPool* pool = &pools[type];
    uint16_t component_slot = pool->obj_to_component[obj_idx];

    // If the pool already has this component, ignore and return the existing one
    if (component_slot != COMP_NONE) return compute_component_address(component_slot, pool);

    // Else, if the pool is full, return NULL
    if (pool->count >= pool->component_description.capacity) return NULL;

    // Otherwise, tie the component to the object and increment the component count in the pool
    uint16_t comp_pool_count = pool->count;
    pool->obj_to_component[obj_idx] = comp_pool_count;
    pool->component_to_obj[comp_pool_count] = obj_idx;

    void* component_address = compute_component_address(comp_pool_count, pool);
    memset(component_address, 0, pool->component_description.type_size);
    pool->count++;

    if (pool->component_description.on_add) {
        pool->component_description.on_add(obj_idx, component_address);
    }

    if (pool->component_description.start) {
        // Run the start function after the object's own on_add, for this specific object
        pool->component_description.start(component_address, &obj_idx);
    }

    return component_address;
}

void comp_remove(uint16_t obj_idx, comp_type_t type) {
    assert(obj_idx < MAX_OBJECTS);
    assert(type < registered_types);

    // Find the slot to remove
    ComponentPool* pool = &pools[type];
    uint16_t component_slot = pool->obj_to_component[obj_idx];

    // Ignore if the component doesn't exist on the object
    if (component_slot == COMP_NONE) return;

    uint16_t last_used_slot = pool->count - 1;
    void* remove_addr = compute_component_address(component_slot, pool);
    void* last_used_comp_addr = compute_component_address(last_used_slot, pool);

    if (pool->component_description.on_remove) {
        pool->component_description.on_remove(obj_idx, remove_addr);
    }

    if (component_slot != last_used_slot) {
        // Copy the last component's bytes over the removed one
        memcpy(remove_addr, last_used_comp_addr, pool->component_description.type_size);

        // Update the map
        uint16_t last_obj_idx = pool->component_to_obj[last_used_slot];
        pool->obj_to_component[last_obj_idx] = component_slot;
        pool->component_to_obj[component_slot] = last_obj_idx;
    }

    pool->obj_to_component[obj_idx] = COMP_NONE;
    pool->count--;
}

void comp_destroy(uint16_t obj_idx, comp_type_t type) {
    assert(obj_idx < MAX_OBJECTS);
    assert(type < registered_types);

    // Find the slot to tag for destruction
    ComponentPool* pool = &pools[type];
    uint16_t component_slot = pool->obj_to_component[obj_idx];

    // Ignore if the component doesn't exist on the object
    if (component_slot == COMP_NONE) return;

    comp_desc_t* desc = &pool->component_description;
    desc->destroy = TRUE;
}
