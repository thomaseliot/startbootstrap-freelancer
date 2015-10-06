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
#define MCU_STATUS_PORT			IO_PORT_C	// MCU Status is port D pin 7
#define MCU_STATUS_PIN			0	


// Sensor Configuration
// 15e_todo_done: If you have sensors, change the #define names and the channels they correspond to
#define WTEMP_L_CH		2
#define WTEMP_R_CH		3
#define SLINEAR_L_CH	4	
#define SLINEAR_R_CH	5	


//Cooling System Configuration
//Fan Enable
#define FAN_ENABLE_PORT IO_PORT_D
#define FAN_ENABLE_PIN 1
//Fan PWM Control
#define FAN_PWM_PORT IO_PORT_D
#define FAN_PWM_PIN 3

//Left Coolant Pump
#define PUMP_L_CTRL_PORT IO_PORT_B 
#define PUMP_L_CTRL_PIN 3

//Right Coolant Pump
#define PUMP_R_CTRL_PORT IO_PORT_B
#define PUMP_R_CTRL_PIN 4


//Brake Light Configuration
#define BRAKE_CTRL_PORT IO_PORT_D
#define BRAKE_CTRL_PIN 0 


// CAN LIB Configuration
#define FOSC 16000


#endif  // _CONFIG_H_