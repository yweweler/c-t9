/*!
  ******************************************************************************
  * @file    corpus.c
  * @author  Yves-Noel Weweler <y.weweler@fh-muenster.de>
  * @version V1.0.0
  * @brief   This file implements a text based data corpus.
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

#include "t9/corpus.h"

t9_error_t
t9_corpus_load(const char *train_path,
               size_t train_limit,
               const char *test_path,
               size_t test_limit,
               corpus_t *corpus) {
    if (t9_read_file(train_path, &corpus->train_buffer_size, &corpus->train_buffer, train_limit) != T9_SUCCESS) {
        return T9_FAILURE;
    }

    if (t9_read_file(test_path, &corpus->test_buffer_size, &corpus->test_buffer, test_limit) != T9_SUCCESS) {
        return T9_FAILURE;
    }

    return T9_SUCCESS;
}

t9_error_t
t9_corpus_unload(corpus_t *corpus) {
    if (corpus == NULL) {
        return T9_FAILURE;
    }

    if (corpus->train_buffer != NULL) {
        free(corpus->train_buffer);
    }

    if (corpus->test_buffer != NULL) {
        free(corpus->test_buffer);
    }

    corpus->train_buffer_size = 0;
    corpus->test_buffer_size = 0;
    return T9_SUCCESS;
}

const t9_symbol_t *
t9_corpus_ltoc(t9_symbol_t symbol) {
    switch (symbol) {
        case '0':
            return (const t9_symbol_t *) SYMBOLS_T0;
        case '1':
            return (const t9_symbol_t *) SYMBOLS_T1;
        case '2':
            return (const t9_symbol_t *) SYMBOLS_T2;
        case '3':
            return (const t9_symbol_t *) SYMBOLS_T3;
        case '4':
            return (const t9_symbol_t *) SYMBOLS_T4;
        case '5':
            return (const t9_symbol_t *) SYMBOLS_T5;
        case '6':
            return (const t9_symbol_t *) SYMBOLS_T6;
        case '7':
            return (const t9_symbol_t *) SYMBOLS_T7;
        case '8':
            return (const t9_symbol_t *) SYMBOLS_T8;
        case '9':
            return (const t9_symbol_t *) SYMBOLS_T9;
        case '*':
            return (const t9_symbol_t *) SYMBOLS_TS;
        case '#':
            return (const t9_symbol_t *) SYMBOLS_TR;
        default:
            return NULL;
    }
}

t9_error_t
t9_corpus_ctol(t9_symbol_t symbol, t9_symbol_t *out) {
    uint16_t key;
    const t9_symbol_t *ptr;

    const t9_symbol_t *const key_table[] = {
            (const t9_symbol_t *const) SYMBOLS_T0,
            (const t9_symbol_t *const) SYMBOLS_T1,
            (const t9_symbol_t *const) SYMBOLS_T2,
            (const t9_symbol_t *const) SYMBOLS_T3,
            (const t9_symbol_t *const) SYMBOLS_T4,
            (const t9_symbol_t *const) SYMBOLS_T5,
            (const t9_symbol_t *const) SYMBOLS_T6,
            (const t9_symbol_t *const) SYMBOLS_T7,
            (const t9_symbol_t *const) SYMBOLS_T8,
            (const t9_symbol_t *const) SYMBOLS_T9,
            (const t9_symbol_t *const) SYMBOLS_TS,
            (const t9_symbol_t *const) SYMBOLS_TR
    };
    // For each T9 key.
    for (key = 0; key < sizeof(key_table) / sizeof(const t9_symbol_t *const); key++) {
        ptr = key_table[key];
        // Check if the symbol to be converted is assigned to it.
        while (*ptr != 0) {
            if (*ptr == symbol) {
                // Corresponding T9 key was found.
                *out = (const t9_symbol_t) LEXICON_SYMBOLS[key];
                return T9_SUCCESS;
            }
            ptr++;
        }
    }
    // Symbol is not assigned to any T9 key.
    return T9_FAILURE;
}

bool
t9_corpus_validate_lexicon_symbol(t9_symbol_t symbol) {
    const t9_symbol_t *lexicon_symbol_table;

    lexicon_symbol_table = (const t9_symbol_t *) LEXICON_SYMBOLS;
    // Cycle all known lexicon symbols an check if it the given symbol can be found.
    while (symbol != *lexicon_symbol_table) {
        lexicon_symbol_table++;
        if (*lexicon_symbol_table == 0) {
            // Symbol is not a known lexicon symbol.
            return false;
        }
    }
    return true;
}

bool
t9_corpus_validate_lexicon_symbols(const t9_symbol_t *const symbols) {
    const t9_symbol_t *symbol;

    if (symbols == NULL) {
        return false;
    }

    // Check for every symbol of the string if it is a valid lexicon symbol.
    symbol = symbols;
    while (*symbol != 0) {
        if (t9_corpus_validate_lexicon_symbol(*symbol) == false) {
            // Invalid lexicon symbol found.
            return false;
        }
        symbol++;
    }

    // All symbols are valid lexicon symbols.
    return true;
}

void
t9_corpus_ngram(const corpus_t *const corpus,
                t9_symbol_t *const ngram,
                uint16_t length,
                size_t *const offset) {
    // Stop when there is not enough data for a new ngram.
    if (((corpus->train_buffer_size - 1) - (*offset)) < length) {
        *ngram = 0;
        *offset = corpus->train_buffer_size;
        return;
    }

    // Create a new ngram.
    memcpy(ngram, &corpus->train_buffer[*offset], length);
    ngram[length] = 0;
    *offset += 1;
}

size_t
t9_corpus_sequence_diff(const t9_symbol_t *seq1,
                        const t9_symbol_t *seq2,
                        size_t length) {
    size_t i;
    size_t diff;

    diff = 0;
    for (i = 0; i < length; i++) {
        if (seq1[i] != seq2[i]) {
            diff++;
        }
    }

    return diff;
}

t9_error_t
t9_corpus_lexicon_from_corpus(const t9_symbol_t *const __buffer,
                              size_t __buffer_size,
                              t9_symbol_t **out) {
    t9_symbol_t *buffer;
    size_t i;

    buffer = (t9_symbol_t *) malloc(__buffer_size + 1);
    if (buffer == NULL) {
        return T9_FAILURE;
    }

    for (i = 0; i < __buffer_size; i++) {
        if (t9_corpus_ctol(__buffer[i], &buffer[i]) == T9_FAILURE) {
            free(buffer);
            return T9_FAILURE;
        }
    }
    buffer[__buffer_size] = 0;
    *out = buffer;

    return T9_SUCCESS;
}