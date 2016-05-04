/*
 * LCD_SPI.h
 *
 *  Created on: Apr 25, 2016
 *      Author: Tom
 */

#ifndef APPLICATION_USER_LCD_H_
#define APPLICATION_USER_LCD_H_

#define SPI_TIMEOUT 1000

void LCD_IO_Init(void);
static void SPIx_Init(void);
static uint32_t SPIx_Read(uint8_t ReadSize);
static void SPIx_Write(uint16_t Value);
static uint8_t SPIx_WriteRead(uint8_t Byte);
static void SPIx_Error(void);
static void SPIx_MspInit(SPI_HandleTypeDef *hspi);
static void LCD_CS_LOW();
static void LCD_CS_HIGH();
static void LCD_WRX_LOW();
static void LCD_WRX_HIGH();
void LCD_Delay(uint32_t Delay);
void WriteParameter(int16_t value);
void WriteCommand(int16_t value);
void LCDResetLow(void);
void LCDResetHigh(void);




#define LCD_SPI_SCL_Pin GPIO_PIN_7
#define LCD_SPI_SCL_Port GPIOF

#define LCD_SPI_CS_Pin GPIO_PIN_8
#define LCD_SPI_CS_Port GPIOF

#define LCD_SPI_SDA_Pin GPIO_PIN_9
#define LCD_SPI_SDA_Port GPIOF

#define LCD_SPI_DCX_Pin GPIO_PIN_13
#define LCD_SPI_DCX_Port GPIOC

#define LCD_RESET_Pin GPIO_PIN_6
#define LCD_RESET_Port GPIOE

#endif /* APPLICATION_USER_LCD_H_ */
