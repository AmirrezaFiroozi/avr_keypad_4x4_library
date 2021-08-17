#ifndef __local_definitions_h
#define __local_definitions_h

/* Customize your keypad settings here*/

/*
 * Valid values for KEYPAD_LAYOUT are KEYPAD_GENERIC_LAYOUT, KEYPAD_CALC_LAYOUT or:
 * The trailing backslashes are mandatory!
 * {'', '', '', '', \
 *  '', '', '', '', \
 *  '', '', '', '', \
 *  '', '', '', ''}
 *
 */
#define KEYPAD_LAYOUT KEYPAD_GENERIC_LAYOUT 

#define KEYPAD_DDR DDRA
#define KEYPAD_PORT PORTA
#define KEYPAD_PIN PINA

#define SAMPLE_COUNT 2
// interval between each sample and the next
// if TIMER_PERIOD is 20ms setting this to 2
// will cause the each sample to be taken 2 * 20 = 40ms later than the previous
#define SAMPLE_INTERVAL_TIMER_CYCLES 2
#endif


