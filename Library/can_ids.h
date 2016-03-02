/*
 * can_ids.h
 * 
 * Contains CAN ids for any message that needs to be sent.
 * This file will eventually be auto-generated by PCAN/Python script
 * 
 * Copyright (c) Carnegie Mellon Racing 2016
 */

#ifndef CAN_IDS_H_
#define CAN_IDS_H_

// Errors (base 0x000)
#define FATAL_ERROR_ID				0x000

// State and signals (base 0x100)
#define STATE_ID_BASE_ADDR			0x100
#define STATE_ID					STATE_ID_BASE_ADDR + 0
#define HV_BUTTON_ID				STATE_ID_BASE_ADDR + 1
#define RTD_BUTTON_ID				STATE_ID_BASE_ADDR + 2
#define RESET_BUTTON_ID				STATE_ID_BASE_ADDR + 3
#define DAQ_BUTTON_ON_ID			STATE_ID_BASE_ADDR + 4
#define DAQ_BUTTON_OFF_ID			STATE_ID_BASE_ADDR + 5


// Heartbeats, the current state of each node shall be transmitted on 
// a heartbeat. (base 0x200)
#define HEARTBEAT_BASE_ADDR			0x200
#define MAIN_HEARTBEAT_ID			HEARTBEAT_BASE_ADDR + 0
#define	FSN_HEARTBEAT_ID			HEARTBEAT_BASE_ADDR + 1
#define FSM_HEARTBEAT_ID			HEARTBEAT_BASE_ADDR + 4
#define RSN_HEARTBEAT_ID			HEARTBEAT_BASE_ADDR + 2
#define UM6_HEARTBEAT_ID			HEARTBEAT_BASE_ADDR + 3
#define DAQ_HEARTBEAT_ID			HEARTBEAT_BASE_ADDR + 4
#define DASH_HEARTBEAT_ID			HEARTBEAT_BASE_ADDR + 5

/* Motor Controller Control Commands */

// Drive Commands
// Left MC
#define MC_DRV_CTRL_BASE_ADDR_L		0x300	//Motor Driver Controls Base Address
#define MC_DRV_CMD_L				MC_DRV_CTRL_BASE_ADDR_L + 1 //Motor Current, velocity setpoint
#define MC_PWR_CMD_L				MC_DRV_CTRL_BASE_ADDR_L + 2 //Motor Power Command, set bus current limit
#define MC_RST_CMD_L				MC_DRV_CTRL_BASE_ADDR_L + 3 //Motor Reset Command


// Right MC
#define MC_DRV_CTRL_BASE_ADDR_R		0x310	//Motor Driver Controls Base Address
#define MC_DRV_CMD_R				MC_DRV_CTRL_BASE_ADDR_R + 1 //Motor Current, velocity setpoint
#define MC_PWR_CMD_R				MC_DRV_CTRL_BASE_ADDR_R + 2 //Motor Power Command, set bus current limit
#define MC_RST_CMD_R				MC_DRV_CTRL_BASE_ADDR_R + 3 //Motor Reset Command

/* Motor Controller Broadcast Messages */


// Sensor readings (base 0x400)
#define SENSOR_MSG_BASE_ADDR		0x400	
//FSN
#define DAMPER_POSITION_LEFT_ID		SENSOR_MSG_BASE_ADDR + 0
#define DAMPER_POSITION_RIGHT_ID	SENSOR_MSG_BASE_ADDR + 1	
#define CURRENT_ID					SENSOR_MSG_BASE_ADDR + 2
#define STEERING_WHEEL_ANGLE_ID		SENSOR_MSG_BASE_ADDR + 3
#define BRAKE_PRESSURE_ID			SENSOR_MSG_BASE_ADDR + 4
#define THROTTLE_POSITION_L_ID		SENSOR_MSG_BASE_ADDR + 5
#define THROTTLE_POSITION_R_ID		SENSOR_MSG_BASE_ADDR + 6
// RSN	
#define WTEMP_L_CH_ID				SENSOR_MSG_BASE_ADDR + 7	
#define WTEMP_R_CH_ID				SENSOR_MSG_BASE_ADDR + 8
#define SLINEAR_L_CH_ID				SENSOR_MSG_BASE_ADDR + 9
#define SLINEAR_R_CH_ID				SENSOR_MSG_BASE_ADDR + 10
// UM6
#define ACCEL_X_ID					SENSOR_MSG_BASE_ADDR + 11	
#define ACCEL_Y_ID					SENSOR_MSG_BASE_ADDR + 12	
#define ACCEL_Z_ID					SENSOR_MSG_BASE_ADDR + 13
#define GYRO_X_ID					SENSOR_MSG_BASE_ADDR + 14
#define GYRO_Y_ID					SENSOR_MSG_BASE_ADDR + 15	
#define GYRO_Z_ID					SENSOR_MSG_BASE_ADDR + 16
#define MAG_X_ID					SENSOR_MSG_BASE_ADDR + 17
#define MAG_Y_ID					SENSOR_MSG_BASE_ADDR + 18
#define MAG_Z_ID					SENSOR_MSG_BASE_ADDR + 19

// Configuration and diagnostics
//TBD




// ... to be continued


#endif /* CAN_IDS_H_ */