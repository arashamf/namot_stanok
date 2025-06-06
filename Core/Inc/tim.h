/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    tim.h
  * @brief   This file contains all the function prototypes for
  *          the tim.c file
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
#ifndef __TIM_H__
#define __TIM_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */
#include "typedef.h"
/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */
#define 	 MAX_xTIMERS 								        3


// TIM_PWM_Drive2 - управление верхним двигателем (намоточный)
#define 	TIM_PWM_Drive2 											TIM1
#define		Drive2_PWM_Channel									LL_TIM_CHANNEL_CH1 

// TIM_PWM_Drive1 - управление нижним двигателем (сдвигающий)
#define 	TIM_PWM_Drive1 											TIM3
#define		Drive1_PWM_Channel									LL_TIM_CHANNEL_CH2 

#define 	TIM_cnt_PWM_DRIVE2 									TIM4
#define		TIM_cnt_PWM_DRIVE2_IRQn							TIM4_IRQn
#define 	TIM_cnt_PWM_DRIVE2_IRQHandler 			TIM4_IRQHandler

#define 	TIM_cnt_PWM_DRIVE1 									TIM5
#define		TIM_cnt_PWM_DRIVE1_IRQn							TIM5_IRQn
#define 	TIM_cnt_PWM_DRIVE1_IRQHandler 			TIM5_IRQHandler

#define 	TIM_DELAY_us 										    TIM7
#define 	TIM_DELAY_us_APB1_BIT 					    LL_APB1_GRP1_PERIPH_TIM7

#define 	TIM_BOUNCE_DELAY 								    TIM6
#define 	TIM_BOUNCE_DELAY_APB1_BIT 			    LL_APB1_GRP1_PERIPH_TIM6
#define  	TIM_BOUNCE_DELAY_IRQn               TIM6_IRQn
#define		TIM_BOUNCE_DELAY_IRQHandler			    TIM6_IRQHandler

//Exported types -----------------------------------------------------------------//
typedef void * xTimerHandle; //указатель типа void на структуру таймера 
typedef void (*SysTick_CALLBACK)(uint32_t); //указатель типа void на ф-ю обработки (коллбэка) системного таймера
typedef void (*tmrTIMER_CALLBACK)(xTimerHandle xTimer);	//указатель на коллбэк программного таймера (вызывается из коллбэка системного таймера)
typedef uint32_t (*portTickType)(void); //указатель типа void на ф-ю возврата системного времени

//--------------------------------------------------------------------------------//
typedef enum 
{
	__IDLE      =   0 , 
	__ACTIVE          , 
	__DONE
} tmrTIMER_STATE;

//--------------------------------------------------------------------------------//
typedef struct
{
	uint32_t expiry;
  uint32_t periodic;
	tmrTIMER_STATE State;
	FunctionalState	AutoReload;
	tmrTIMER_CALLBACK	CallbackFunction;
} xTIMER;


/* USER CODE END Private defines */
//--------------------------------------------------------------------------------//
void MX_TIM1_Init(void);
void MX_TIM3_Init(void);
void MX_TIM4_Init(void);
void MX_TIM5_Init(void);

/* USER CODE BEGIN Prototypes */
void Drives_PWM_start(PWM_data_t * );
void Drive1_PWM_start(PWM_data_t * ) ;
void Drive1_PWM_repeat(void);
void Drive1_PWM_stop(void) ;

void Drive2_PWM_start (PWM_data_t * );
void Drive2_PWM_repeat(void);
void Drive2_PWM_stop(void);

void Stop_Count_Timers(void);
void delay_us(uint16_t );
void timers_ini (void);
void delay_ms(uint32_t delay);

xTimerHandle xTimer_Create(uint32_t xTimerPeriodInTicks, FunctionalState AutoReload, 
tmrTIMER_CALLBACK CallbackFunction, FunctionalState NewState);
void xTimer_SetPeriod(xTimerHandle , uint32_t );
void xTimer_Reload(xTimerHandle );
void xTimer_Delete(xTimerHandle );
void xTimer_Task(uint32_t portTick);
void xTimersButtonBounce_Reload (uint16_t );
void xTimerEncDelay_Reload (uint16_t );

//Variables -----------------------------------------------------------------------//


/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __TIM_H__ */

