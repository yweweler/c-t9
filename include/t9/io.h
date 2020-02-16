/*!
  ******************************************************************************
  * @file    io.h
  * @author  Yves-Noel Weweler <y.weweler@fh-muenster.de>
  * @version V1.0.0
  * @brief   Header file for io.c
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

#ifndef C_T9_IO_H
#define C_T9_IO_H

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "errno.h"
#include "corpus.h"

/*!
 * Check if a file exists.
 * @param path Path to the file.
 * @param exists Is set to 1 if the file exists. 0 if it does not exists. On failure the value is not changed.
 * @return T9_SUCCESS on success, T9_FAILURE otherwise.
 */
t9_error_t
t9_file_exists(const char *const path, uint8_t *const exists);

/*!
 * Get the size of a file in bytes.
 * @param path Path to the file.
 * @param size Is set to the file size. On failure the value is not changed.
 * @return T9_SUCCESS on success, T9_FAILURE otherwise.
 */
t9_error_t
t9_file_size(const char *const path, size_t *const size);

/*!
 * Read the content of a file to memory.
 * @note buffer has to be freed by the user once it is not longer used.
 * @param path Pointer to a string with the path of the file to be read.
 * @param size Is set to the file size. On failure the value is not changed.
 * @param buffer Pointer to a variable were the address of the read bytes should be placed.
 * @param max_size Maximal number of bytes to be read. If 0 is supplied, the while file will be read.
 * @return T9_SUCCESS on success, T9_FAILURE otherwise.
 */
t9_error_t
t9_read_file(const char *const path,
             size_t *const size,
             t9_symbol_t **const buffer,
             size_t max_size);

#endif //C_T9_IO_H
