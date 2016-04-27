/*
 * pwm.h
 * 
 * Copyright (c) Carnegie Mellon Racing 2016
 */ 

#include <stdint.h>
#include <avr/io.h>

#ifndef PWM_H_
#define PWM_H_

// Constants & Definitions
#define PWM_0A			0
#define PWM_0B			1
#define PWM_1A			2
#define PWM_1B			3
// Note: PWM_1A and PWM_1B compare to a 16-bit timer

typedef enum {
	PWM_CLOCK_DIV1    = 0x01,
	PWM_CLOCK_DIV8    = 0x02,
	PWM_CLOCK_DIV64   = 0x03,
	PWM_CLOCK_DIV256  = 0x04,
	PWM_CLOCK_DIV1024 = 0x05,
	PWM_CLOCK_EXT_NEG = 0x06,
	PWM_CLOCK_EXT_POS = 0x07
} PWM_CLOCK_DIV_t;

// Functions
void pwmInit(uint8_t port, PWM_CLOCK_DIV_t prescaler);
void pwmSetDutyCycle(uint8_t port, uint16_t duty);
void pwmDisable(uint8_t port);


#endif
