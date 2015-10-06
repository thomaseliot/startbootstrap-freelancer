/*
 * config.h
 * Board-specific configuration
 * 
 * Created: 4/10/2015
 * Author: Shepard Emerson (semerson)
 * Platform: ATmega64C1
 * Modified: 4/15/2015
 * 
 * Change List:
 *  - 4/15/15: Added change list and known errors to file header (Shepard Emerson)
 * 
 * Known Errors:
 *	- CAN frequency is locked at 16MHz. Need to restructure CAN library to fix (Shepard Emerson)
 */

// Check if this has been defined yet, if not, define everything
#ifndef _CONFIG_H_
#define _CONFIG_H_

// DEFINITIONS
// Digital Port Configuration
#define MCU_STATUS_PORT			IO_PORT_D	// MCU Status is port D pin 7
#define MCU_STATUS_PIN			7	

// Sensor Configuration
// 15e_todo: If you have sensors, change the #define names and the channels they correspond to
#define DAMPER_POSITION_LEFT	5	// ADC_CH_1 -> ADC5
#define DAMPER_POSITION_RIGHT	7	// ADC_CH_2 -> ADC7
#define CURRENT					6	// ADC_CH_3 -> ADC6
#define STEERING_WHEEL_ANGLE	8	// ADC_CH_4 -> ADC8
#define BRAKE_PRESSURE			10	// ADC_CH_6 -> ADC10
#define THROTTLE_POSITION_L		9	// ADC_CH_7 -> ADC9
#define THROTTLE_POSITION_R		2	// ADC_CH_8 -> ADC2

// Watchdog timeout
#define WATCHDOG_TIMEOUT		WDTO_120MS	// 120ms timeout

#endif  // _CONFIG_H_