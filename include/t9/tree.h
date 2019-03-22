/*!
  ******************************************************************************
  * @file    tree.h
  * @author  Yves-Noel Weweler <y.weweler@fh-muenster.de>
  * @version V1.0.0
  * @brief   Header file for tree.c
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

#ifndef C_T9_TREE_H
#define C_T9_TREE_H

#include "libraries/kvec/kvec.h"

// Forward declarations of trees to break cyclic redundancy.
struct struct_t9_corpus_node_t;
typedef struct struct_t9_corpus_node_t t9_corpus_node_t;

struct struct_t9_search_node_t;
typedef struct struct_t9_search_node_t t9_search_node_t;

#include <string.h>
#include <stdbool.h>

#include "t9/node.h"
#include "t9/model.h"
#include "libraries/list/list.h"

#define PROBABILITY_BUTTON 1.0

/*!
 * Corpus tree. Used build a statistical model of a corpus.
 */
struct struct_t9_corpus_tree_t {
    t9_corpus_node_t *root;
};

typedef struct struct_t9_corpus_tree_t t9_corpus_tree_t;

/*!
 * Search tree. Used to search the best text suggestions based on an user input and a learned statistical model.
 */
struct struct_t9_search_tree_t {
    t9_search_node_t *root;
    kvec_t(list_t *) level_table2;
};

typedef struct struct_t9_search_tree_t t9_search_tree_t;


/* === Corpus tree ================================================================== */

/*!
 * Create a corpus tree.
 * @note The user is responsible for destroying the corpus tree using t9_corpus_tree_destroy once it is no longer required.
 * @return Pointer to a new corpus tree.
 */
t9_corpus_tree_t *
t9_corpus_tree_create(void);

/*!
 * Destroy a corpus tree.
 * @param tree Pointer to a corpus tree to be destroyed.
 */
void
t9_corpus_tree_destroy(t9_corpus_tree_t *const tree);

/*!
 * Calculate the probability of a letter given, that a t9 key was pressed.
 * If the letter is assigned to the key the probability is p. Otherwise 1 - p.
 * @param button Lexicon symbol (T9 key).
 * @param letter Letter to calculate the probability for.
 * @return Probability of letter given that button was pressed.
 */
float
t9_corpus_tree_button_for_letter(t9_symbol_t button,
                                 t9_symbol_t letter);

/*!
 * Calculate the probability of a symbol sequence in the corpus tree.
 * @param tree Pointer to a corpus tree to be searched.
 * @param word Pointer to a string which is to be searched in the tree.
 * @return Probability of the sequence if the sequence was found in the tree. Otherwise 0.0.
 */
float
t9_corpus_tree_conditional_probability(const t9_corpus_tree_t *const tree,
                                       const t9_symbol_t *const word);

/*!
 * Given a corpus insert all possible ngrams of a given length into a corpus tree.
 * @param tree Pointer to a corpus tree to be filled.
 * @param corpus Pointer to a corpus to be used for ngram generation.
 * @param ngram_length Length of the ngrams to be generated.
 * @return T9_SUCCESS if insertion was successful. Otherwise T9_FAILURE.
 */
t9_error_t
t9_corpus_tree_insert_ngrams(t9_corpus_tree_t *const tree,
                             const corpus_t *const corpus,
                             uint16_t ngram_length);

/*!
 * Calculate the probabilities for all tree nodes.
 * @param tree Pointer to a corpus tree to be finalized.
 */
void
t9_corpus_tree_finalize(t9_corpus_tree_t *const tree);

/* ================================================================================== */


/* === Search tree ================================================================== */

/*!
 * Create a search tree.
 * @note The user is responsible for destroying the search tree using t9_search_tree_destroy once it is no longer
 * required.
 * @return Pointer to a new search tree.
 */
t9_search_tree_t *
t9_search_tree_create(void);

/*!
 * Destroy a search tree.
 * @param tree Pointer to a search tree to be destroyed.
 */
void
t9_search_tree_destroy(t9_search_tree_t *const tree);

/*!
 * Type a sequence of keys into the search tree and calculate the best text suggestions for the netered keys.
 * @param model Pointer to a model to be used for searching the best text suggestions.
 * @param sequence Pointer to a lexicon sequence to enter.
 * @return T9_SUCCESS on success. Otherwise T9_FAILURE.
 */
t9_error_t
t9_search_tree_type(t9_model_t *const model,
                    const t9_symbol_t *const sequence);

/*!
 * Type a single symbol into a search tree and update the whole model.
 * This includes searching the best paths and pruning the model.
 * @param model Pointer to a model the key is to be typed into.
 * @param symbol Lexicon symbol to be typed.
 * @return T9_SUCCESS on success. Otherwise T9_FAILURE.
 */
t9_error_t
t9_search_tree_insert(t9_model_t *const model,
                      t9_symbol_t symbol);

/*!
 * Prune a search tree.
 * All nodes that are not an element of the best known paths are pruned.
 * @param model Pointer to a model containing the search tree to be pruned.
 */
void
t9_search_tree_prune(t9_model_t *const model);

/*!
 * Update the list of best paths by searching a search tree.
 * @note The existing list of best paths is overwritten.
 * @param model Pointer to a model containing the search tree to be used for searching.
 */
void
t9_search_tree_search_paths(t9_model_t *const model);

/* ================================================================================== */

#endif //C_T9_TREE_H
