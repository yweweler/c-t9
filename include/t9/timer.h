/*!
  ******************************************************************************
  * @file    timer.h
  * @author  Yves-Noel Weweler <y.weweler@fh-muenster.de>
  * @version V1.0.0
  * @brief   Header file for timer.c
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

#ifndef C_T9_TIMER_H
#define C_T9_TIMER_H

#include <time.h>
#include <stdint.h>

/*!
 * Timer that can be used to measure code execution times.
 */
struct struct_t9_timer_t {
    clock_t start;
    clock_t end;
};

typedef struct struct_t9_timer_t t9_timer_t;

/*!
 * Start a timer.
 * @param timer Pointer to a timer to be started.
 */
void
t9_timer_start(t9_timer_t *const timer);

/*!
 * Stop a timer.
 * @param timer Pointer to a timer to be stopped.
 */
void
t9_timer_stop(t9_timer_t *const timer);

/*!
 * Restart a timer. The timer gets reinitialized and started.
 * @param timer Pointer to a timer to be restarted.
 */
void
t9_timer_restart(t9_timer_t *const timer);

/*!
 * Stop timer and query elapsed time in milliseconds.
 * @param timer Pointer to a timer to be queried.
 * @return Elapsed time in milliseconds.
 */
double
t9_timer_duration_ms(t9_timer_t *const timer);

#endif //C_T9_TIMER_H
