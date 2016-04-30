/*
 * LCD_SPI.c
 *
 *  Created on: Apr 25, 2016
 *      Author: Tom
 */

#include "stm32f4xx_hal.h"
#include "cmsis_os.h"
#include "LCD_SPI.h"

#include "mxconstants.h"

static SPI_HandleTypeDef hspi5;
static uint8_t Is_LCD_IO_Initialized = 0;
/******************************* SPI Routines *********************************/

/**
  * @brief  SPIx Bus initialization
  */
static void SPIx_Init(void)
{
  if(HAL_SPI_GetState(&hspi5) == HAL_SPI_STATE_RESET)
  {
    /* SPI configuration -----------------------------------------------------*/
	  hspi5.Instance = SPI5;
    /* SPI baudrate is set to 5.6 MHz (PCLK2/SPI_BaudRatePrescaler = 90/16 = 5.625 MHz)
       to verify these constraints:
       - ILI9341 LCD SPI interface max baudrate is 10MHz for write and 6.66MHz for read
       - l3gd20 SPI interface max baudrate is 10MHz for write/read
       - PCLK2 frequency is set to 90 MHz
    */
	  hspi5.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;

    /* On STM32F429I-Discovery, LCD ID cannot be read then keep a common configuration */
    /* for LCD and GYRO (SPI_DIRECTION_2LINES) */
    /* Note: To read a register a LCD, SPI_DIRECTION_1LINE should be set */
	  hspi5.Init.Direction      = SPI_DIRECTION_1LINE;
	  hspi5.Init.CLKPhase       = SPI_PHASE_1EDGE;
	  hspi5.Init.CLKPolarity    = SPI_POLARITY_LOW;
	  hspi5.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLED;
	  hspi5.Init.CRCPolynomial  = 7;
	  hspi5.Init.DataSize       = SPI_DATASIZE_8BIT;
	  hspi5.Init.FirstBit       = SPI_FIRSTBIT_MSB;
	  hspi5.Init.NSS            = SPI_NSS_SOFT;
	  hspi5.Init.TIMode         = SPI_TIMODE_DISABLED;
	  hspi5.Init.Mode           = SPI_MODE_MASTER;

    SPIx_MspInit(&hspi5);
    HAL_SPI_Init(&hspi5);
  }
}

/**
  * @brief  Reads 4 bytes from device.
  * @param  ReadSize: Number of bytes to read (max 4 bytes)
  * @retval Value read on the SPI
  */

static void LCD_CS_LOW() {
	  HAL_GPIO_WritePin(LCD_SPI_CS_Port, LCD_SPI_CS_Pin, GPIO_PIN_RESET);
}

static void LCD_CS_HIGH() {
	  HAL_GPIO_WritePin(LCD_SPI_CS_Port, LCD_SPI_CS_Pin, GPIO_PIN_SET);
}

static void LCD_WRX_LOW(){
	HAL_GPIO_WritePin(LCD_SPI_DCX_Port, LCD_SPI_DCX_Pin, GPIO_PIN_RESET);
}

static void LCD_WRX_HIGH(){
	HAL_GPIO_WritePin(LCD_SPI_DCX_Port, LCD_SPI_DCX_Pin, GPIO_PIN_SET);
}

static uint32_t SPIx_Read(uint8_t ReadSize)
{
  HAL_StatusTypeDef status = HAL_OK;
  uint32_t readvalue;

  status = HAL_SPI_Receive(&hspi5, (uint8_t*) &readvalue, ReadSize, SPI_TIMEOUT);

  /* Check the communication status */
  if(status != HAL_OK)
  {
    /* Re-Initialize the BUS */
    SPIx_Error();
  }

  return readvalue;
}

/**
  * @brief  Writes a byte to device.
  * @param  Value: value to be written
  */
static void SPIx_Write(uint16_t Value)
{
  HAL_StatusTypeDef status = HAL_OK;

  status = HAL_SPI_Transmit(&hspi5, (uint8_t*) &Value, 1, SPI_TIMEOUT);

  /* Check the communication status */
  if(status != HAL_OK)
  {
    /* Re-Initialize the BUS */
    SPIx_Error();
  }
}

/**
  * @brief  Sends a Byte through the SPI interface and return the Byte received
  *         from the SPI bus.
  * @param  Byte: Byte send.
  * @retval The received byte value
  */
static uint8_t SPIx_WriteRead(uint8_t Byte)
{
  uint8_t receivedbyte = 0;

  /* Send a Byte through the SPI peripheral */
  /* Read byte from the SPI bus */
  if(HAL_SPI_TransmitReceive(&hspi5, (uint8_t*) &Byte, (uint8_t*) &receivedbyte, 1, SPI_TIMEOUT) != HAL_OK)
  {
    SPIx_Error();
  }

  return receivedbyte;
}

/**
  * @brief  SPIx error treatment function.
  */
static void SPIx_Error(void)
{
  /* De-initialize the SPI communication BUS */
  HAL_SPI_DeInit(&hspi5);

  /* Re- Initialize the SPI communication BUS */
  SPIx_Init();
}

/**
  * @brief  SPI MSP Init.
  * @param  hspi: SPI handle
  */
static void SPIx_MspInit(SPI_HandleTypeDef *hspi)
{

  GPIO_InitTypeDef   GPIO_InitStructure;

  /* Enable SPIx clock */
  __SPI5_CLK_ENABLE();


  /* Enable DISCOVERY_SPI GPIO clock */
//  DISCOVERY_SPIx_GPIO_CLK_ENABLE();
  //done in main already

  /* configure SPI SCK, MOSI and MISO */


  /**SPI5 GPIO Configuration
  PF7     ------> SPI5_SCK
  PF9     ------> SPI5_MOSI
  */

  GPIO_InitStructure.Pin    = (LCD_SPI_SCL_Pin | LCD_SPI_SDA_Pin);
  GPIO_InitStructure.Mode   = GPIO_MODE_AF_PP;
  GPIO_InitStructure.Pull   = GPIO_PULLDOWN;
  GPIO_InitStructure.Speed  = GPIO_SPEED_MEDIUM;
  GPIO_InitStructure.Alternate = GPIO_AF5_SPI5;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStructure);
}

/********************************* LINK LCD ***********************************/

/**
  * @brief  Configures the LCD_SPI interface.
  */
void LCD_IO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  if(Is_LCD_IO_Initialized == 0)
  {
    Is_LCD_IO_Initialized = 1;



    /* Configure NCS in Output Push-Pull mode */
//    LCD_WRX_GPIO_CLK_ENABLE();
    //clock started in main.c already
    GPIO_InitStructure.Pin     = LCD_SPI_DCX_Pin;
    GPIO_InitStructure.Mode    = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Pull    = GPIO_NOPULL;
    GPIO_InitStructure.Speed   = GPIO_SPEED_FAST;
    HAL_GPIO_Init(LCD_SPI_DCX_Port, &GPIO_InitStructure);

//    LCD_RDX_GPIO_CLK_ENABLE();
//    GPIO_InitStructure.Pin     = LCD_RDX_PIN;
//    GPIO_InitStructure.Mode    = GPIO_MODE_OUTPUT_PP;
//    GPIO_InitStructure.Pull    = GPIO_NOPULL;
//    GPIO_InitStructure.Speed   = GPIO_SPEED_FAST;
//    HAL_GPIO_Init(LCD_RDX_GPIO_PORT, &GPIO_InitStructure);

    /* Configure the LCD Control pins ----------------------------------------*/
//    LCD_NCS_GPIO_CLK_ENABLE();

    /* Configure NCS in Output Push-Pull mode */
    //Done in main.c already
//    GPIO_InitStructure.Pin     = LCD_SPI_CS_Pin;
//    GPIO_InitStructure.Mode    = GPIO_MODE_OUTPUT_PP;
//    GPIO_InitStructure.Pull    = GPIO_NOPULL;
//    GPIO_InitStructure.Speed   = GPIO_SPEED_FAST;
//    HAL_GPIO_Init(LCD_NCS_GPIO_PORT, &GPIO_InitStructure);

    /* Set or Reset the control line */
    LCD_CS_LOW();
    LCD_CS_HIGH();

    SPIx_Init();



  }
}

/**
  * @brief  Writes register value.
  */
void LCD_IO_WriteData(uint16_t RegValue)
{
  /* Set WRX to send data */
  LCD_WRX_HIGH();

  /* Reset LCD control line(/CS) and Send data */
  LCD_CS_LOW();
  SPIx_Write(RegValue);

  /* Deselect: Chip Select high */
  LCD_CS_HIGH();
}

/**
  * @brief  Writes register address.
  */
void LCD_IO_WriteReg(uint8_t Reg)
{
  /* Reset WRX to send command */
  LCD_WRX_LOW();

  /* Reset LCD control line(/CS) and Send command */
  LCD_CS_LOW();
  SPIx_Write(Reg);

  /* Deselect: Chip Select high */
  LCD_CS_HIGH();
}

/**
  * @brief  Reads register value.
  * @param  RegValue Address of the register to read
  * @param  ReadSize Number of bytes to read
  * @retval Content of the register value
  */
uint32_t LCD_IO_ReadData(uint16_t RegValue, uint8_t ReadSize)
{
  uint32_t readvalue = 0;

  /* Select: Chip Select low */
  LCD_CS_LOW();

  /* Reset WRX to send command */
  LCD_WRX_LOW();

  SPIx_Write(RegValue);

  readvalue = SPIx_Read(ReadSize);

  /* Set WRX to send data */
  LCD_WRX_HIGH();

  /* Deselect: Chip Select high */
  LCD_CS_HIGH();

  return readvalue;
}

/**
  * @brief  Wait for loop in ms.
  * @param  Delay in ms.
  */
void LCD_Delay(uint32_t Delay)
{
  HAL_Delay(Delay);
}
