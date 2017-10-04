/*!
  ******************************************************************************
  * @file    model.c
  * @author  Yves-Noel Weweler <y.weweler@fh-muenster.de>
  * @version V1.0.0
  * @brief   This file implements a T9 model.
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

#include "t9/model.h"

t9_model_t *
t9_model_create() {
    t9_model_t *model;

    // Allocate memory.
    model = (t9_model_t *) malloc(sizeof(t9_model_t));
    if (model == NULL) {
        return NULL;
    }

    // Erase memory.
    memset(model, 0, sizeof(t9_model_t));

    // Initialize list of the best paths.
    kv_init(model->paths);

    return model;
}

void
t9_model_destroy(t9_model_t *const model) {
    uint32_t i;

    if (model == NULL) {
        return;
    }

    // Destroy single paths entries.
    for (i = 0; i < kv_size(model->paths); i++) {
        t9_path_destroy(kv_A(model->paths, i));
    }

    // Destroy the vector containing paths.
    kv_destroy(model->paths);

    // Destroy corpus tree.
    if (model->corpus_tree != NULL) {
        t9_corpus_tree_destroy(model->corpus_tree);
    }

    // Destroy search tree.
    if (model->search_tree != NULL) {
        t9_search_tree_destroy(model->search_tree);
    }

    // Unload corpus.
    t9_corpus_unload(&model->corpus);

    // Erase and free the memory.
    memset(model, 0, sizeof(t9_model_t));
    free(model);
}

void
t9_model_sort_paths(t9_model_t *const model) {
    uint32_t i;
    uint32_t j;
    size_t length;
    t9_path_t *tmp;

    if (model == NULL) {
        return;
    }

    if (kv_size(model->paths) == 0) {
        return;
    }

    length = kv_size(model->paths) - 1;
    for (j = 0; j < length; j++) {
        for (i = 0; i < length; i++) {
            // Check if the next path is better.
            if (kv_A(model->paths, i + 1)->probability < kv_A(model->paths, i)->probability) {
                // The next path is better.
                // Therefore swap paths from index (i) and (i + 1).
                tmp = kv_A(model->paths, i);
                kv_A(model->paths, i) = kv_A(model->paths, i + 1);
                kv_A(model->paths, i + 1) = tmp;
            }
        }
    }
}

t9_error_t t9_model_autocomplete(t9_model_t *const model,
                                 const t9_symbol_t *const lexicon_sequence,
                                 t9_symbol_t **suggestion) {
    // Populate the search tree.
    if (t9_search_tree_type(model, lexicon_sequence) == T9_FAILURE) {
        return T9_FAILURE;
    }

    // Extract the best suggested text from the model.
    // Note: model->paths is a list of the completion suggestions with descending scores.
    *suggestion = t9_path_flatten(kv_A(model->paths, 0));
    if (*suggestion == NULL) {
        return T9_FAILURE;
    }

    return T9_SUCCESS;
}

t9_error_t
t9_model_evaluate(t9_model_t *const model,
                  double *const error) {
    t9_symbol_t *lexicon_sequence;
    t9_symbol_t *suggestion;
    size_t diff;
    size_t length;

    // Convert text into lexicon symbols.
    if (t9_corpus_lexicon_from_corpus(model->corpus.test_buffer,
                                      model->corpus.test_buffer_size,
                                      &lexicon_sequence) == T9_FAILURE) {
        return T9_FAILURE;
    }

    // Limit lexicon sequence to the max. allowed length.
    // If the lexicon text is smaller, use the lexicon text length.
    length = strlen((const char *) lexicon_sequence);

    if (t9_model_autocomplete(model, lexicon_sequence, &suggestion) == T9_FAILURE) {
        return T9_FAILURE;
    }

    // Calculate the deviation between the suggestion and the original.
    diff = t9_corpus_sequence_diff(suggestion, model->corpus.test_buffer, length);

    // Calculate the error.
    *error = (double) diff / (double) length;

    printf("[Evaluation]: Suggestion: \"%s\"\n", suggestion);

    free(lexicon_sequence);
    free(suggestion);
    return T9_SUCCESS;
}

t9_error_t
t9_model_prune_path(t9_model_t *const model,
                    t9_path_t *const path) {
    ssize_t i;
    t9_search_node_t *node;

    if (model == NULL || path == NULL) {
        return T9_FAILURE;
    }

    // Prune all nodes contained in the path, starting at the end of the path.
    if (kv_size(path->nodes) > 0) {
        i = kv_size(path->nodes) - 1;
        do {
            // Get a node of the path.
            node = kv_A(path->nodes, i);
            // Prune this node.
            __t9_model_prune_path_helper(model, node, (size_t) i);
            i--;
        } while (i >= 0);
    }
    return T9_SUCCESS;
}

void
__t9_model_prune_path_helper(t9_model_t *const model,
                             t9_search_node_t *const node,
                             size_t depth) {
    list_t *level_map;
    list_node_t *list_node;

    // We can only prune a node if it has no further children.
    if (t9_search_node_is_leaf(node) == true) {
        // Remove node from its parents children.
        list_node = list_find(node->parent->children2, node);
        list_remove(node->parent->children2, list_node);

        // Remove node from the list of nodes for the tree level it resides on.
        level_map = kv_A(model->search_tree->level_table2, depth);
        list_node = list_find(level_map, node);
        list_remove(level_map, list_node);

        // Destroy the node itself.
        t9_search_node_destroy(node);
    }
}