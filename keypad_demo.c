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


//#define F_CPU 12288000UL
#include <avr/io.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/pgmspace.h>
#include <stdlib.h>
#include "uart.h"
#include "keypad_4x4.h"

static FILE mystd_uart = FDEV_SETUP_STREAM(uart_putchar, uart_getchar , _FDEV_SETUP_RW);

void invoke_sleep(void){
    // Invoking the sleep here
    cli();
    sleep_enable();
    //sleep_bod_disable();
    sei();
    sleep_cpu();
    sleep_disable();
    sei();
//
}


ISR (TIMER0_COMP_vect){
    keypad_debounce();
}

int main(void)
{
    // TODO: change timer setup here to match you F_CPU and Timer Period
    // F_CPU = 12288000UL
    // Timer Period: 20 ms
    TCCR0=(1<<WGM01) | (1<<CS01) | (1<<CS00);
    OCR0=0xEF;
    TIMSK=(1<<OCIE0);


    uart_init();
    stdin = &mystd_uart;
    stdout = &mystd_uart;

    set_sleep_mode(SLEEP_MODE_IDLE);
    keypad_init();

    sei();

    for(;;){
        /*
         * How to use the keypad_4x4 library:
         * Check for a new key:
         * if(keypad_isNewKeyAvailable()){
         *   // place EITHER keypad_getc() OR keypad_getKey() here.
         *   // Do stuff with your key...
         *   }
         *
         *   keypad_getKey() returns the key code (0<= key_code <= 15)
         *   Keypad_getc() returns the character corresponding to that key from keypad_font[] array
         *
         *   keypad_getKey() and keypad_getc() consume the pressed key
         *   if you place one after the other the second call will contain NO_KEY_PRESSED or NO_KEY_PRESSED_CHAR
         *
         *   // suppose the user presses the 6th key
         *   if (keypad_isNewKeyAvailable()){
         *      uint8_t key = keypad_getKey(); // contains 5
         *      uint8_t key_char = keypad_getc(); // contains NO_KEY_PRESSED_CHAR !
         *   }
         *
         *   // To avoid the above situation you should use keypad_translateKey()
         *   if (keypad_isNewKeyAvailable()){
         *      uint8_t key = keypad_getKey(); // contains 5
         *      uint8_t key_char = keypad_translateKey(key); // contains keypad_font[5]
         *   }
         *
         *
         */

        if (keypad_isNewKeyAvailable()){
            // this returns the key code not the character!
            uint8_t key = keypad_getKey();
            printf_P(PSTR("Key Code: %" PRIu8 " Key Char: %c \n"), key, keypad_translateKey(key));
        }
        invoke_sleep();
    }
    return 1;
}


