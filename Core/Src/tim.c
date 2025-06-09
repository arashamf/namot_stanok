/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    tim.c
  * @brief   This file provides code for the configuration
  *          of the TIM instances.
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
#include "tim.h"

/* USER CODE BEGIN 0 */
#include "typedef.h"
#include "usart.h"
#include "drive.h"
#include "gpio.h"
#include "encoder.h"
#include "button.h"

// Defines ---------------------------------------------------------------------//
#define 	PERIODIC 										1
#define 	ONCE 												0
#define   NULL                        0

// Variables --------------------------------------------------------------------//
static portTickType xTimeNow; //указатель на ф-ю возврата системного времени вида uint32_t Get_SysTick(void)
static SysTick_CALLBACK SysTick_CallbackFunction = NULL; //инициализация указателя на ф-ю обработки (коллбэка) системного таймера
static uint32_t TicksCounter = 0; //счётчик системных тиков

static xTIMER xTimerList[MAX_xTIMERS]; //массив структур программных таймеров
static xTimerHandle xTimerBtnBounce;
static xTimerHandle xTimerEncDelay;

// Functions --------------------------------------------------------------------//
static void vTimerBtnBounceCallback(xTimerHandle xTimer); 
static void vTimerEncDelayCallback(xTimerHandle xTimer); 
static void tim_delay_init (void);
static void timer_bounce_init (void);
static uint32_t Get_SysTick(void);

/* USER CODE END 0 */

/* TIM1 init function */
void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  LL_TIM_InitTypeDef TIM_InitStruct = {0};
  LL_TIM_OC_InitTypeDef TIM_OC_InitStruct = {0};
  LL_TIM_BDTR_InitTypeDef TIM_BDTRInitStruct = {0};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* Peripheral clock enable */
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM1);

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  TIM_InitStruct.Prescaler = 71;
  TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
  TIM_InitStruct.Autoreload = 999;
  TIM_InitStruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
  TIM_InitStruct.RepetitionCounter = 255;
  LL_TIM_Init(TIM1, &TIM_InitStruct);
  LL_TIM_DisableARRPreload(TIM1);
  LL_TIM_SetClockSource(TIM1, LL_TIM_CLOCKSOURCE_INTERNAL);
  LL_TIM_OC_EnablePreload(TIM1, LL_TIM_CHANNEL_CH1);
  TIM_OC_InitStruct.OCMode = LL_TIM_OCMODE_PWM2;
  TIM_OC_InitStruct.OCState = LL_TIM_OCSTATE_DISABLE;
  TIM_OC_InitStruct.OCNState = LL_TIM_OCSTATE_DISABLE;
  TIM_OC_InitStruct.CompareValue = 499;
  TIM_OC_InitStruct.OCPolarity = LL_TIM_OCPOLARITY_HIGH;
  TIM_OC_InitStruct.OCNPolarity = LL_TIM_OCPOLARITY_HIGH;
  TIM_OC_InitStruct.OCIdleState = LL_TIM_OCIDLESTATE_LOW;
  TIM_OC_InitStruct.OCNIdleState = LL_TIM_OCIDLESTATE_LOW;
  LL_TIM_OC_Init(TIM1, LL_TIM_CHANNEL_CH1, &TIM_OC_InitStruct);
  LL_TIM_OC_DisableFast(TIM1, LL_TIM_CHANNEL_CH1);
  LL_TIM_SetTriggerOutput(TIM1, LL_TIM_TRGO_OC1REF);
  LL_TIM_EnableMasterSlaveMode(TIM1);
  TIM_BDTRInitStruct.OSSRState = LL_TIM_OSSR_DISABLE;
  TIM_BDTRInitStruct.OSSIState = LL_TIM_OSSI_DISABLE;
  TIM_BDTRInitStruct.LockLevel = LL_TIM_LOCKLEVEL_OFF;
  TIM_BDTRInitStruct.DeadTime = 0;
  TIM_BDTRInitStruct.BreakState = LL_TIM_BREAK_DISABLE;
  TIM_BDTRInitStruct.BreakPolarity = LL_TIM_BREAK_POLARITY_HIGH;
  TIM_BDTRInitStruct.AutomaticOutput = LL_TIM_AUTOMATICOUTPUT_DISABLE;
  LL_TIM_BDTR_Init(TIM1, &TIM_BDTRInitStruct);
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);
    /**TIM1 GPIO Configuration
    PA8     ------> TIM1_CH1
    */
  GPIO_InitStruct.Pin = PUL_DRIVE2_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  LL_GPIO_Init(PUL_DRIVE2_GPIO_Port, &GPIO_InitStruct);

}
/* TIM3 init function */
void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  LL_TIM_InitTypeDef TIM_InitStruct = {0};
  LL_TIM_OC_InitTypeDef TIM_OC_InitStruct = {0};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* Peripheral clock enable */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM3);

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  TIM_InitStruct.Prescaler = 71;
  TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
  TIM_InitStruct.Autoreload = 999;
  TIM_InitStruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
  LL_TIM_Init(TIM3, &TIM_InitStruct);
  LL_TIM_DisableARRPreload(TIM3);
  LL_TIM_OC_EnablePreload(TIM3, LL_TIM_CHANNEL_CH2);
  TIM_OC_InitStruct.OCMode = LL_TIM_OCMODE_PWM2;
  TIM_OC_InitStruct.OCState = LL_TIM_OCSTATE_DISABLE;
  TIM_OC_InitStruct.OCNState = LL_TIM_OCSTATE_DISABLE;
  TIM_OC_InitStruct.CompareValue = 499;
  TIM_OC_InitStruct.OCPolarity = LL_TIM_OCPOLARITY_HIGH;
  LL_TIM_OC_Init(TIM3, LL_TIM_CHANNEL_CH2, &TIM_OC_InitStruct);
  LL_TIM_OC_DisableFast(TIM3, LL_TIM_CHANNEL_CH2);
  LL_TIM_SetTriggerOutput(TIM3, LL_TIM_TRGO_OC2REF);
  LL_TIM_EnableMasterSlaveMode(TIM3);
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);
    /**TIM3 GPIO Configuration
    PA7     ------> TIM3_CH2
    */
  GPIO_InitStruct.Pin = PUL_DIR1_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  LL_GPIO_Init(PUL_DIR1_GPIO_Port, &GPIO_InitStruct);

}
/* TIM4 init function */
void MX_TIM4_Init(void)
{

  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  LL_TIM_InitTypeDef TIM_InitStruct = {0};

  /* Peripheral clock enable */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM4);

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  TIM_InitStruct.Prescaler = 0;
  TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
  TIM_InitStruct.Autoreload = 1599;
  TIM_InitStruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
  LL_TIM_Init(TIM4, &TIM_InitStruct);
  LL_TIM_DisableARRPreload(TIM4);
  LL_TIM_SetTriggerInput(TIM4, LL_TIM_TS_ITR0);
  LL_TIM_SetClockSource(TIM4, LL_TIM_CLOCKSOURCE_EXT_MODE1);
  LL_TIM_DisableIT_TRIG(TIM4);
  LL_TIM_DisableDMAReq_TRIG(TIM4);
  LL_TIM_SetTriggerOutput(TIM4, LL_TIM_TRGO_RESET);
  LL_TIM_DisableMasterSlaveMode(TIM4);
  /* USER CODE BEGIN TIM4_Init 2 */
	LL_TIM_ClearFlag_UPDATE(TIM_cnt_PWM_DRIVE2); 		//сброс флага обновления таймера
	LL_TIM_EnableIT_UPDATE(TIM_cnt_PWM_DRIVE2); 		//Enable update interrupt
	NVIC_SetPriority(TIM_cnt_PWM_DRIVE2_IRQn, 1); 	//приоритет прерывания
  NVIC_EnableIRQ(TIM_cnt_PWM_DRIVE2_IRQn);	
	LL_TIM_SetCounter(TIM_cnt_PWM_DRIVE2, 0); 			//сброс счётного регистра
  /* USER CODE END TIM4_Init 2 */

}
/* TIM5 init function */
void MX_TIM5_Init(void)
{

  /* USER CODE BEGIN TIM5_Init 0 */

  /* USER CODE END TIM5_Init 0 */

  LL_TIM_InitTypeDef TIM_InitStruct = {0};

  /* Peripheral clock enable */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM5);

  /* USER CODE BEGIN TIM5_Init 1 */

  /* USER CODE END TIM5_Init 1 */
  TIM_InitStruct.Prescaler = 0;
  TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
  TIM_InitStruct.Autoreload = 199;
  TIM_InitStruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
  LL_TIM_Init(TIM5, &TIM_InitStruct);
  LL_TIM_DisableARRPreload(TIM5);
  LL_TIM_SetTriggerInput(TIM5, LL_TIM_TS_ITR1);
  LL_TIM_SetClockSource(TIM5, LL_TIM_CLOCKSOURCE_EXT_MODE1);
  LL_TIM_DisableIT_TRIG(TIM5);
  LL_TIM_DisableDMAReq_TRIG(TIM5);
  LL_TIM_SetTriggerOutput(TIM5, LL_TIM_TRGO_RESET);
  LL_TIM_DisableMasterSlaveMode(TIM5);
  /* USER CODE BEGIN TIM5_Init 2 */
	LL_TIM_ClearFlag_UPDATE(TIM_cnt_PWM_DRIVE1); 		//сброс флага обновления таймера
	LL_TIM_EnableIT_UPDATE(TIM_cnt_PWM_DRIVE1); 		//Enable update interrupt
	NVIC_SetPriority(TIM_cnt_PWM_DRIVE1_IRQn, 1); 	//приоритет прерывания
  NVIC_EnableIRQ(TIM_cnt_PWM_DRIVE1_IRQn);	
	LL_TIM_SetCounter(TIM_cnt_PWM_DRIVE1, 0); 			//сброс счётного регистра
  /* USER CODE END TIM5_Init 2 */

}

/* USER CODE BEGIN 1 */
//---------------------------------------------------------------------------------------//
void Drives_PWM_start(PWM_data_t * PWM_data) 
{
	LL_TIM_DisableCounter(TIM_cnt_PWM_DRIVE1);
	LL_TIM_SetAutoReload(TIM_cnt_PWM_DRIVE1, PWM_data->number_cnt_PWM_DR1);
	LL_TIM_SetCounter(TIM_cnt_PWM_DRIVE1, 0); 		//сброс счётного регистра
	LL_TIM_EnableCounter(TIM_cnt_PWM_DRIVE1); 		//включение таймера	
	
	LL_TIM_DisableCounter(TIM_cnt_PWM_DRIVE2);
	LL_TIM_SetCounter(TIM_cnt_PWM_DRIVE2, 0); 		//сброс счётного регистра
	LL_TIM_EnableCounter(TIM_cnt_PWM_DRIVE2); 		//включение таймера	*/
	
	LL_TIM_DisableCounter(TIM_PWM_Drive1); //выключение таймера, управляющий верхним двигателем (намоточным)
	LL_TIM_DisableCounter(TIM_PWM_Drive2); //выключение таймера, управляющий нижнем двигателем (сдвигающем)
	LL_TIM_SetCounter(TIM_PWM_Drive1, 0);
	LL_TIM_SetCounter(TIM_PWM_Drive2, 0);
	LL_TIM_SetAutoReload(TIM_PWM_Drive1, PWM_data-> Period_Drive1); 	//Set the Autoreload value 
	LL_TIM_SetAutoReload(TIM_PWM_Drive2, PWM_data-> Period_Drive2); 	//Set the Autoreload value 
	LL_TIM_OC_SetCompareCH2(TIM_PWM_Drive1, PWM_data->Compare_Drive1); //Set compare value for output channel 
	LL_TIM_OC_SetCompareCH1(TIM_PWM_Drive2, PWM_data->Compare_Drive2); //Set compare value for output channel 
	LL_TIM_CC_EnableChannel(TIM_PWM_Drive1,  Drive1_PWM_Channel); //включение канала ШИМ таймера
	LL_TIM_CC_EnableChannel(TIM_PWM_Drive2,  Drive2_PWM_Channel); //включение канала ШИМ таймера
	if (TIM_PWM_Drive1 == TIM1)
	{	LL_TIM_EnableAllOutputs(TIM_PWM_Drive1);	}	//включение таймера  для генерации ШИМ
	
	if (TIM_PWM_Drive2 == TIM1)
	{	LL_TIM_EnableAllOutputs(TIM_PWM_Drive2);	}//включение таймера  для генерации ШИМ

	LL_TIM_EnableCounter(TIM_PWM_Drive1); 		//Enable timer counter
	LL_TIM_EnableCounter(TIM_PWM_Drive2); 
}

//---------------------------------------------------------------------------------------//
void Drive1_PWM_start(PWM_data_t * PWM_data) 
{	
	LL_TIM_DisableCounter(TIM_PWM_Drive1); //выключение таймера, управляющий нижнем двигателем (сдвигающем)
	LL_TIM_SetCounter(TIM_PWM_Drive1, 0);
	LL_TIM_SetAutoReload(TIM_PWM_Drive1, PWM_data->Period_Drive1);
	LL_TIM_OC_SetCompareCH2(TIM_PWM_Drive1,	PWM_data->Compare_Drive1);
	LL_TIM_CC_EnableChannel(TIM_PWM_Drive1,  Drive1_PWM_Channel); //включение канала ШИМ таймера
	if (TIM_PWM_Drive1 == TIM1)
	{
		LL_TIM_EnableAllOutputs(TIM_PWM_Drive1);	//включение таймера  для генерации ШИМ
	}
	LL_TIM_EnableCounter(TIM_PWM_Drive1); 		//Enable timer counter
}

//---------------------------------------------------------------------------------------//
void Drive1_PWM_repeat(void) 
{
	LL_TIM_SetCounter(TIM_PWM_Drive1, 0);
	LL_TIM_EnableCounter(TIM_PWM_Drive1); //включение таймера  для генерации ШИМ двигателя 1
}

//---------------------------------------------------------------------------------------//
void Drive1_PWM_stop(void) 
{
		LL_TIM_DisableCounter(TIM_PWM_Drive1); //выключение таймера
		LL_TIM_CC_DisableChannel(TIM_PWM_Drive1, Drive1_PWM_Channel ); //выключение таймера  для генерации ШИМ двигателя 1
}

//---------------------------------------------------------------------------------------//
void TIM_cnt_PWM_DRIVE1_IRQHandler(void)
{
	if (LL_TIM_IsActiveFlag_UPDATE(TIM_cnt_PWM_DRIVE1) == SET)
	{
		LL_TIM_ClearFlag_UPDATE (TIM_cnt_PWM_DRIVE1);
		Counter_PWM_Drive1_Callback ();
	}
}

//---------------------------------------------------------------------------------------//
void Drive2_PWM_start(PWM_data_t * PWM_data) 
{
	LL_TIM_DisableCounter(TIM_PWM_Drive2); //выключение таймера
	LL_TIM_SetCounter(TIM_PWM_Drive2, 0);
	LL_TIM_SetAutoReload(TIM_PWM_Drive2, PWM_data->Period_Drive2); //Set the Autoreload value
	LL_TIM_OC_SetCompareCH1(TIM_PWM_Drive2, PWM_data->Compare_Drive2); //Set compare value for output channel 1 (TIMx_CCR2)
	LL_TIM_CC_EnableChannel(TIM_PWM_Drive2, Drive2_PWM_Channel); //включение канала 1 таймера
  LL_TIM_EnableAllOutputs(TIM_PWM_Drive2);	//включение таймера для генерации ШИМ
	LL_TIM_EnableCounter(TIM_PWM_Drive2); //Enable timer counter
}

//---------------------------------------------------------------------------------------//
void Drive2_PWM_repeat(void) 
{
	LL_TIM_SetCounter(TIM_PWM_Drive2, 0);
	LL_TIM_EnableCounter(TIM_PWM_Drive2); //включение таймера  для генерации ШИМ
	LL_TIM_EnableAllOutputs(TIM_PWM_Drive2);	//включение каналов таймера 
}

//---------------------------------------------------------------------------------------//
void Drive2_PWM_stop(void) 
{
	LL_TIM_DisableCounter(TIM_PWM_Drive2); //выключение таймера
	LL_TIM_DisableAllOutputs(TIM_PWM_Drive2); //
	LL_TIM_CC_DisableChannel(TIM_PWM_Drive2, Drive2_PWM_Channel );
}

//---------------------------------------------------------------------------------------//
void Stop_Count_Timers(void)
{
	LL_TIM_DisableCounter(TIM_cnt_PWM_DRIVE1);
	LL_TIM_DisableCounter(TIM_cnt_PWM_DRIVE2);
}	

//---------------------------------------------------------------------------------------//
void TIM_cnt_PWM_DRIVE2_IRQHandler(void)
{
	if (LL_TIM_IsActiveFlag_UPDATE(TIM_cnt_PWM_DRIVE2) == SET)
	{
		LL_TIM_ClearFlag_UPDATE (TIM_cnt_PWM_DRIVE2);
		Counter_PWM_Drive2_Callback ();
	}
}

//---------------------------------------------------------------------------------------//
static void tim_delay_init (void)
{
	LL_TIM_InitTypeDef TIM_InitStruct = {0};

  LL_APB1_GRP1_EnableClock(TIM_DELAY_us_APB1_BIT );   // Peripheral clock enable 

  TIM_InitStruct.Prescaler = (uint16_t)((CPU_CLOCK_VALUE/1000000)-1); //предделитель 72МГц/72=1МГц
  TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
  TIM_InitStruct.Autoreload = 0xFF;
  TIM_InitStruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
  LL_TIM_Init(TIM_DELAY_us, &TIM_InitStruct);
  LL_TIM_DisableARRPreload(TIM_DELAY_us);
}

//---------------------------------------------------------------------------------------//
void delay_us(uint16_t delay)
{
  LL_TIM_SetAutoReload(TIM_DELAY_us, delay); //установка задержки в мкс
	LL_TIM_ClearFlag_UPDATE(TIM_DELAY_us); //сброс флага обновления таймера
	LL_TIM_SetCounter(TIM_DELAY_us, 0); //сброс счётного регистра
	LL_TIM_EnableCounter(TIM_DELAY_us); //включение таймера
	while (LL_TIM_IsActiveFlag_UPDATE(TIM_DELAY_us) == 0) {} //ожидание установки флага обновления таймера 
	LL_TIM_DisableCounter(TIM_DELAY_us); //выключение таймера		
}

//---------------------------------------------------------------------------------------//
static void timer_bounce_init (void)
{
	LL_TIM_InitTypeDef TIM_InitStruct = {0};

  LL_APB1_GRP1_EnableClock(TIM_BOUNCE_DELAY_APB1_BIT);   // Peripheral clock enable 

  TIM_InitStruct.Prescaler = (uint16_t)((CPU_CLOCK_VALUE/1000000)-1); //предделитель 72МГц/72=1МГц
  TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
  TIM_InitStruct.Autoreload = (1000-1);
  TIM_InitStruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
  TIM_InitStruct.RepetitionCounter = 0;
  LL_TIM_Init(TIM_BOUNCE_DELAY, &TIM_InitStruct);
  LL_TIM_DisableARRPreload(TIM_BOUNCE_DELAY);	
	
	LL_TIM_ClearFlag_UPDATE(TIM_BOUNCE_DELAY); //сброс флага обновления таймера
	LL_TIM_EnableIT_UPDATE(TIM_BOUNCE_DELAY); //разрешений прерываний от таймера
	LL_TIM_SetCounter(TIM_BOUNCE_DELAY, 0); //сброс счётного регистра

	NVIC_SetPriority(TIM_BOUNCE_DELAY_IRQn, 4); //приоритет прерывания
  NVIC_EnableIRQ(TIM_BOUNCE_DELAY_IRQn); //включение прерывания от таймера
  LL_TIM_EnableCounter(TIM_BOUNCE_DELAY); //включение таймера	

}

//---------------------------------------------------------------------------------------//
void TIM_BOUNCE_DELAY_IRQHandler(void)
{
	if (LL_TIM_IsActiveFlag_UPDATE(TIM_BOUNCE_DELAY) == SET)
	{	
	  LL_TIM_ClearFlag_UPDATE (TIM_BOUNCE_DELAY); //сброс флага обновления таймера
		//LL_TIM_DisableCounter(TIM_BOUNCE_DELAY); //выключение таймера
		//end_bounce = SET; //установка флага окончания ожидания прекращения дребезга		
    TicksCounter++; 
	  if ( SysTick_CallbackFunction != NULL ) //если указатель на коллбэк xTimer_Task проинициализирован
	  { SysTick_CallbackFunction(TicksCounter); } //вызов коллбэка
	}
}

//---------------------------------------------------------------------------------------//
void timers_ini (void)
{
	tim_delay_init(); 		//инициализация TIM7 для микросекундных задержек
	timer_bounce_init();	//инициализация TIM6 для привязки к xTimer
  SysTick_CallbackFunction = xTimer_Task; //присвоение указателю значения ф-и xTimer_Task
  xTimeNow = Get_SysTick; //инициализация указателя на ф-ю получения системного времени
  xTimerBtnBounce = xTimer_Create(KEY_BOUNCE_TIME, ONCE, &vTimerBtnBounceCallback, DISABLE);

  xTimerEncDelay = xTimer_Create(ENC_DELAY_TIME, ONCE, &vTimerEncDelayCallback, DISABLE);
}

//---------------------------------------------------------------------------------------//
xTimerHandle xTimer_Create(uint32_t xTimerPeriodInTicks, FunctionalState AutoReload, 
tmrTIMER_CALLBACK CallbackFunction, FunctionalState NewState)
{
	xTimerHandle NewTimer = NULL; //инициализация указателя на структуру для программного таймера
	
	for (uint16_t count = 0; count < MAX_xTIMERS; count++) //поиск свободного элемента массива структур с таймерами
	{
		if (xTimerList[count].CallbackFunction == NULL) 		//если указатель на ф-ю коллбэка пуст
		{
			xTimerList[count].periodic = xTimerPeriodInTicks;		//время задержки в мс
			xTimerList[count].AutoReload = AutoReload;				//периодический запуск или одиночный
			xTimerList[count].CallbackFunction = CallbackFunction;	//указатель на ф-ю коллбэка
			
			if (NewState != DISABLE) //если новый таймер в состояние  ENABLE
			{
				xTimerList[count].expiry = xTimeNow() + xTimerPeriodInTicks; //вычисление времени следующего вызова коллбэка
				xTimerList[count].State = __ACTIVE; //состояние таймера активен
			} 
			else 
			{	xTimerList[count].State = __IDLE;	}	//если новый таймер в состояние  DISABLE	
			NewTimer = (xTimerHandle)(count+1); //возврат номера указателя на структуру таймера
			break;
    }
  }
	return NewTimer;
}

//---------------------------------------------------------------------------------------//
void xTimer_SetPeriod(xTimerHandle xTimer, uint32_t xTimerPeriodInTicks) 
{
	if ( xTimer != NULL )  
	{	xTimerList[(uint32_t)xTimer-1].periodic = xTimerPeriodInTicks;	} //установка нового значения задержки таймера
}

//---------------------------------------------------------------------------------------//
void xTimer_Reload(xTimerHandle xTimer) 
{
	if ( xTimer != NULL ) 
	{
		xTimerList[(uint32_t)xTimer-1].expiry = xTimeNow() + xTimerList[(uint32_t)xTimer-1].periodic; //установка уставки таймера
		xTimerList[(uint32_t)xTimer-1].State = __ACTIVE; //запуск таймера
	}
}

//---------------------------------------------------------------------------------------//
void xTimer_Delete(xTimerHandle xTimer)
{
	if ( xTimer != NULL ) 
	{
		xTimerList[(uint32_t)xTimer-1].CallbackFunction = NULL;
		xTimerList[(uint32_t)xTimer-1].State = __IDLE;
		xTimer = NULL;
	}		
}

//---------------------------------ф-я диспетчера xTimer---------------------------------//
void xTimer_Task(uint32_t portTick)
{
	uint16_t i;
	
	for (i = 0; i < MAX_xTIMERS; i++) 
  {
		switch (xTimerList[i].State) 
		{
			case __ACTIVE: //если таймер активен
				if ( portTick >= xTimerList[i].expiry ) //если текущее системное время больше или равно установленному
				{				
					if ( xTimerList[i].AutoReload != DISABLE ) //если таймер периодический
					{	xTimerList[i].expiry = portTick + xTimerList[i].periodic;	} //установка нового времени срабатывания таймера
					else 
					{	xTimerList[i].State = __IDLE;	}	//статус таймера не активен		
					xTimerList[i].CallbackFunction((xTimerHandle)(i+1)); //вызов коллбэка
				}					
				break;
				
			default:
				break;
		}
	}	
}

// коллбэк прерывания таймера SysTick ------------------------------------------------------//
void SysTick_Callback(void)
{
	TicksCounter++; 
	if ( SysTick_CallbackFunction != NULL ) //если указатель на коллбэк xTimer_Task проинициализирован
	{SysTick_CallbackFunction(TicksCounter);} //вызов коллбэка
}

//------------------------------------------------------------------------------------------//
static void vTimerBtnBounceCallback(xTimerHandle xTimer) 
{
  ButtonBounceCallback ();
}

//---------------------------------------------------------------------------------------------------------//
void xTimersButtonBounce_Reload (uint16_t delay_ms)
{
	xTimer_SetPeriod(xTimerBtnBounce, delay_ms);
	xTimer_Reload(xTimerBtnBounce);	
}

//------------------------------------------------------------------------------------------//
static void vTimerEncDelayCallback(xTimerHandle xTimer)
{
  EncoderDelayCallback ();
}

//---------------------------------------------------------------------------------------------------------//
void xTimerEncDelay_Reload (uint16_t delay_ms)
{
	xTimer_SetPeriod(xTimerEncDelay, delay_ms);
	xTimer_Reload(xTimerEncDelay);	
}

// возвращает значение SysTick-------------------------------------------------------------//
static uint32_t Get_SysTick(void)
{ 
  return TicksCounter;  
}
 
// задержка в мс---------------------------------------------------------------------------//
void delay_ms(uint32_t delay)
{	
  uint32_t ticks = 0;
	ticks = Get_SysTick(); //количество тиков на момент вызова ф-ии
	while((Get_SysTick() - ticks) < delay)	{}
}



/* USER CODE END 1 */
