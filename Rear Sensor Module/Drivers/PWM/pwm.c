/*
 * pwm.c
 * 
 * Copyright (c) Carnegie Mellon Racing 2016
 */ 

#include "pwm.h"


void pwmInit(uint8_t port, PWM_CLOCK_DIV_t prescaler){
		
		switch(port){
			case PWM_0A:
			case PWM_0B:
				TCCR0A |= (1<<WGM01)|(1<<WGM00)|(1<<COM0A1);
				TCCR0B = (TCCR0B & 0xF8) | (prescaler & 0x07);
				break;
			case PWM_1A:
			case PWM_1B:
				TCCR1A |= (1<<WGM11)|(1<<WGM10)|(1<<COM1A1);
				TCCR1B = (TCCR1B & 0xF8) | (prescaler & 0x07);
				break;
		}
}

void pwmSetDutyCycle(uint8_t port, uint16_t duty){
	
	switch(port){
		case PWM_0A:
			OCR0A = (uint8_t) duty;
			break;
		case PWM_0B:
			OCR0B = (uint8_t) duty;
			break;
		case PWM_1A:
			OCR1A = duty;
			break;
		case PWM_1B:
			OCR1B = duty;
			break;
		
	}
}


void pwmDisable(uint8_t port){
	
	switch(port){
		case PWM_0A:
		case PWM_0B:
		TCCR0A &= ~((1<<WGM01)|(1<<WGM00)|(1<<COM0A1));
		break;
		case PWM_1A:
		case PWM_1B:
		TCCR1A &= ~((1<<WGM11)|(1<<WGM10)|(1<<COM1A1));
		break;
	}
}
