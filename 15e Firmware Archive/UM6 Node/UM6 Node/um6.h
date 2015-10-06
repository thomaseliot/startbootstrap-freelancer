/*
 * um6.h
 * UM6 helper functions and constants
 * 
 * Created: 4/23/2015
 * Author:	Daniel Gorziglia	(dgorzigl)
			Suyash Bhatt		(sbhatt)
 *			Shepard Emerson		(semerson)
 * Platform: ATmega64C1
 * Modified: 4/23/2015
 * 
 */

#include <avr/io.h>
#include "init.h"

#ifndef UM6_H_
#define UM6_H_

// UM6 operation codes
#define UM6_READ	0x00
#define UM6_WRITE	0x01
#define UM6_CMD		0x02

// UM6 config options
#define UM6_CMD_LEN	4

// Register IDs
#define UM6_STATUS				0x55
#define UM6_GYRO_PROC_XY		0x5C
#define UM6_GYRO_PROC_Z			0x5D
#define UM6_ACCEL_PROC_XY		0x5E
#define UM6_ACCEL_PROC_Z		0x5F
#define UM6_MAG_PROC_XY			0x60
#define UM6_MAG_PROC_Z			0x61
#define UM6_GPS_LONGITUDE		0x77
#define UM6_GPS_LATITUDE		0x78
#define UM6_GPS_ALTITUDE		0x79
#define UM6_GPS_COURSE_SPEED	0x7D

// Struct to hold all UM6 data
struct um6_data_struct {
	uint16_t gyro_x;
	uint16_t gyro_y;
	uint16_t gyro_z;
	uint16_t accel_x;
	uint16_t accel_y;
	uint16_t accel_z;
	uint16_t mag_x;
	uint16_t mag_y;
	uint16_t mag_z;
};

// um6 data variable
struct um6_data_struct um6_data;


// Helper function prototypes
/* um6_command
 * Perform a transaction (send and receive) to read/write/command registers
 */
void um6_command(uint8_t cmd_type, uint8_t reg, uint8_t* buffer_ptr, uint8_t buffer_len);

#endif /* UM6_H_ */