/*!
  ******************************************************************************
  * @file    io.c
  * @author  Yves-Noel Weweler <y.weweler@fh-muenster.de>
  * @version V1.0.0
  * @brief   This file implements IO helper functions for handling files.
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

#include "t9/io.h"

t9_error_t
t9_file_exists(const char *const path, uint8_t *const exists) {
    int fd;

    fd = open(path, O_RDONLY | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);
    if (fd < 0) {
        if (errno == EEXIST) {
            *exists = 1;
        } else {
            *exists = 0;
        }

        return T9_SUCCESS;
    }
    return T9_FAILURE;
}

t9_error_t
t9_file_size(const char *const path, size_t *const size) {
    FILE *fp;
    ssize_t offset;

    // Open file.
    fp = fopen(path, "r");
    if (fp == NULL) {
        return T9_FAILURE;
    }

    // Query file size.
    fseek(fp, 0L, SEEK_END);
    offset = ftell(fp);

    if (offset == -1) {
        fclose(fp);
        return T9_FAILURE;
    }

    *size = (size_t) offset;

    // Close file.
    fclose(fp);
    return T9_SUCCESS;
}

t9_error_t
t9_read_file(const char *const path,
             size_t *const size,
             t9_symbol_t **const buffer,
             size_t max_size) {
    size_t file_size;
    FILE *fp;
    t9_symbol_t *buffer_ptr;
    size_t read;
    uint8_t exists;

    // Invalid arguments.
    if (path == NULL || buffer == NULL) {
        return T9_FAILURE;
    }

    if (t9_file_exists(path, &exists) != T9_SUCCESS) {
        return T9_FAILURE;
    }

    if (t9_file_size(path, &file_size) != T9_SUCCESS) {
        return T9_FAILURE;
    }

    // Limit file size to be loaded.
    if (max_size != 0 && file_size > max_size) {
        file_size = max_size;
    }

    // Open corpus file.
    fp = fopen(path, "r");
    if (fp == NULL) {
        return T9_FAILURE;
    }

    // Allocate memory for the file.
    buffer_ptr = malloc(file_size + 1);
    if (buffer_ptr == NULL) {
        return T9_FAILURE;
    }

    *size = file_size;
    *buffer = buffer_ptr;

    // Read file to memory.
    while (file_size > 0) {
        read = fread((void *) buffer_ptr, sizeof(uint8_t), file_size, fp);
        if (read != file_size) {
            // Read error occurred.
            free(buffer_ptr);
            return T9_FAILURE;
        }
        file_size -= read;
        buffer_ptr += read;
    }
    buffer_ptr[file_size] = 0;

    fclose(fp);
    return T9_SUCCESS;
}