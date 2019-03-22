/*!
  ******************************************************************************
  * @file    path.h
  * @author  Yves-Noel Weweler <y.weweler@fh-muenster.de>
  * @version V1.0.0
  * @brief   Header file for path.c
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

#ifndef C_T9_PATH_H
#define C_T9_PATH_H

#define kvec_path_t(type) struct struct_kvec_path {size_t n, m; type *a; }

#include "libraries/kvec/kvec.h"

// Forward declarations of path to break cyclic redundancy.
struct struct_t9_path_t;
typedef struct struct_t9_path_t t9_path_t;

typedef kvec_path_t(t9_path_t *) t9_path_vector_t;

#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include "t9/tree.h"
#include "t9/node.h"

/*!
 * Path structure, describing a path trough a search tree.
 */
struct struct_t9_path_t {
    float probability;
    t9_search_node_vector_t nodes;
};

typedef struct struct_t9_path_t t9_path_t;

/*!
 * Create a path.
 * @note The user is responsible for destroying the path using t9_path_destroy once it is no longer required.
 * @return Pointer to the newly created path.
 */
t9_path_t *
t9_path_create();

/*!
 * Destroy a path.
 * @note The nodes contained in a path are not destroyed.
 * @param path
 */
void
t9_path_destroy(t9_path_t *const path);

/*!
 * Append a search node to a path.
 * @param path Pointer to a path to be modified.
 * @param node Pointer to a note as is to be appended.
 */
void
t9_path_push(t9_path_t *const path, t9_search_node_t *const node);

/*!
 * Remove the last node of a path.
 * @note The paths probability will reset to -1.0 until a new node is added again.
 * @param path Pointer to a path to be modified.
 * @return Pointer to the node that was removed from the path.
 */
t9_search_node_t *
t9_path_pop(t9_path_t *const path);

/*!
 * Duplicate a path by creating a new path and coping the all contents.
 * @note The user is responsible for destroying the duplicated path using t9_path_destroy once it is no longer required.
 * @param path Pointer to a path to be duplicated.
 * @return Pointer to the new path. NULL if an error occurred.
 */
t9_path_t *
t9_path_duplicate(const t9_path_t *const path);

/*!
 * Create a symbol string from a path.
 * This string represent the path taken trough the search tree.
 * @note The user is responsible for destroying the string using free once it is no longer required.
 * @param path Pointer to a path a string should be created from.
 * @return Pointer to a new string. NULL if an error occurred.
 */
t9_symbol_t *
t9_path_flatten(const t9_path_t *const path);

/*!
 * Create a string from path that includes the symbols the path passes trough its nodes and the overall path
 * probability. The form is "sssss...sss" : probability.
 * @note The user is responsible for destroying the string using free once it is no longer required.
 * @param path Pointer to a path a string should be created from.
 * @return Pointer to a new string. NULL if an error occurred.
 */
char *
t9_path_str(const t9_path_t *const path);

/*!
 * Check if two paths are equal by comparing the nodes there are made of.
 * @note Both paths have to have the same length.
 * @note The probability itself is not used for the equality check.
 * @param path_a Pointer to the first path.
 * @param path_b Pointer to the second path.
 * @return true if the paths are equal, false otherwise.
 */
bool
t9_path_is_equal(const t9_path_t *const path_a, const t9_path_t *const path_b);

#endif //C_T9_PATH_H
