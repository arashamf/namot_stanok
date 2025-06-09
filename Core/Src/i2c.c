/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    i2c.c
  * @brief   This file provides code for the configuration
  *          of the I2C instances.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "i2c.h"

/* USER CODE BEGIN 0 */
#include "typedef.h"
#include "usart.h"

#define EEPROM_I2C I2C1

static uint8_t I2C_WriteAdress (uint16_t );
/* USER CODE END 0 */

/* I2C1 init function */
void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  LL_I2C_InitTypeDef I2C_InitStruct = {0};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOB);
  /**I2C1 GPIO Configuration
  PB8   ------> I2C1_SCL
  PB9   ------> I2C1_SDA
  */
  GPIO_InitStruct.Pin = EEPROM_SCL_Pin|EEPROM_SDA_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
  LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  LL_GPIO_AF_EnableRemap_I2C1();

  /* Peripheral clock enable */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C1);

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */

  /** I2C Initialization
  */
  LL_I2C_DisableOwnAddress2(I2C1);
  LL_I2C_DisableGeneralCall(I2C1);
  LL_I2C_EnableClockStretching(I2C1);
  I2C_InitStruct.PeripheralMode = LL_I2C_MODE_I2C;
  I2C_InitStruct.ClockSpeed = 400000;
  I2C_InitStruct.DutyCycle = LL_I2C_DUTYCYCLE_2;
  I2C_InitStruct.OwnAddress1 = 0;
  I2C_InitStruct.TypeAcknowledge = LL_I2C_ACK;
  I2C_InitStruct.OwnAddrSize = LL_I2C_OWNADDRESS1_7BIT;
  LL_I2C_Init(I2C1, &I2C_InitStruct);
  LL_I2C_SetOwnAddress2(I2C1, 0);
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/* USER CODE BEGIN 1 */
//------------------------------------------------------------------------------//
static uint8_t I2C_WriteAdress (uint16_t reg_addr)
{
  LL_I2C_DisableBitPOS(EEPROM_I2C); //Disable Pos
	if (LL_I2C_IsActiveFlag_BUSY(EEPROM_I2C) == SET) return RESET;
  LL_I2C_AcknowledgeNextData(EEPROM_I2C, LL_I2C_ACK); //Prepare the generation of a ACKnowledge condition after the address receive match code or next received byte
	
  LL_I2C_GenerateStartCondition(EEPROM_I2C); //Generate a START condition
	while(!LL_I2C_IsActiveFlag_SB(EEPROM_I2C)){}; //Indicate the status of Start Bit (master mode)
  (void) EEPROM_I2C->SR1;   //read state
		
  LL_I2C_TransmitData8(EEPROM_I2C, SLAVE_OWN_ADDRESS | I2C_REQUEST_WRITE); //передача адреса микросхемы и бита write 
  while(!LL_I2C_IsActiveFlag_ADDR(EEPROM_I2C)){};
  LL_I2C_ClearFlag_ADDR(EEPROM_I2C);
		
  LL_I2C_TransmitData8(EEPROM_I2C, (uint8_t) (reg_addr>>8)); //передача старшего байта адреса регистра
  while(!LL_I2C_IsActiveFlag_TXE(EEPROM_I2C)){};
  LL_I2C_TransmitData8(EEPROM_I2C, (uint8_t) reg_addr); //передача младшего байта адреса регистра 
	while(!LL_I2C_IsActiveFlag_TXE(EEPROM_I2C)){};
	return 1;
}

//------------------------------------------------------------------------------//
uint8_t I2C_WriteBuffer (uint16_t reg_addr, uint8_t *buf, uint16_t bytes_count)
{
	if (I2C_WriteAdress (reg_addr) == RESET)
	{	return 0;	}
  for(uint16_t i=0; i<bytes_count; i++)
  {
    LL_I2C_TransmitData8(EEPROM_I2C, buf[i]);
    while(!LL_I2C_IsActiveFlag_TXE(EEPROM_I2C)){};
  }
  LL_I2C_GenerateStopCondition(EEPROM_I2C);
	return bytes_count;
}

//------------------------------------------------------------------------------//
uint8_t I2C_WriteByte (uint8_t byte, uint16_t reg_addr)
{
  if (I2C_WriteAdress (reg_addr) == RESET)
	{	return 0;	}		

  LL_I2C_TransmitData8(EEPROM_I2C, byte);
  while(!LL_I2C_IsActiveFlag_TXE(EEPROM_I2C)){};
  LL_I2C_GenerateStopCondition(EEPROM_I2C);
		
	return 1;
}

//------------------------------------------------------------------------------//
uint8_t I2C_ReadBuffer (uint16_t reg_addr, uint8_t *buf, uint16_t bytes_count)
{
  uint16_t i;
	
	if (I2C_WriteAdress (reg_addr) == RESET)
	{	return 0;	}	
	
  LL_I2C_GenerateStartCondition(EEPROM_I2C); //условие Start
  while(!LL_I2C_IsActiveFlag_SB(EEPROM_I2C)){};
  (void) EEPROM_I2C->SR1;
  LL_I2C_TransmitData8(EEPROM_I2C, SLAVE_OWN_ADDRESS | I2C_REQUEST_READ); //передача адреса микросхемы и бита read 
  while (!LL_I2C_IsActiveFlag_ADDR(EEPROM_I2C)){};
  LL_I2C_ClearFlag_ADDR(EEPROM_I2C);
		
  for(uint16_t i=0; i<bytes_count; i++)
  {
    if(i<(bytes_count-1))
    {
      while(!LL_I2C_IsActiveFlag_RXNE(EEPROM_I2C)){};
      buf[i] = LL_I2C_ReceiveData8(EEPROM_I2C);
    }
    else //если последний байт буффера
    {
      LL_I2C_AcknowledgeNextData(EEPROM_I2C, LL_I2C_NACK);
      LL_I2C_GenerateStopCondition(EEPROM_I2C); //условие Stop после получения текущего байта
      while(!LL_I2C_IsActiveFlag_RXNE(EEPROM_I2C)){};
      buf[i] = LL_I2C_ReceiveData8(EEPROM_I2C);
    }
  }	
	return bytes_count;
}

//------------------------------------------------------------------------------//
uint8_t I2C_ReadByte (uint16_t reg_addr)
{
	uint8_t byte;
	
	if (I2C_WriteAdress (reg_addr) == RESET)
	{	return 0;	}	
		
  LL_I2C_GenerateStartCondition(EEPROM_I2C); //условие Start
  while(!LL_I2C_IsActiveFlag_SB(EEPROM_I2C)){};
  (void) EEPROM_I2C->SR1;
  LL_I2C_TransmitData8(EEPROM_I2C, SLAVE_OWN_ADDRESS | I2C_REQUEST_READ); //передача адреса микросхемы и бита read 
  while (!LL_I2C_IsActiveFlag_ADDR(EEPROM_I2C)){};
	LL_I2C_ClearFlag_ADDR(EEPROM_I2C);
		
	LL_I2C_AcknowledgeNextData(EEPROM_I2C, LL_I2C_NACK);
	LL_I2C_GenerateStopCondition(EEPROM_I2C); //условие Stop после получения текущего байта
	while(!LL_I2C_IsActiveFlag_RXNE(EEPROM_I2C)){};
	byte = LL_I2C_ReceiveData8(EEPROM_I2C);	
		
	return byte;
}

/* USER CODE END 1 */
