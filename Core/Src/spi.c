/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    spi.c
  * @brief   This file provides code for the configuration
  *          of the SPI instances.
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
#include "spi.h"

/* USER CODE BEGIN 0 */
#include "gpio.h"
#include "ssd1306.h"
#include "typedef.h"

uint8_t count = 0;
/* USER CODE END 0 */

/* SPI2 init function */
void MX_SPI2_Init(void)
{

  /* USER CODE BEGIN SPI2_Init 0 */

  /* USER CODE END SPI2_Init 0 */

  LL_SPI_InitTypeDef SPI_InitStruct = {0};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* Peripheral clock enable */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_SPI2);

  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOB);
  /**SPI2 GPIO Configuration
  PB13   ------> SPI2_SCK
  PB15   ------> SPI2_MOSI
  */
  GPIO_InitStruct.Pin = DISP_SCK_Pin|DISP_MOSI_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* SPI2 DMA Init */

  /* SPI2_TX Init */
  LL_DMA_SetDataTransferDirection(DMA1, LL_DMA_CHANNEL_5, LL_DMA_DIRECTION_MEMORY_TO_PERIPH);

  LL_DMA_SetChannelPriorityLevel(DMA1, LL_DMA_CHANNEL_5, LL_DMA_PRIORITY_LOW);

  LL_DMA_SetMode(DMA1, LL_DMA_CHANNEL_5, LL_DMA_MODE_NORMAL);

  LL_DMA_SetPeriphIncMode(DMA1, LL_DMA_CHANNEL_5, LL_DMA_PERIPH_NOINCREMENT);

  LL_DMA_SetMemoryIncMode(DMA1, LL_DMA_CHANNEL_5, LL_DMA_MEMORY_INCREMENT);

  LL_DMA_SetPeriphSize(DMA1, LL_DMA_CHANNEL_5, LL_DMA_PDATAALIGN_BYTE);

  LL_DMA_SetMemorySize(DMA1, LL_DMA_CHANNEL_5, LL_DMA_MDATAALIGN_BYTE);

  /* USER CODE BEGIN SPI2_Init 1 */

  /* USER CODE END SPI2_Init 1 */
  SPI_InitStruct.TransferDirection = LL_SPI_FULL_DUPLEX;
  SPI_InitStruct.Mode = LL_SPI_MODE_MASTER;
  SPI_InitStruct.DataWidth = LL_SPI_DATAWIDTH_8BIT;
  SPI_InitStruct.ClockPolarity = LL_SPI_POLARITY_LOW;
  SPI_InitStruct.ClockPhase = LL_SPI_PHASE_1EDGE;
  SPI_InitStruct.NSS = LL_SPI_NSS_SOFT;
  SPI_InitStruct.BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV4;
  SPI_InitStruct.BitOrder = LL_SPI_MSB_FIRST;
  SPI_InitStruct.CRCCalculation = LL_SPI_CRCCALCULATION_DISABLE;
  SPI_InitStruct.CRCPoly = 10;
  LL_SPI_Init(SPI2, &SPI_InitStruct);
  /* USER CODE BEGIN SPI2_Init 2 */
	LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_5);
  LL_DMA_ClearFlag_TC5(DMA1);
  LL_DMA_ClearFlag_TE5(DMA1); 
  LL_SPI_EnableDMAReq_TX(SPI2); //коммутация SPI2 с каналом 5 DMA
  LL_DMA_EnableIT_TC(DMA1, LL_DMA_CHANNEL_5); //Enable Transfer complete interrupt
  LL_DMA_EnableIT_TE(DMA1, LL_DMA_CHANNEL_5); //Enable Transfer error interrupt
  LL_SPI_Enable(SPI2); //enable SPI2
  /* USER CODE END SPI2_Init 2 */

}

/* USER CODE BEGIN 1 */
//-------------------------------------------------------------------------------------//
void spi_write_byte (uint8_t byte)
{	
  while(!LL_SPI_IsActiveFlag_TXE(SPI2)) {}
  LL_SPI_TransmitData8 (SPI2, (uint8_t)byte);
 // while(!LL_SPI_IsActiveFlag_RXNE(SPI2)) {}
//	(void) SPI2->DR;
}

//-------------------------------------------------------------------------------------//
void spi_write_buffer (uint8_t * data, uint16_t data_size)
{	
	for (uint8_t count = 0; count < data_size; count++)
	{	
		while(!LL_SPI_IsActiveFlag_TXE(SPI2)) {}
		LL_SPI_TransmitData8 (SPI2, (uint8_t)(*(data + count)));
//		while(!LL_SPI_IsActiveFlag_RXNE(SPI2)) {}
//		(void) SPI2->DR;
	}
}

//-------------------------------------------------------------------------------------//
void spi_write_buffer_DMA (void)
{	
	LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_5);
	LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_5, SSD1306_BUFFER_SIZE/4);
	LL_DMA_ConfigAddresses(DMA1, LL_DMA_CHANNEL_5, (uint32_t)&SSD1306_Buffer[(SSD1306_X_SIZE*count) - 4], 
	LL_SPI_DMA_GetRegAddr(SPI2), LL_DMA_GetDataTransferDirection(DMA1, LL_DMA_CHANNEL_5));  //Get the data register address used for DMA transfer
	if (count < 4)
	{
		SSD1306_PageAdress(count);
		LCD_DC(ON);
		LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_5);
		count++;
	}	
	else
	{
		count = 0;
		LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_5);
		CS(OFF);
	}
}

//-------------------------------------------------------------------------------------//
void DMA1_Channel5_Callback(void)
{
	LL_DMA_ClearFlag_GI5(DMA1); // Clear Channel 5 global interrupt flag
	spi_write_buffer_DMA ();
}

/* USER CODE END 1 */
