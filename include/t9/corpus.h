/*!
  ******************************************************************************
  * @file    corpus.h
  * @author  Yves-Noel Weweler <y.weweler@fh-muenster.de>
  * @version V1.0.0
  * @brief   Header file for corpus.c
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

#ifndef C_T9_CORPUS_H
#define C_T9_CORPUS_H

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

typedef uint8_t t9_symbol_t;

#include "t9/errno.h"
#include "t9/io.h"

// List of corpus symbols assigned to single T9 keys.
#define SYMBOLS_T0  "0"
#define SYMBOLS_T1  ".,1"
#define SYMBOLS_T2  "aAbBcC2"
#define SYMBOLS_T3  "dDeEfF3"
#define SYMBOLS_T4  "gGhHiI4"
#define SYMBOLS_T5  "jJkKlL5"
#define SYMBOLS_T6  "mMnNoO6"
#define SYMBOLS_T7  "pPqQrRsS7"
#define SYMBOLS_T8  "tTuUvV8"
#define SYMBOLS_T9  "wWxXyYzZ9"
#define SYMBOLS_TS  ""
#define SYMBOLS_TR  " "

// List of existing T9 keys.
#define LEXICON_SYMBOLS     "0123456789*#"

// Complete list of corpus symbols.
#define CORPUS_SYMBOLS      SYMBOLS_T0 SYMBOLS_T1 SYMBOLS_T2 \
                            SYMBOLS_T3 SYMBOLS_T4 SYMBOLS_T5 \
                            SYMBOLS_T6 SYMBOLS_T7 SYMBOLS_T8 \
                            SYMBOLS_T9 SYMBOLS_TS SYMBOLS_TR

// Number of lexicon symbols (T9 keys).
#define NUM_LEXICON_SYMBOLS 12

// Number of corpus symbols.
#define NUM_CORPUS_SYMBOLS  1 + 3 + 7 + 7 + 7 + 7 + 7 + 9 + 7 + 9 + 1


/*!
 * Structure storing corpus text data.
 */
struct struct_t9_corpus_t {
    t9_symbol_t *train_buffer;
    size_t train_buffer_size;
    t9_symbol_t *test_buffer;
    size_t test_buffer_size;
};

typedef struct struct_t9_corpus_t corpus_t;

/*!
 * Load corpus text from a file to a corpus structure..
 * @param train_path Path of a file containing the train corpus data to be read.
 * @param train_limit Maximal number of bytes to be read from the train file. 0 indicates, that the whole file should
 * be loaded.
 * @param test_path Path of a file containing the test corpus data to be read.
 * @param test_limit Maximal number of bytes to be read from the test file. 0 indicates, that the whole file should
 * be loaded.
 * @param corpus Pointer to a corpus_t structure that is to be filled.
 * @return T9_SUCCESS on success, otherwise T9_FAILURE.
 */
t9_error_t
t9_corpus_load(const char *train_path,
               size_t train_limit,
               const char *test_path,
               size_t test_limit,
               corpus_t *corpus);

/*!
 * Unload a corpus from memory.
 * @param corpus Pointer to a corpus_t structure to be unloaded.
 * @return t9_error_t T9_SUCCESS on success, otherwise T9_FAILURE.
 */
t9_error_t
t9_corpus_unload(corpus_t *corpus);

/*!
 * Get a string containing all corpus symbols that are assigned to a lexicon symbol.
 * @param symbol Lexicon symbol.
 * @return Pointer to a constant string. NULL if the lexicon symbol is unknown.
 */
const t9_symbol_t *
t9_corpus_ltoc(t9_symbol_t symbol);

/*!
 * Convert a corpus symbol to the lexicon symbol (T9 key) that it corresponds to.
 * @param symbol Corpus symbol to be converted.
 * @param out Pointer to a variable were the lexicon symbol should be stored.
 * @return T9_SUCCESS on success, T9_FAILURE if the given symbol is not assigned to a lexicon symbol.
 */
t9_error_t
t9_corpus_ctol(t9_symbol_t symbol, t9_symbol_t *out);

/*!
 * Check if a given symbols is a valid lexicon symbol.
 * @param symbol Symbol to be checked.
 * @return true if symbols is valid, false otherwise.
 */
bool
t9_corpus_validate_lexicon_symbol(t9_symbol_t symbol);

/*!
 * Check if a symbol sequence only contains valid lexicon symbols.
 * @param symbols Pointer to a string that is to be checked.
 * @return true if all symbols are is valid, false otherwise.
 */
bool
t9_corpus_validate_lexicon_symbols(const t9_symbol_t *const symbols);

/*!
 * Generate a new ngram of the length length and place it in ngram.
 * Ngrams are generated from a corpus based on the offset of a previously generated ngram.
 * @param corpus Corpus to be used for the ngram generation.
 * @param ngram Pointer to a buffer of the size (length + 1) in which the new ngram is to be placed.
 * @param length Length of the ngram to be generated.
 * @param offset Offset of the previously generated ngram.
 */
void
t9_corpus_ngram(const corpus_t *const corpus,
                t9_symbol_t *const ngram,
                uint16_t length,
                size_t *const offset);

/*!
 * Count element wise how many characters in two character sequences are differing.
 * Both sequences are required to have the same length.
 * @param seq1 Pointer to character sequence 1.
 * @param seq2 Pointer to character sequence 2.
 * @param length Length of the character sequences including the 0 byte.
 * @return Amount of element wise differing characters.
 */
size_t
t9_corpus_sequence_diff(const t9_symbol_t *seq1,
                        const t9_symbol_t *seq2,
                        size_t length);

/*!
 * Convert a corpus text of size __buffer_size residing in __buffer into a lexicon text.
 * The result is writen to a newly allocated memory block and the pointer to that is written ti out.
 * @note The user is responsible for destroying the string using free once it is no longer required.
 * @param __buffer Pointer to a string containing the corpus text to be converted.
 * @param __buffer_size Length of the corpus text to be converted.
 * @param out Pointer to a variable where the pointer to the resulting string is placed.
 * @return T9_SUCCESS on success, otherwise T9_FAILURE.
 */
t9_error_t
t9_corpus_lexicon_from_corpus(const t9_symbol_t *const __buffer,
                              size_t __buffer_size,
                              t9_symbol_t **out);

#endif //C_T9_CORPUS_H
