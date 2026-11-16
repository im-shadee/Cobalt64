/**
 * Copyright (c) 2026 Shadee
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef HIERARCHY_TREE_H
#define HIERARCHY_TREE_H

#include <stdint.h>

typedef struct {
    uint16_t parent_idx;
    uint16_t first_child_idx;
    uint16_t next_sibling_idx;
} HierarchyTree; // 6 bytes

#endif // HIERARCHY_TREE_H
