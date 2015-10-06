/*
 * standard_dio.h
 *
 * Created: 3/4/2015 8:39:26 PM
 *  Author: Robert
 */ 


#ifndef STANDARD_DIO_H_
#define STANDARD_DIO_H_


#define sbi(x,y) x |= _BV(y)				 // set bit
#define cbi(x,y) x &= ~(_BV(y))				 // clear bit
#define tbi(x,y) x ^= _BV(y)				 // toggle bit
#define is_high(x,y) (x & _BV(y)) == _BV(y)  // (for input) checks if the input is high (Logic 1)
#define is_low(x,y) ((x & _BV(y)) == 0)      // check if the input pin is low


#endif /* STANDARD_DIO_H_ */