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


#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdbool.h>
#include "uart.h"
#include "keypad_4x4.h"

volatile uint8_t key_samples[SAMPLE_COUNT];
volatile uint8_t pressed_key = NO_KEY_PRESSED;

/*
 *  This flag is set when a valid pressed button is released
 *  When using keypad_getKey(), this flag is automatically cleared
 *  otherwise, you must clear it manually after each key detection...
 *  Use keypad_getKey() !
 */
volatile bool is_button_released = false; // is set when a valid button is released
volatile bool is_button_down = false;

/*
 * Returns debounced key
 * If there is no new key, it returns NO_KEY_PRESSED
 * It automagically resets the is_button_released to false to allow pickup of new keys.
 *
 */
uint8_t keypad_getKey(void);

/*
 * Returns true when there is a new keypress to be processed.
 */
bool keypad_isNewKeyAvailable(void);

/*
 * This function is meant to be used internally.
 *
 */
uint8_t keypad_read(void);
/*
 * execute this function inside a timer ISR
 */
void keypad_debounce(void);

/*
 * initialises keypad DDR and PORT
 * And populates samples with NO_KEY_PRESSED s
 *
 */
void keypad_init(void);


void keypad_init(void){
    KEYPAD_DDR = 0xF0;
    KEYPAD_PORT = 0xFF;

    for (uint8_t i = SAMPLE_COUNT; i > 0; --i){
        key_samples[i - 1] = NO_KEY_PRESSED;
    }

}

inline bool keypad_isNewKeyAvailable(void){
    return is_button_released;
}

uint8_t keypad_getKey(void){
    if (keypad_isNewKeyAvailable()){
            is_button_released = false;
            return pressed_key;
    }
    return NO_KEY_PRESSED;
}

uint8_t keypad_read(void){
    //printf_P(PSTR("entering keypad_read\n"));
    // keypad 4x4
    // rows as output
    // columns as input with pullup enabled
    // shifting a zero and over rows and reading columns
    // when both are zero the key is pressed

    // 0b0111 1111, 0b1011 1111, 0b1101 1111, 0b1110 1111
    // Is it more efficient to store this array in flash?
    uint8_t shift_array[4]={0x7F, 0xBF, 0xDF, 0xEF}; // Top left key is 0 
    //uint8_t shift_array[4]={0xEF, 0xDF, 0xBF, 0x7F}; // Buttom left key is 0 
    //
    // Valid key_codes : 0<=code<=15 any arbitary value not in this range could be used to indicate no key press
    // for example 20 :)
    uint8_t key_code = NO_KEY_PRESSED;
    /* Counting down in the loop is believed to have advantages over up. refer to 'AVR Software User Guide' */
    for (uint8_t i = 4; i > 0; --i){
        // exit the loop early if key is found
        if (key_code != NO_KEY_PRESSED) break;
        KEYPAD_PORT = shift_array[i-1];
        for (uint8_t j = 4; j > 0; --j){
            if ((KEYPAD_PIN & (1 << (4-j) )) == 0){
                // key pressed
                key_code = 20 - (4*i + j);
                //key_code = 4*(4-i) + (4-j);
                /*
                if (key_code != NO_KEY_PRESSED)
                    printf("i: %" PRIu8 " j: %" PRIu8 "\n", i, j);
                */
                // when key is found exit the loop
                break;
            }
        }
    }// end of for
    //printf_P(PSTR("exiting keypad_read\n"));
    KEYPAD_PORT = 0xFF;
    return key_code;
}// end of keypad_read function


void keypad_debounce(void){
    volatile static uint8_t timer_cycles = 0;
    volatile static uint8_t sample_index = SAMPLE_COUNT - 1;
    
    timer_cycles += 1;
    //printf_P(PSTR("T: %" PRIu8 "\n"), timer_cycles);
    if (timer_cycles >= SAMPLE_INTERVAL_TIMER_CYCLES){
        //printf_P(PSTR("T > I\n"));
        // sample every SAMPLE_INTERVAL_TIMER_CYCLES * DEBOUNCE_TIMER_PERIOD_MILLISECONDS milliseconds
        // testing to see if sample index goes out of range!
        // printf_P(PSTR("S: %" PRIu8 "\n"), sample_index);
        key_samples[sample_index] = keypad_read();
        // if button is released...
        if(is_button_down && (key_samples[sample_index] == NO_KEY_PRESSED)){
            is_button_released = true;
            is_button_down = false;
        }

        if(sample_index == (SAMPLE_COUNT-1)){
            bool is_valid_keypress = true;
            // if samples differ OR if they are NO_KEY_PRESSED, they are invalid
            for ( uint8_t i = SAMPLE_COUNT - 1; i > 0; --i ){
                if ( (key_samples[i - 1] != key_samples[i]) || (key_samples[i] == NO_KEY_PRESSED) ){
                    is_valid_keypress = false;
                    break;
                } 
            }

            if (is_valid_keypress){
                pressed_key = key_samples[0];

                // resetting samples...
                for (uint8_t i = SAMPLE_COUNT ; i > 0 ; --i){
                    key_samples[i - 1] = NO_KEY_PRESSED;
                }

                is_button_down = true;
            }// end of is_valid_keypress
        }
        // e.g. SAMPLE_COUNT = 4; 0<= sample_index <= 3
        // why do I love weird code? IDK :| :)
        //sample_index = sample_index == 0 ? SAMPLE_COUNT - 1 : sample_index - 1;

        // Setting index to SAMPLE_COUNT if it is zero, because it will get decremented and will become in range
        if (sample_index == 0) sample_index = SAMPLE_COUNT;

        sample_index -= 1;

        // reset the timer_cycles 
        timer_cycles = 0;

    }// end of timer_cycles>=SAMPLE_INTERVAL_TIMER_CYCLES if


}// end of keypad debounce function

