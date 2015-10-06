/*
 * Cooling.c
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

#include "cooling.h"
#include "cmr_library_64c1.h"
#include "config.h"

//Cooling system state
uint8_t cooling_state = COOLING_DISABLE;
uint8_t cooling_delay_flag = 0;
uint8_t cooling_delay_cnt = 0;


void init_fan(){
	/* Set Timer/Counter Control Register A – TCCR0A
	 *	Update of OCRx at TOP, TOV set on MAX, FAST PWM
			WGM02, WGM01, WGM00 = 0,1,1
	 *		
	 *	Clear OC0A on compare match, set OC0A at TOP
	 *		COM0A1, COM0A0 = 1, 0(default) set to FAST PWM Mode
		
	*/
	TCCR0A |= (1<<WGM01)|(1<<WGM00)|(1<<COM0A1);
	/* Set Timer/Counter Control Register B – TCCR0B
	 *	WGM02 = 0 (default)
		Clock Select, clkI/O/1 (from prescaler)
			CS02:0 = 0,0,1
	*/
	
	TCCR0B |= (1 << CS00);
	
}
void set_fan_speed(uint8_t duty_cycle){
	OCR0A = duty_cycle; 
}

void update_cooling(uint8_t new_cooling_state, uint8_t fan_speed){
	set_fan_speed(fan_speed); 
	//Switch depending on whether the cooling system is to be updated to enabled or disabled
	switch(new_cooling_state){
		
		case COOLING_ENABLE:
		
			//Cooling system is already on
			if (cooling_state == COOLING_ENABLE){
				//Ensure fan and pumps are still on (redundant)
				//Enable fan
				set_io_pin(FAN_ENABLE_PORT, FAN_ENABLE_PIN, HIGH);
				//Set fan speed
				
				//Turn on pumps
				set_io_pin(PUMP_L_CTRL_PORT, PUMP_L_CTRL_PIN, HIGH);
				set_io_pin(PUMP_R_CTRL_PORT, PUMP_R_CTRL_PIN, HIGH);
			}
			
			//Cooling system is currently off, set delay to turn on. 
			else{
				//Initialize the counter if this is the first loop 
				// for the delay
				if (cooling_delay_flag == 0){
					cooling_delay_flag = 1;
					cooling_delay_cnt = 0;
				}
				//Turn Left pump on if delay is met
				if(cooling_delay_cnt > PUMP_L_CNT) 
					set_io_pin(PUMP_L_CTRL_PORT, PUMP_L_CTRL_PIN, HIGH);
				//Turn Right pump on if delay is met
				if(cooling_delay_cnt > PUMP_R_CNT) 
					set_io_pin(PUMP_R_CTRL_PORT, PUMP_R_CTRL_PIN, HIGH);
				//Turn Fan on and end delayed turn on
				if(cooling_delay_cnt > FAN_CNT){
					//Turn fan on
					//set_fan_speed(50) 15e_todo set fan speed
					set_io_pin(FAN_ENABLE_PORT, FAN_ENABLE_PIN, HIGH);
					//Reset delay counter and flag
					cooling_delay_flag = 0;
					cooling_delay_cnt = 0;
					cooling_state = COOLING_ENABLE;
				}
				//Delayed cooling is not done, so increment count
				else{
					//Increment delay counter
					cooling_delay_cnt++;
				}
				
			}
			
			break;
			
		case COOLING_DISABLE:
			//Fall into default case, all off
			
		default:
			//Reset cooling state variables
			cooling_delay_flag = 0;
			cooling_delay_cnt = 0;
			cooling_state = COOLING_DISABLE;
			
			//Turn fan off
			set_io_pin(FAN_ENABLE_PORT, FAN_ENABLE_PIN, LOW);
			
			//Turn pumps off
			set_io_pin(PUMP_L_CTRL_PORT, PUMP_L_CTRL_PIN, LOW);
			set_io_pin(PUMP_R_CTRL_PORT, PUMP_R_CTRL_PIN, LOW);
			break;
			
	}
}