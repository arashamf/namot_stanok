/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gpio.h
  * @brief   This file contains all the function prototypes for
  *          the gpio.c file
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __GPIO_H__
#define __GPIO_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */
#define DRIVE1_ENABLE(x) ((x)? (LL_GPIO_ResetOutputPin (ENABLE1_GPIO_Port, ENABLE1_Pin)) : (LL_GPIO_SetOutputPin(ENABLE1_GPIO_Port, ENABLE1_Pin)))
#define DRIVE2_ENABLE(x) ((x)? (LL_GPIO_ResetOutputPin (ENABLE2_GPIO_Port, ENABLE2_Pin)) : (LL_GPIO_SetOutputPin(ENABLE2_GPIO_Port, ENABLE2_Pin)))

#define CS(x) ((x)? (LL_GPIO_ResetOutputPin (CS_GPIO_Port, CS_Pin)) : (LL_GPIO_SetOutputPin(CS_GPIO_Port, CS_Pin)))
#define LCD_DC(x) ((x)? (LL_GPIO_SetOutputPin (DC_GPIO_Port, DC_Pin)) : (LL_GPIO_ResetOutputPin(DC_GPIO_Port, DC_Pin)))
#define LCD_RESET(x) ((x)? (LL_GPIO_SetOutputPin (RST_GPIO_Port, RST_Pin)) : (LL_GPIO_ResetOutputPin(RST_GPIO_Port, RST_Pin)))

#define DIR_DRIVE1(x) ((x)? (LL_GPIO_SetOutputPin (DIR1_GPIO_Port, DIR1_Pin)) : (LL_GPIO_ResetOutputPin(DIR1_GPIO_Port, DIR1_Pin)))
#define DIR_DRIVE2(x) ((x)? (LL_GPIO_SetOutputPin (DIR2_GPIO_Port, DIR2_Pin)) : (LL_GPIO_ResetOutputPin(DIR2_GPIO_Port, DIR2_Pin)))
/* USER CODE END Private defines */

void MX_GPIO_Init(void);

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__ GPIO_H__ */

