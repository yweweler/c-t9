/*!
  ******************************************************************************
  * @file    math.c
  * @author  Yves-Noel Weweler <y.weweler@fh-muenster.de>
  * @version V1.0.0
  * @brief   This file implements math helper functions.
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

#include "t9/math.h"

float
t9_ln(float x) {
    double tmp;

    tmp = x + DBL_MIN;
    if (tmp > 1.0) {
        tmp = 1.0;
    }
    return (float) log(tmp);
}