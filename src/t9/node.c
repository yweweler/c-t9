/*!
  ******************************************************************************
  * @file    node.c
  * @author  Yves-Noel Weweler <y.weweler@fh-muenster.de>
  * @version V1.0.0
  * @brief   This file implements T9 corpus nodes and search nodes that are
  *          used to build trees.
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

#include "t9/node.h"

/* === Corpus node ================================================================== */

t9_corpus_node_t *
t9_corpus_node_create() {
    t9_corpus_node_t *node;

    // Allocate memory.
    node = (t9_corpus_node_t *) malloc(sizeof(t9_corpus_node_t));
    if (node == NULL) {
        return NULL;
    }

    // Erase memory.
    memset(node, 0, sizeof(t9_corpus_node_t));

    // Initialize list of children.
    kv_init(node->children);

    return node;
}

void
t9_corpus_node_destroy(t9_corpus_node_t *const node) {
    uint32_t i;

    if (node == NULL) {
        return;
    }

    // Recursively delete children.
    if (kv_size(node->children) > 0) {
        for (i = 0; i < kv_size(node->children); i++) {
            t9_corpus_node_destroy(kv_A(node->children, i));
        }
    }
    // Destroy the children list.
    kv_destroy(node->children);

    // Erase and free memory.
    memset(node, 0, sizeof(t9_corpus_node_t));
    free(node);
}

void
t9_corpus_node_finalize(t9_corpus_node_t *const node) {
    uint32_t i;
    t9_corpus_node_t *child;

    // Recursively calculate node probability.
    for (i = 0; i < kv_size(node->children); i++) {
        child = kv_A(node->children, i);
        child->probability = (float) child->count / (float) node->count;
        t9_corpus_node_finalize(child);
    }
}

t9_corpus_node_t *
t9_corpus_node_get_child(const t9_corpus_node_t *const parent,
                         t9_symbol_t symbol) {
    uint32_t i;
    t9_corpus_node_t *child;

    // Search child.
    for (i = 0; i < kv_size(parent->children); i++) {
        child = kv_A(parent->children, i);
        if (child->symbol == symbol) {
            // Child was found.
            return child;
        }
    }
    // Child was not found.
    return NULL;
}

t9_corpus_node_t *
t9_corpus_node_get_child_safe(t9_corpus_node_t *const parent,
                              t9_symbol_t symbol) {
    t9_corpus_node_t *child;

    // Look for child with the desired symbol.
    child = t9_corpus_node_get_child(parent, symbol);
    if (child == NULL) {
        // No child could be found, therefore create and insert a new one.
        child = t9_corpus_node_create();
        child->symbol = symbol;
        child->parent = parent;
        t9_corpus_node_add_child(parent, child);
    }

    return child;
}

void
t9_corpus_node_insert_ngram(t9_corpus_node_t *const node,
                            const t9_symbol_t *const ngram) {
    t9_corpus_node_t *child;

    if (*ngram == 0) {
        return;
    }

    child = t9_corpus_node_get_child_safe(node, ngram[0]);
    child->count++;
    t9_corpus_node_insert_ngram(child, ngram + 1);
}

float
t9_corpus_node_conditional_probability(const t9_corpus_node_t *const node,
                                       const t9_symbol_t *const word) {
    uint32_t i;
    t9_corpus_node_t *child;

    for (i = 0; i < kv_size(node->children); i++) {
        child = kv_A(node->children, i);
        // Check if there is a child with a symbol corresponding to the first symbol of the word.
        if (child->symbol == word[0]) {
            // Matching child was found.
            if (word[1] == 0) {
                // End of word reached, return probability.
                return child->probability;
            } else {
                // Find child for the next character of the word.
                return t9_corpus_node_conditional_probability(child, (word + 1));
            }
        }
    }
    // Word is not in tree.
    return 0.0;
}

void
t9_corpus_node_add_child(t9_corpus_node_t *const node,
                         t9_corpus_node_t *const child) {
    kv_push(t9_corpus_node_t *, node->children, child);
}

/* ================================================================================== */

/* === Search tree ================================================================== */

t9_search_node_t *
t9_search_node_create() {
    t9_search_node_t *node;

    // Allocate memory.
    node = (t9_search_node_t *) malloc(sizeof(t9_search_node_t));
    if (node == NULL) {
        return NULL;
    }

    // Erase memory.
    memset(node, 0, sizeof(t9_search_node_t));

    // Initialize list of children.
    node->children2 = list_new();

    return node;
}

void
t9_search_node_destroy(t9_search_node_t *const node) {
    list_iterator_t *iter;
    list_node_t *list_node;
    t9_search_node_t *child;

    if (node == NULL) {
        return;
    }

    // Recursively destroy all children.
    iter = list_iterator_new(node->children2, LIST_HEAD);
    while ((list_node = list_iterator_next(iter)) != NULL) {
        child = list_node_data(list_node);
        t9_search_node_destroy(child);
    }
    list_iterator_destroy(iter);

    // Destroy the children list.
    list_destroy(node->children2);

    // Erase and free memory.
    memset(node, 0, sizeof(t9_search_node_t));
    free(node);
}

bool t9_search_node_is_leaf(const t9_search_node_t *const node) {
    if (list_size(node->children2) == 0) {
        return true;
    } else {
        return false;
    }
}

t9_error_t
t9_search_node_insert(t9_search_node_t *const node,
                      t9_symbol_t t9_input,
                      const t9_symbol_t *sequence,
                      const uint32_t depth,
                      t9_model_t *const model) {

    list_iterator_t *iter;
    list_node_t *list_node;
    t9_search_node_t *child;
    const t9_symbol_t *symbol;
    float prob_t_b;
    float prob_b_bb;
    size_t sequence_length;
    t9_symbol_t *sequence_ptr;
    t9_symbol_t word[model->ngram_length + sizeof(t9_symbol_t) + 1];

    if (node == NULL || sequence == NULL) {
        return T9_FAILURE;
    }

    if (t9_search_node_is_leaf(node)) {
        // Append a child for each corpus symbols to the leaf node.
        symbol = (const t9_symbol_t *) CORPUS_SYMBOLS;
        while (*symbol != 0) {
            // Create a new child.
            child = t9_search_node_create();
            if (child == NULL) {
                return T9_FAILURE;
            }

            sequence_length = strlen((const char *) sequence);
            if (sequence_length >= (size_t) (model->ngram_length - 1)) {
                sequence += (sequence_length - (model->ngram_length - 1));
            }
            if (snprintf((char *) word, sizeof(word), "%s%c", sequence, *symbol) < 0) {
                t9_search_node_destroy(child);
                return T9_FAILURE;
            }

            // Calculate child probability.
            prob_t_b = -t9_ln(t9_corpus_tree_button_for_letter(t9_input, *symbol));
            prob_b_bb = -t9_ln(t9_corpus_tree_conditional_probability(model->corpus_tree, word));
            child->probability = prob_t_b + prob_b_bb + node->probability;

            // Set child symbol and parent.
            child->symbol = *symbol;
            child->parent = node;

            // Add child to parent.
            t9_search_node_add_child(node, child);

            // Add the new child to the list of nodes that are on the same tree depth.
            list_rpush(kv_A(model->search_tree->level_table2, depth), list_node_new(child));

            symbol++;
        }
    } else {
        // Descend the tree until a leaf node.
        iter = list_iterator_new(node->children2, LIST_HEAD);
        while ((list_node = list_iterator_next(iter)) != NULL) {
            child = list_node_data(list_node);
            if (asprintf((char **)&sequence_ptr, "%s%c", sequence, child->symbol) == -1) {
                list_iterator_destroy(iter);
                return T9_FAILURE;
            }
            if (t9_search_node_insert(child, t9_input, sequence_ptr, depth + 1, model) != T9_SUCCESS) {
                free(sequence_ptr);
                list_iterator_destroy(iter);
                return T9_FAILURE;
            }
            free(sequence_ptr);
            sequence_ptr = NULL;
        }
        list_iterator_destroy(iter);
    }
    return T9_SUCCESS;
}

void
t9_search_node_add_child(t9_search_node_t *const node,
                         t9_search_node_t *const child) {
    list_rpush(node->children2, list_node_new(child));
}

void
t9_node_search_paths(t9_search_node_t *const node,
                     t9_model_t *const model,
                     t9_path_t *tmp_path) {
    t9_search_node_t *child;
    t9_path_t *candidate;

    list_iterator_t *iter;
    list_node_t *list_node;

    if (kv_size(model->paths) > 0) {
        if (kv_size(model->paths) >= model->number_paths) {
            // Maximal number of paths to search was reached.
            if (node->probability >= kv_last(model->paths)->probability) {
                // Current path is not better than the worst path in the list of known best paths.
                // Skip this path.
                return;
            }
        }
    }

    if (t9_search_node_is_leaf(node)) {
        // Leaf node was hit, the taken path therefore spans the whole tree depth.

        // Copy path.
        candidate = t9_path_duplicate(tmp_path);
        // Append path to the list of best paths.
        kv_push(t9_path_t *, model->paths, candidate);
        // Sort list of best paths.
        t9_model_sort_paths(model);

        // In case there are now more best paths than requested by the model delete the worst path.
        if (kv_size(model->paths) > model->number_paths) {
            candidate = kv_pop(model->paths);
            t9_path_destroy(candidate);
        }
    } else {
        // Descend tree until a leaf node is hit.
        iter = list_iterator_new(node->children2, LIST_HEAD);
        while ((list_node = list_iterator_next(iter)) != NULL) {
            child = list_node_data(list_node);
            // Descend down separate a path for each child.
            t9_path_push(tmp_path, child);
            t9_node_search_paths(child, model, tmp_path);
            t9_path_pop(tmp_path);
        }
        list_iterator_destroy(iter);
    }
}

void
t9_search_node_prune(t9_search_node_t *const node,
                     t9_model_t *const model,
                     t9_path_t *const path) {
    list_iterator_t *iter;
    list_node_t *list_node;
    t9_search_node_t *child;
    t9_path_t *best_path;
    size_t i;
    bool found;

    if (t9_search_node_is_leaf(node)) {
        // End of tree was reached.
        // Check if the path that was taken to reach this node is one of the best known paths.
        found = false;
        for (i = 0; i < kv_size(model->paths); i++) {
            best_path = kv_A(model->paths, i);
            if (t9_path_is_equal(path, best_path) == true) {
                // Taken path is one of the best paths. (No pruning)
                found = true;
                break;
            }
        }
        if (found == false) {
            // Path is not known to be one of the bst paths.
            // Prune this path.
            t9_model_prune_path(model, path);
        }
    } else {
        // Iterate all children of the node.
        iter = list_iterator_new(node->children2, LIST_HEAD);
        while ((list_node = list_iterator_next(iter)) != NULL) {
            child = list_node_data(list_node);
            // Descend down a path for each child.
            t9_path_push(path, child);
            t9_search_node_prune(child, model, path);
            t9_path_pop(path);
        }
        list_iterator_destroy(iter);
    }
}

t9_search_node_t *
t9_search_node_get_child(const t9_search_node_t *const parent,
                         t9_symbol_t symbol) {
    list_iterator_t *iter;
    list_node_t *node;
    t9_search_node_t *child;

    // Search child.
    iter = list_iterator_new(parent->children2, LIST_HEAD);
    while ((node = list_iterator_next(iter)) != NULL) {
        child = list_node_data(node);
        if (child->symbol == symbol) {
            // Child was found.
            list_iterator_destroy(iter);
            return child;
        }
    }

    // Child could not be found.
    list_iterator_destroy(iter);
    return NULL;
}

t9_search_node_t *
t9_search_node_descend(const t9_search_node_t *const node,
                       const t9_symbol_t *const sequence) {
    t9_search_node_t *child;

    if (node == NULL || sequence == NULL) {
        return NULL;
    }

    child = t9_search_node_get_child(node, *sequence);
    if (child == NULL) {
        return NULL;
    }

    if (*(sequence + 1) == 0) {
        return child;
    }

    return t9_search_node_descend(child, sequence + 1);
}

/* ================================================================================== */