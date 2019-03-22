/*!
  ******************************************************************************
  * @file    node.h
  * @author  Yves-Noel Weweler <y.weweler@fh-muenster.de>
  * @version V1.0.0
  * @brief   Header file for node.c
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

#ifndef C_T9_NODE_H
#define C_T9_NODE_H

#define kvec_cnode_t(type) struct struct_kvec_cnode {size_t n, m; type *a; }
#define kvec_snode_t(type) struct struct_kvec_snode {size_t n, m; type *a; }

#include "libraries/kvec/kvec.h"

// Forward declarations of nodes to break cyclic redundancy.
struct struct_t9_corpus_tree_t;
typedef struct struct_t9_corpus_tree_t t9_corpus_tree_t;

struct struct_t9_search_tree_t;
typedef struct struct_t9_search_tree_t t9_search_tree_t;

struct struct_t9_corpus_node_t;
typedef struct struct_t9_corpus_node_t t9_corpus_node_t;

struct struct_t9_search_node_t;
typedef struct struct_t9_search_node_t t9_search_node_t;

typedef kvec_cnode_t(t9_corpus_node_t *) t9_corpus_node_vector_t;
typedef kvec_snode_t(t9_search_node_t *) t9_search_node_vector_t;

#include <stdbool.h>
#include "libraries/list/list.h"

#include "t9/corpus.h"
#include "t9/math.h"
#include "t9/tree.h"
#include "t9/model.h"
#include "t9/path.h"

/*!
 * Node structure used in a corpus tree.
 */
struct struct_t9_corpus_node_t {
    uint64_t count;
    t9_symbol_t symbol;
    float probability;
    struct struct_t9_corpus_node_t *parent;
    t9_corpus_node_vector_t children;
};

typedef struct struct_t9_corpus_node_t t9_corpus_node_t;

/*!
 * Node structure used in a search tree.
 */
struct struct_t9_search_node_t {
    t9_symbol_t symbol;
    float probability;
    struct struct_t9_search_node_t *parent;
    list_t *children2;
};

typedef struct struct_t9_search_node_t t9_search_node_t;

/* === Corpus node ================================================================== */

/*!
 * Create a corpus node.
 * @note The user is responsible for destroying the corpus node using t9_corpus_node_destroy once it is no longer required.
 * @return Pointer to a new corpus node.
 */
t9_corpus_node_t *
t9_corpus_node_create();

/*!
 * Destroy a corpus node.
 * @param node Pointer to a corpus node to be destroyed.
 */
void
t9_corpus_node_destroy(t9_corpus_node_t *const node);

/*!
 * Calculate probability of a node and it's children.
 * @param node Pointer to a corpus node.
 */
void
t9_corpus_node_finalize(t9_corpus_node_t *const node);

/*!
 * Search a node with a given symbol within the children of a node.
 * @param parent Pointer to a corpus node whose children are to be searched.
 * @param symbol Symbol to look for in the children.
 * @return Pointer to a corpus node if a child was found. If no child was found NULL is returned.
 */
t9_corpus_node_t *
t9_corpus_node_get_child(const t9_corpus_node_t *const parent,
                         t9_symbol_t symbol);

/*!
 * Search a node with a given symbol within the children of a node.
 * If no child is found, a new child with the searched symbol is created.
 * @param parent Pointer to a corpus node whose children are to be searched.
 * @param symbol Symbol to look for in the children.
 * @return Pointer to a corpus node.
 */
t9_corpus_node_t *
t9_corpus_node_get_child_safe(t9_corpus_node_t *const parent,
                              t9_symbol_t symbol);

/*!
 * Insert a ngram into a corpus tree starting at a given node.
 * @param node Pointer to a corpus node from which the ngram is to be inserted.
 * @param ngram Pointer to a string to be inserted.
 */
void
t9_corpus_node_insert_ngram(t9_corpus_node_t *const node,
                            const t9_symbol_t *const ngram);

/*!
 * Calculate the probability of a symbol sequence starting at a given node.
 * @param node Pointer to a corpus node whose children are to be searched.
 * @param word Pointer to a string whose probability should be calculated.
 * @return Probability of the word.
 */
float
t9_corpus_node_conditional_probability(const t9_corpus_node_t *const node,
                                       const t9_symbol_t *const word);

/*!
 * Add a child to a corpus node.
 * @param node Pointer to a corpus node to which a child should be added.
 * @param child Pointer to a child which is to be added.
 */
void
t9_corpus_node_add_child(t9_corpus_node_t *const node,
                         t9_corpus_node_t *const child);

/* ================================================================================== */


/* === Search tree ================================================================== */

/*!
 * Create a search node.
 * @note The user is responsible for destroying the search node using t9_search_node_destroy once it is no longer
 * required.
 * @return Pointer to a new search node.
 */
t9_search_node_t *
t9_search_node_create();

/*!
 * Destroy a search node.
 * @param node Pointer to a search node to be destroyed.
 */
void
t9_search_node_destroy(t9_search_node_t *const node);

/*!
 * Check if a search node is a leaf node.
 * @param node Pointer to a search node that is to be checked.
 * @return true if node is a leaf, false otherwise.
 */
bool
t9_search_node_is_leaf(const t9_search_node_t *const node);

t9_error_t
t9_search_node_insert(t9_search_node_t *const node,
                      t9_symbol_t t9_input,
                      const t9_symbol_t *sequence,
                      const uint32_t depth,
                      t9_model_t *const model);

/*!
 * Add a child to a search node.
 * @param node Pointer to a search node to which a child should be added.
 * @param child Pointer to a child which is to be added.
 */
void
t9_search_node_add_child(t9_search_node_t *const node,
                         t9_search_node_t *const child);

/*!
 * Populate the list of best paths in a given model, starting with a given node.
 * @param node Pointer to a search node to start the path search at.
 * @param model Pointer to a model in which to search and store the best paths.
 * @param tmp_path Pointer to a path that was taken to reach the given node. This path is modified as the tree is
 * searched.
 */
void
t9_node_search_paths(t9_search_node_t *const node,
                     t9_model_t *const model,
                     t9_path_t *tmp_path);

/*!
 * Prune a a given node (and all its children) that was reached over a given path.
 * All nodes that are not part of the paths that are known to be the best paths of the given model are pruned.
 * @param node Pointer to a search node to be pruned.
 * @param model Pointer to a model that contains the search node to be pruned.
 * @param path Pointer to a path that was taken to reach the node to be pruned.
 */
void
t9_search_node_prune(t9_search_node_t *const node,
                     t9_model_t *const model,
                     t9_path_t *const path);

/*!
 * Search a node with a given symbol within the children of a node.
 * @param parent Pointer to a search node whose children are to be searched.
 * @param symbol Symbol to look for in the children.
 * @return Pointer to a search node if a child was found. If no child was found NULL is returned.
 */
t9_search_node_t *
t9_search_node_get_child(const t9_search_node_t *const parent,
                         t9_symbol_t symbol);

/*!
 * Descend down a the search tree starting at a given node with given a symbol sequence and return the last search
 * node encountered.
 * @param node Pointer to a search node to begin the descend with.
 * @param sequence Pointer to a string that is do be followed through the search tree.
 * @return Pointer to the last node of the followed sequence. NULL if the sequence is not contained in the tree.
 */
t9_search_node_t *
t9_search_node_descend(const t9_search_node_t *const node,
                       const t9_symbol_t *const sequence);

/* ================================================================================== */


#endif //C_T9_NODE_H
