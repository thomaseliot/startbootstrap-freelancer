/*
 * Cooling.h
 * The main file for the rear sensor node cooling control
 * 
 * Created: 4/19/2015
 * Author: Daniel Haddox (dhaddox)
 * Platform: ATmega64C1
 * Modified: 4/20/2015
 * 
 * Change List:
 *
 */


#include <avr/io.h>

#ifndef COOLING_H_
#define COOLING_H_

//Macros for turning cooling system on
#define COOLING_ENABLE 1
#define COOLING_DISABLE 0


/* Intelligent coolant timing
 * All calculations assume cooling loop is ran at 100Hz */

//Cooling system state
extern uint8_t cooling_state;
extern uint8_t cooling_delay_flag;
extern uint8_t cooling_delay_cnt;

//Cooling system startup delays, in milliseconds
#define PUMP_L_DELAY 0
#define PUMP_R_DELAY 500
#define FAN_DELAY 1000

//Cooling system delay flags and counters
#define PUMP_L_CNT PUMP_L_DELAY/10
#define PUMP_R_CNT PUMP_R_DELAY/10
#define FAN_CNT FAN_DELAY/10

//Predefined fan speeds for each state for bare bones
#define GLV_ON_FAN_SPEED 0x00 //0%
#define HV_EN_FAN_SPEED (0xFF)>>1 //50%
#define RTD_FAN_SPEED 0xFF //100%
#define ERROR_FAN_SPEED 0x00 //0%
#define FAN_OFF 0x00 // 0%



void update_cooling(uint8_t new_cooling_state, uint8_t fan_speed);
void set_fan_speed(uint8_t duty_cycle);
void init_fan();



#endif /* COOLING_H_ */