/*
 * config.h
 
	The contents of this file were originally from the CAN Software Library
	provided by Atmel written for AT90CAN devices. Use of this file is subject
	to Atmel's End User License Agreement.

	THIS FILE SHOULD BE PLACED IN EACH INDIVIDUAL PROJECT, NOT THE LIBRARY
 
 */ 

#ifndef _CONFIG_H_
#define _CONFIG_H_

//_____ I N C L U D E S ________________________________________________________
#include <avr/io.h>
#include <avr/interrupt.h>

//_____ M A C R O S ____________________________________________________________
//! Some useful macros...
// Max(a, b): Take the max between a and b
// Min(a, b): Take the min between a and b
// Align_up(val, n):   Around (up)   the number (val) on the (n) boundary
// Align_down(val, n): Around (down) the number (val) on the (n) boundary
#define Max(a, b)          ( (a)>(b) ? (a) : (b) )
#define Min(a, b)          ( (a)<(b) ? (a) : (b) )

typedef uint8_t Bool;

//_____ D E F I N I T I O N S __________________________________________________

// -------------- MCU LIB CONFIGURATION 
// 15e_todo: CHANGE OSCILLATOR FREQUENCY TO BE THE FREQUENCY OF YOUR OSCILLATOR
#define FOSC           16000       // 16Mhz external clock
#define F_CPU          (FOSC*1000) // Need for AVR GCC

// -------------- CAN LIB CONFIGURATION
// 15e_todo: CHANGE BAUDRATE TO BE THE CANBUS BAUDRATE OF THE CAR
#define CAN_BAUDRATE   500        // in kBit

    // -------------- MISCELLANEOUS

//_____ D E C L A R A T I O N S ________________________________________________

#endif  // _CONFIG_H_