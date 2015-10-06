/*
 * dashboard_dio.h
 *
 * Created: 3/4/2015 8:41:05 PM
 *  Author: Robert
 */ 

#ifndef DASHBOARD_DIO_H_
#define DASHBOARD_DIO_H_

/* Sets up Direction registers for buttons, LEDs, and SPI on the dashboard. */
void set_DDRs();

/* Functions to read the state of button inputs.
 * These return 1 if the button is pressed, 0 otherwise.
 */
uint8_t LVE_btn_pressed();
uint8_t HVE_btn_pressed();
uint8_t RTD_btn_pressed();
uint8_t DAQ_btn_pressed();
uint8_t mode_btn_pressed();
uint8_t left_disp_btn_pressed();
uint8_t center_disp_btn_pressed();
uint8_t right_disp_btn_pressed();

/* SPI Slave Select functions. 
 * These set the state of each SPI pin, taking care of inverted logic.    */
void SPI_select_left_disp(uint8_t state);
void SPI_select_center_disp(uint8_t state);
void SPI_select_right_disp(uint8_t state);

#endif /* DASHBOARD_DIO_H_ */