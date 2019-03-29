/*!
  ******************************************************************************
  * @file    main.h
  * @author  Yves-Noel Weweler <y.weweler@fh-muenster.de>
  * @version V1.0.0
  * @brief   Header file for main.c
  ******************************************************************************
  * @attention
  *
  * MIT License
  *
  * Copyright (c) 2019 Yves-Noel Weweler
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

#ifndef C_T9_MAIN_H
#define C_T9_MAIN_H

#include <stdlib.h>
#include <stdio.h>

#include "t9/config.h"
#include "t9/corpus.h"
#include "t9/tree.h"
#include "t9/timer.h"


void
build_corpus_tree(t9_model_t *const model);

/*!
 * Example:
 * Evaluate the given statistical model by generating completing a known symbol sequence and comparing the deviation
 * between the generated sequence and the ground truth sequence.
 * @param model Pointer to the model to be evaluated.
 */
void
example_evaluation(t9_model_t *const model);

/*!
 * Example:
 * Autocomplete a given input sequence based on the statistical model.
 * @param model Pointer to the model to be used for completion.
 */
void
example_autocomplete(t9_model_t *const model, const char * text);

#endif //C_T9_MAIN_H
