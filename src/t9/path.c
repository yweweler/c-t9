/*!
  ******************************************************************************
  * @file    path.c
  * @author  Yves-Noel Weweler <y.weweler@fh-muenster.de>
  * @version V1.0.0
  * @brief   This file implements a structure that describes path taken through
  *          a search tree.
  ******************************************************************************
  * @attention
  *
  * MIT License
  *
  * Copyright (c) 2017 Yves-Noel Weweler
  *
  * Permission is hereby granted, free of charge, to any person obtaining a copy
  * of this software and associated documentation files (the "Software"), to deal
  * in the Software without restriction, including without limitation the rights
  * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  * copies of the Software, and to permit persons to whom the Software is
  * furnished to do so, subject to the following conditions:
  *
  * The above copyright notice and this permission notice shall be included in
  * all copies or substantial portions of the Software.
  *
  * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  * SOFTWARE.
  *
  ******************************************************************************
  */

#include <stdio.h>

#include "t9/path.h"

t9_path_t *
t9_path_create() {
    t9_path_t *path;

    // Allocate memory.
    path = (t9_path_t *) malloc(sizeof(t9_path_t));
    if (path == NULL) {
        return NULL;
    }

    // Erase allocated memory.
    memset(path, 0, sizeof(t9_path_t));

    // Initialize list of nodes that make up the path.
    kv_init(path->nodes);

    return path;
}

void
t9_path_destroy(t9_path_t *const path) {
    if (path == NULL) {
        return;
    }

    // Destroy list of nodes (The nodes itself should not be destroyed).
    kv_destroy(path->nodes);

    // Erase and free the memory.
    memset(path, 0, sizeof(t9_path_t));
    free(path);
}

void
t9_path_push(t9_path_t *const path, t9_search_node_t *const node) {
    path->probability = node->probability;
    kv_push(t9_search_node_t *, path->nodes, node);
}

t9_search_node_t *
t9_path_pop(t9_path_t *const path) {
    path->probability = -1.0f;
    return kv_pop(path->nodes);
}

t9_path_t *
t9_path_duplicate(const t9_path_t *const path) {
    uint32_t i;
    t9_path_t *clone;

    if (path == NULL) {
        return NULL;
    }

    clone = t9_path_create();
    if (clone == NULL) {
        return NULL;
    }

    // IMPROVEMENT: Use kv_copy instead.
    clone->probability = path->probability;
    for (i = 0; i < kv_size(path->nodes); i++) {
        kv_push(t9_search_node_t *, clone->nodes, kv_A(path->nodes, i));
    }

    return clone;
}

t9_symbol_t *
t9_path_flatten(const t9_path_t *const path) {
    t9_symbol_t *nodes_str;
    size_t length;
    uint32_t i;

    if (path == NULL) {
        return NULL;
    }

    if (kv_size(path->nodes) == 0) {
        return NULL;
    }

    length = sizeof(t9_symbol_t) * kv_size(path->nodes) + sizeof(t9_symbol_t);
    nodes_str = (t9_symbol_t *) malloc(length);
    if (nodes_str == NULL) {
        return NULL;
    }

    for (i = 0; i < kv_size(path->nodes); ++i) {
        nodes_str[i] = kv_A(path->nodes, i)->symbol;
    }
    nodes_str[kv_size(path->nodes)] = 0;

    return nodes_str;
}

char *
t9_path_str(const t9_path_t *const path) {
    char *str;
    t9_symbol_t *nodes_str;

    if (path == NULL) {
        return NULL;
    }

    if (kv_size(path->nodes) == 0) {
        return NULL;
    }

    nodes_str = t9_path_flatten(path);
    asprintf(&str, "\"%s\" : %f", nodes_str, path->probability);

    free(nodes_str);
    return str;
}

bool
t9_path_is_equal(const t9_path_t *const path_a, const t9_path_t *const path_b) {
    size_t i;

    if (kv_size(path_a->nodes) != kv_size(path_b->nodes)) {
        return false;
    }

    for (i = kv_size(path_a->nodes) - 1; i > 0; i--) {
        if (kv_A(path_a->nodes, i) != kv_A(path_b->nodes, i)) {
            return false;
        }
    }
    return true;
}