/*!
  ******************************************************************************
  * @file    tree.c
  * @author  Yves-Noel Weweler <y.weweler@fh-muenster.de>
  * @version V1.0.0
  * @brief   This file implements corpus trees and search trees.
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

#include <list/list.h>
#include "t9/tree.h"

/* === Corpus tree ================================================================== */

t9_corpus_tree_t *
t9_corpus_tree_create() {
    t9_corpus_tree_t *tree;

    // Allocate memory.
    tree = (t9_corpus_tree_t *) malloc(sizeof(t9_corpus_tree_t));
    if (tree == NULL) {
        return NULL;
    }

    // Erase memory.
    memset(tree, 0, sizeof(t9_corpus_tree_t));

    // Create a root node.
    tree->root = t9_corpus_node_create();
    tree->root->symbol = ' ';

    return tree;
}

void
t9_corpus_tree_destroy(t9_corpus_tree_t *const tree) {
    if (tree == NULL) {
        return;
    }

    // Destroy the root node.
    if (tree->root != NULL) {
        t9_corpus_node_destroy(tree->root);
    }

    // Erase and free the memory.
    memset(tree, 0, sizeof(t9_corpus_tree_t));
    free(tree);
}

float
t9_corpus_tree_button_for_letter(t9_symbol_t button,
                                 t9_symbol_t letter) {
    float prob;
    const t9_symbol_t *lexicon;

    prob = PROBABILITY_BUTTON;
    lexicon = t9_corpus_ltoc(button);
    // Iterate corpus symbols assigned to the button.
    while (*lexicon != 0) {
        if (*lexicon == letter) {
            // Letter is assigned to the button.
            return prob;
        }
        lexicon++;
    }
    // Letter is not assigned to the button.
    return 1.0f - prob;
}

float
t9_corpus_tree_conditional_probability(const t9_corpus_tree_t *const tree,
                                       const t9_symbol_t *const word) {
    if (tree == NULL || word == NULL) {
        return 0.0;
    }

    if (tree->root == NULL) {
        return 0.0;
    }

    // Find probability of the sequence in tree.
    return t9_corpus_node_conditional_probability(tree->root, word);
}

t9_error_t
t9_corpus_tree_insert_ngrams(t9_corpus_tree_t *const tree,
                             const corpus_t *const corpus,
                             uint16_t ngram_length) {
    t9_symbol_t *ngram = NULL;
    t9_symbol_t *ngram_buffer;
    size_t offset;

    // Prepare a buffer for a single ngram.
    ngram_buffer = (t9_symbol_t *) malloc(ngram_length + 1);
    if (ngram_buffer == NULL) {
        return T9_FAILURE;
    }
    // Erase buffer.
    memset(ngram_buffer, 0, ngram_length + 1);
    ngram = ngram_buffer;

    offset = 0;
    // Create the first ngram.
    t9_corpus_ngram(corpus, ngram, ngram_length, &offset);
    do {
        // Insert ngram into tree.
        t9_corpus_node_insert_ngram(tree->root, ngram);
        // Create a new ngram.
        t9_corpus_ngram(corpus, ngram, ngram_length, &offset);
    } while (offset != corpus->train_buffer_size);

    free(ngram_buffer);
    return T9_SUCCESS;
}

void
t9_corpus_tree_finalize(t9_corpus_tree_t *const tree) {
    uint32_t i;
    t9_corpus_node_t *child;
    t9_corpus_node_t *root;

    if (tree == NULL) {
        return;
    }

    root = tree->root;
    if (root == NULL) {
        return;
    }

    // Count children of root node.
    for (i = 0; i < kv_size(root->children); i++) {
        child = kv_A(root->children, i);
        root->count += child->count;
    }

    // Root node does not have a probability.
    root->probability = 0.0;

    // Recursively calculate node probabilities through the tree.
    t9_corpus_node_finalize(root);
}

/* ================================================================================== */


/* === Search tree ================================================================== */

t9_search_tree_t *
t9_search_tree_create() {
    t9_search_tree_t *tree;

    // Allocate memory.
    tree = (t9_search_tree_t *) malloc(sizeof(t9_search_tree_t));
    if (tree == NULL) {
        return NULL;
    }

    // Erase memory.
    memset(tree, 0, sizeof(t9_search_tree_t));

    // Initialize root node.
    tree->root = t9_search_node_create();
    tree->root->symbol = ' ';

    // Initialize level table.
    kv_init(tree->level_table2);

    return tree;
}

void
t9_search_tree_destroy(t9_search_tree_t *const tree) {
    size_t i;

    if (tree == NULL) {
        return;
    }

    // Destroy all tree nodes.
    if (tree->root != NULL) {
        t9_search_node_destroy(tree->root);
    }

    // Delete level table entries.
    for (i = 0; i < kv_size(tree->level_table2); i++) {
        list_destroy(kv_A(tree->level_table2, i));
    }

    // Destroy level table.
    kv_destroy(tree->level_table2);

    // Erase and free memory.
    memset(tree, 0, sizeof(t9_search_tree_t));
    free(tree);
}

t9_error_t
t9_search_tree_type(t9_model_t *const model,
                    const t9_symbol_t *const sequence) {
    size_t i;

    const t9_symbol_t *symbol;
    list_t *level_map_entry;

    if (model == NULL) {
        return T9_FAILURE;
    }

    if (model->search_tree == NULL || sequence == NULL) {
        return T9_FAILURE;
    }

    // Validate that the sequence to be inserted only contains valid lexicon symbols.
    if (t9_corpus_validate_lexicon_symbols(sequence) == false) {
        return T9_FAILURE;
    }

    symbol = sequence;
    i = 0;
    while (*symbol != 0) {
        // Add a new search tree table entry for the new level.
        level_map_entry = list_new();
        kv_push(list_t *, model->search_tree->level_table2, level_map_entry);
        // Type symbol.
        if (t9_search_tree_insert(model, *symbol) != T9_SUCCESS) {
            return T9_FAILURE;
        }
        symbol++;
        i++;
    }
    return T9_SUCCESS;
}

t9_error_t
t9_search_tree_insert(t9_model_t *const model,
                      t9_symbol_t symbol) {
    t9_error_t error;

    error = t9_search_node_insert(model->search_tree->root, symbol, (const t9_symbol_t *const) "", 0, model);
    if (error != T9_SUCCESS) {
        return T9_FAILURE;
    }
    t9_search_tree_search_paths(model);
    t9_search_tree_prune(model);

    return T9_SUCCESS;
}

void
t9_search_tree_prune(t9_model_t *const model) {
    t9_path_t *path;
    size_t tree_depth;

    if (model->ngram_length < 1) {
        // No need for pruning.
        return;
    }

    tree_depth = kv_size(model->search_tree->level_table2);
    if (tree_depth < model->ngram_length) {
        // Wait until the tree is as deep as the ngram length before pruning.
        return;
    }

    path = t9_path_create();
    // Prune the tree starting at the root node.
    t9_search_node_prune(model->search_tree->root, model, path);
    t9_path_destroy(path);
}

void
t9_search_tree_search_paths(t9_model_t *const model) {
    size_t i;
    t9_path_t *tmp_path;

    // Delete existing paths.
    for (i = 0; i < kv_size(model->paths); i++) {
        t9_path_destroy(kv_A(model->paths, i));
    }
    // Reinitialize list of best paths.
    kv_destroy(model->paths);
    kv_init(model->paths);

    tmp_path = t9_path_create();
    // Search best path in the search tree.
    t9_node_search_paths(model->search_tree->root, model, tmp_path);
    t9_path_destroy(tmp_path);
}