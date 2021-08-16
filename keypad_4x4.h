/*
* MIT License
* Copyright (c) 2021 Amirreza Firoozi
* Permission is hereby granted, free of charge, to any person obtaining a copy of
* this software and associated documentation files (the "Software"), to deal in
* the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell 
* copies of the Software, and to permit persons to whom the Software is furnished
* to do so, subject to the following conditions:
* The above copyright notice and this permission notice (including the next paragraph)
* shall be included in all copies or substantial portions of the Software.
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
* INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
* DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
* ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef __keypad_4x4_h
#define __keypad_4x4_h

#include <inttypes.h>
#include <stdbool.h>
#include "local_definitions.h"

/* AVOID editing this file directly. Define your MACROs in local_definitions.h instead */

#ifndef SAMPLE_COUNT
/*
 * Resolution or the number of samples taken
 */
#define SAMPLE_COUNT 2
#endif
#if SAMPLE_COUNT<2
#error "Minimum SAMPLE_COUNT is 2"
#endif


#ifndef DEBOUNCE_TIMER_PERIOD_MILLISECONDS
/*
 * This macro is purly informational and NOT functional
 * It's needed for TOTAL_DEBOUNCE_TIME to contain the correct information
 */
#define DEBOUNCE_TIMER_PERIOD_MILLISECONDS  20
#endif

#ifndef SAMPLE_INTERVAL_TIMER_CYCLES
/*
 * Time between two consecutive samples taken
 */
#define SAMPLE_INTERVAL_TIMER_CYCLES 2
#endif

/*
 * This macro contains the total time spent debouncing a button
 * This macro is purly informational and NOT functional
 */
#define TOTAL_DEBOUNCE_TIME (SAMPLE_COUNT * SAMPLE_INTERVAL_TIMER_CYCLES * DEBOUNCE_TIMER_PERIOD_MILLISECONDS)

#ifndef KEYPAD_DDR
#define KEYPAD_DDR DDRA
#endif

#ifndef KEYPAD_PORT
#define KEYPAD_PORT PORTA
#endif

#ifndef KEYPAD_PIN
#define KEYPAD_PIN PINA
#endif
/*
 *  Any arbitary value outside the range 0<=x<=15 can represent no key press...
 */
#define NO_KEY_PRESSED 127

uint8_t keypad_read(void);
uint8_t keypad_getKey(void);
bool keypad_isNewKeyAvailable(void);
void keypad_debounce(void);
void keypad_init(void);

#endif
