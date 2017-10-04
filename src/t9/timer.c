/*!
  ******************************************************************************
  * @file    timer.c
  * @author  Yves-Noel Weweler <y.weweler@fh-muenster.de>
  * @version V1.0.0
  * @brief   This file implements helper functions to measure execution times.
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

#include "t9/timer.h"

void
t9_timer_start(t9_timer_t *const timer) {
    if (timer == NULL) {
        return;
    }

    timer->start = clock();
}

void
t9_timer_stop(t9_timer_t *const timer) {
    if (timer == NULL) {
        return;
    }

    timer->end = clock();
}

void
t9_timer_restart(t9_timer_t *const timer) {
    if (timer == NULL) {
        return;
    }

    timer->start = 0;
    timer->end = 0;

    t9_timer_start(timer);
}

double
t9_timer_duration_ms(t9_timer_t *const timer) {
    if (timer == NULL) {
        return 0;
    }

    t9_timer_stop(timer);
    return ((double) (timer->end - timer->start) / CLOCKS_PER_SEC) * 1000.0;
}