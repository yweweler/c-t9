/*!
  ******************************************************************************
  * @file    model.h
  * @author  Yves-Noel Weweler <y.weweler@fh-muenster.de>
  * @version V1.0.0
  * @brief   Header file for model.c
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

#ifndef C_T9_MODEL_H
#define C_T9_MODEL_H

// Forward declarations of model to break cyclic redundancy.
struct t9_model_struct;
typedef struct t9_model_struct t9_model_t;

#include <stdint.h>
#include "libraries/kvec/kvec.h"

#include "t9/tree.h"
#include "t9/path.h"

/*!
 * T9 model.
 */
struct t9_model_struct {
    corpus_t corpus;
    t9_corpus_tree_t *corpus_tree;
    t9_search_tree_t *search_tree;
    t9_path_vector_t paths;
    uint8_t ngram_length;
    uint16_t number_paths;
};

typedef struct t9_model_struct t9_model_t;


/*!
 * Create a model.
 * @note The user is responsible for destroying the model using t9_model_destroy once it is no longer required.
 * @return Pointer to a new model.
 */
t9_model_t *
t9_model_create();

/*!
 * Destroy an existing model. All contained data is destroyed automatically.
 * @param model Pointer to a model that is to be destroyed.
 */
void
t9_model_destroy(t9_model_t *const model);

/*!
 * Sort the list of best paths ascending, so that the best path is the first entry.
 * @param model Pointer to a model whose paths are to be sorted.
 */
void
t9_model_sort_paths(t9_model_t *const model);

/*!
 * Autocomplete a given symbol sequence as text based on the statistical model.
 * @param model Pointer to the model to be used for completion.
 * @param lexicon_sequence Pointer to a lexicon sequence to enter.
 * @param suggestion
 * @return T9_SUCCESS on success, otherwise T9_FAILURE.
 */
t9_error_t t9_model_autocomplete(t9_model_t *const model,
                                 const t9_symbol_t *const lexicon_sequence,
                                 t9_symbol_t ** suggestion);

/*!
 * Evaluate a model by inserting text from an test set and comparing it to the original.
 * @param model Pointer to the model to be evaluated.
 * @param error Pointer to a variable where the pointer to the resulting string is placed.
 * @return T9_SUCCESS on success, otherwise T9_FAILURE.
 */
t9_error_t
t9_model_evaluate(t9_model_t *const model,
                  double *const error);

/*!
 * Prune all nodes in a models search tree a given path consists of.
 * @param model Pointer toa model that is to be pruned.
 * @param path Pointer to a path that is to prune the search tree.
 * @return T9_SUCCESS on success, otherwise T9_FAILURE.
 */
t9_error_t
t9_model_prune_path(t9_model_t *const model,
                    t9_path_t *const path);

/*!
 * Helper function used to remove a search node from a model.
 * @param model Pointer to a model from which a node is to me removed.
 * @param node Pointer to a node that is to be removed.
 * @param depth Level of the tree, the node to be removed is on.
 */
void __t9_model_prune_path_helper(t9_model_t *const model,
                                  t9_search_node_t *const node,
                                  size_t depth);

#endif //C_T9_MODEL_H
