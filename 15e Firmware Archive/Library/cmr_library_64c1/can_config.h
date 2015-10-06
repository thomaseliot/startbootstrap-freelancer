/*
 * can_config.h
 * 
 * A configuration file for CAN constants such as baudrate and oscillator frequency
 * 
 * Created: 3/23/2015
 * Author: Shepard Emerson (semerson)
 * Platform: ATmega64C1
 * Modified: 4/15/2015
 * 
 * Change List:
 *  - 4/15/15: Added change list and known errors to file header (Shepard Emerson)
 * 
 * Known Errors:
 *	- None
 */

#ifndef CAN_CONFIG_H_
#define CAN_CONFIG_H_

// Includes
#include <avr/io.h>
#include <avr/interrupt.h>

typedef uint8_t Bool;

// Macros, useful for CAN stuff
// Max(a, b): Take the max between a and b
// Min(a, b): Take the min between a and b
// Align_up(val, n):   Around (up)   the number (val) on the (n) boundary
// Align_down(val, n): Around (down) the number (val) on the (n) boundary
#define Max(a, b)          ( (a)>(b) ? (a) : (b) )
#define Min(a, b)          ( (a)<(b) ? (a) : (b) )

// CAN LIB Configuration
// 15e_todo: CHANGE BAUDRATE TO BE THE CANBUS BAUDRATE OF THE CAR
#define CAN_BAUDRATE	500        // in kBit
#define FOSC			16000
#define F_CPU          (FOSC*1000) // Need for AVR GCC

#endif /* CAN_CONFIG_H_ */