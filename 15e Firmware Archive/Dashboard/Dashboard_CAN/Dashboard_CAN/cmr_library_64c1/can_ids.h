/*
 * can_ids.h
 * 
 * Contains CAN ids for any message that needs to be sent.
 * This file will eventually be auto-generated by PCAN/Python script
 * 
 * Created: 4/15/2015 11:21:24 AM
 * Author: Shepard Emerson (semerson)
 * Platform: ATMega64C1
 * Modified: 4/15/2015
 * 
 * Change List:
 *  - 4/15/15: created file (Shepard Emerson)
 *  - 4/25/15: added signals for button presses (Bob Rudolph)
 * 
 * Known Errors:
 *	- None
 */

#ifndef CAN_IDS_H_
#define CAN_IDS_H_

// Errors (base 0x000)
#define FATAL_ERROR_ID				0x000

// Motor Controller Controls
#define MC_DRIVER_CTRL_BASE_ADDR_L	0x050
#define MC_MOTOR_DRIVE_COMMAND_L	MC_DRIVER_CTRL_BASE_ADDR_L + 1
#define MC_DRIVER_CTRL_BASE_ADDR_R	0x060
#define MC_MOTOR_DRIVE_COMMAND_R	MC_DRIVER_CTRL_BASE_ADDR_R + 1

// State and signals (base 0x100)
#define STATE_ID					0x100
#define HV_BUTTON_ID				0x101
#define RTD_BUTTON_ID				0x102
#define RESET_BUTTON_ID				0x103
#define DAQ_BUTTON_ON_ID			0x104
#define DAQ_BUTTON_OFF_ID			0x105


// Heartbeats, the current state of each node shall be transmitted on 
// a heartbeat. (base 0x200)
#define MAIN_HEARTBEAT_ID			0x200
#define	FSN_HEARTBEAT_ID			0x201
#define RSN_HEARTBEAT_ID			0x202
#define UM6_HEARTBEAT_ID			0x203
#define DAQ_HEARTBEAT_ID			0x204
#define DASH_HEARTBEAT_ID			0x205

// Sensor readings (base 0x400)
#define DAMPER_POSITION_LEFT_ID		0x300	// FSN
#define DAMPER_POSITION_RIGHT_ID	0x301	
#define CURRENT_ID					0x302
#define STEERING_WHEEL_ANGLE_ID		0x303
#define BRAKE_PRESSURE_ID			0x304
#define THROTTLE_POSITION_L_ID		0x305
#define THROTTLE_POSITION_R_ID		0x306	
#define WTEMP_L_CH_ID				0x307	// RSN
#define WTEMP_R_CH_ID				0x308
#define SLINEAR_L_CH_ID				0x309
#define SLINEAR_R_CH_ID				0x30A
#define ACCEL_X_ID					0x30B	// UM6
#define ACCEL_Y_ID					0x30C	
#define ACCEL_Z_ID					0x30D
#define GYRO_X_ID					0x30E
#define GYRO_Y_ID					0x30F	
#define GYRO_Z_ID					0x310
#define MAG_X_ID					0x311
#define MAG_Y_ID					0x312
#define MAG_Z_ID					0x313

// Configuration and diagnostics
#define UM6_FIRMWARE                0x400

// ... to be continued


#endif /* CAN_IDS_H_ */