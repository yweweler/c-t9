/*!
  ******************************************************************************
  * @file    main.c
  * @author  Yves-Noel Weweler <y.weweler@fh-muenster.de>
  * @version V1.0.0
  * @brief   T9 program entry.
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

#include <stdlib.h>
#include <stdio.h>

#include "t9/corpus.h"
#include "t9/tree.h"
#include "t9/timer.h"


void build_corpus_tree(t9_model_t *const model) {
    t9_corpus_tree_t *corpus_tree;

    // Build a corpus tree.
    corpus_tree = t9_corpus_tree_create();
    t9_corpus_tree_insert_ngrams(corpus_tree, &model->corpus, model->ngram_length);
    t9_corpus_tree_finalize(corpus_tree);

    model->corpus_tree = corpus_tree;
}

/*!
 * Example:
 * Evaluate the given statistical model by generating completing a known symbol sequence and comparing the deviation
 * between the generated sequence and the ground truth sequence.
 * @param model Pointer to the model to be evaluated.
 */
void example_evaluation(t9_model_t *const model) {
    t9_timer_t timer;
    double error;

// Evaluate model with the test corpus.
    printf("[Evaluation]: ...\n");

    t9_timer_start(&timer);
    if (t9_model_evaluate(model, &error) == T9_FAILURE) {
        printf("[Evaluation]: Error during evaluation.\n");
        return;
    };
    t9_timer_stop(&timer);

    printf("[Evaluation]: error %.3f, duration: %.2f ms.\n", error, t9_timer_duration_ms(&timer));
}

/*!
 * Example:
 * Autocomplete a given input sequence based on the statistical model.
 * @param model Pointer to the model to be used for completion.
 */
void example_autocomplete(t9_model_t *const model, const char * text) {
    t9_timer_t timer;
    t9_symbol_t *suggestion;
    const t9_symbol_t * input = (const t9_symbol_t *)text;

    // Autocomplete a given input sequence based onm the model.
    printf("[Completion]: Typing sequence: \"%s\"\n", (const char *) input);

    t9_timer_start(&timer);
    if (t9_model_autocomplete(model, input, &suggestion) == T9_FAILURE) {
        printf("[Completion]: Error during completion.\n");
        return;
    };
    t9_timer_stop(&timer);

    printf("[Completion]: Suggested: \"%s\"\n", suggestion);
    free(suggestion);
}

int main() {
    t9_timer_t timer;
    t9_model_t *model;
    const char *train_file;
    const char *test_file;
    size_t train_symbols;
    size_t test_symbols;

    model = t9_model_create();

    // Load the whole file for model training.
    train_file = "../data/trump/stripped.txt";
    train_symbols = 0;

    // Use only 1000 symbols for testing.
    test_file = "../data/trump/stripped.txt";
    test_symbols = 1000;

    // Load corpus.
    t9_timer_start(&timer);
    if (t9_corpus_load(train_file, train_symbols,
                       test_file, test_symbols,
                       &model->corpus) != T9_SUCCESS) {
        printf("Error: Could not load corpus.\n");
        return EXIT_FAILURE;
    }
    t9_timer_stop(&timer);
    printf("[Corpus]: Loaded (%zd bytes) in %.2f ms.\n",
           model->corpus.train_buffer_size + model->corpus.test_buffer_size,
           t9_timer_duration_ms(&timer));

    // Populate model with the train corpus.
    model->ngram_length = 3;
    // Number best completion paths (completion sequences) to maintain.
    model->number_paths = 15;
    // Build the statistical model.
    build_corpus_tree(model);
    // Initialize the search tree.
    model->search_tree = t9_search_tree_create();

    // Example 1: Simple completion of text.
    example_autocomplete(model, "366253#87867");

    // Example 2: Evaluation of the statistical model.
    // example_evaluation(model);

    t9_model_destroy(model);
    return EXIT_SUCCESS;
}