
// Includes --------------------------------------------------------------------//
#include "encoder.h"
#include "main.h"
#include "typedef.h"
#include "tim.h"

// Defines ---------------------------------------------------------------------//
#define MAX_CNT_TIMEOUT   15

// Functions -------------------------------------------------------------------//
static uint8_t read_gray_code_from_encoder(void) ;

// Variables -------------------------------------------------------------------//
static uint8_t val_old = 0, val_new = 0; //значения битов энкодера
static uint8_t flag_dir = 0, flag_rev = 0; //флаг направления поворота
static uint8_t state_machine = ENC_WAIT; //стадия конечного автомата
static uint8_t cnt_timeout = 0; //количество таймаутов ожидания изменения показаний энкодера
static int32_t enc_count = 0; //счётчик щелчков энкодера
uint8_t	flag_end_delay = RESET;

//-----------------------------------------------------------------------------//
uint8_t read_encoder (encoder_data_t * HandleEncData) 
{
	HandleEncData->currCounter_SetClick = read_gray_code_from_encoder(); //чтение данных энкодера
  	switch (state_machine) //проверка номера стадии конечного автомата
  	{
    	case ENC_WAIT: //стадия ожидания поворота энкодера
   	 	if (flag_end_delay == RESET) //если таймаут истёк
    	{
      		if (val_new != val_old) //если показания энкодера изменились
	    	{ 
        		if (val_new == PULSE_STATE_FIRST_REV) 
        		{
          			state_machine = ENC_FIRST; //переход на следующую стадию автомата
          			flag_rev = 1; //флаг реверсного поворота энкодера
        		}
        		else
        		{
          			if (val_new == PULSE_STATE_FIRST_DIR)
          			{
            			state_machine = ENC_FIRST; //переход на следующую стадию автомата
            			flag_dir = 1; //флаг прямого поворота энкодера
          			}
          			else 
          			{
            			flag_rev = flag_dir = 0; //сброс флагов поворота энкодера
            			state_machine = ENC_RESET;  //переход в начальную стадию конечного автомата
					}
        		}
        		cnt_timeout = 0; //сброс счётчика таймаутов
        		val_old = val_new; //сохранение текущего показания энкодера
				flag_end_delay = SET;
				xTimerEncDelay_Reload(ENC_DELAY_TIME); //начало таймаута
      		}
    	}
    	break;

    	case ENC_FIRST: //стадия средней части импульса энкодера
      	if (flag_end_delay == RESET) //если таймаут истёк
      	{
        	if (val_new == val_old)
	      	{ 
          		cnt_timeout++;
          		if (cnt_timeout > MAX_CNT_TIMEOUT)
          		{
            		cnt_timeout = 0;  //сброс счётчика таймаутов
            		flag_rev = flag_dir = 0; //сброс флагов поворота энкодера
            		state_machine = ENC_RESET;
          		}
          		else
          		{ 
					flag_end_delay = SET;
					xTimerEncDelay_Reload(ENC_DELAY_TIME); ///начало таймаута ожидания изменений показаний энкодера 
				}
        	}       
      		else
      		{ 
        		cnt_timeout = 0; //сброс счётчика таймаутов             
        		val_old = val_new; //сохранение текущего показания энкодера
        		if (val_new == PULSE_STATE_SECOND) //если данные от экодера соотвествуют ожидаемым
        		{ state_machine = ENC_NULL; } //переход на следующую стадию автомата
        		else //если данные от энкодера не соотвествуют ожидаемым
        		{
          			flag_rev = flag_dir = 0;
          			state_machine = ENC_RESET; //переход на стадию сброса показаний энкодера
        		}
				flag_end_delay = SET;
				xTimerEncDelay_Reload(ENC_DELAY_TIME); ///начало таймаута ожидания изменений показаний энкодера 
     		}
   	 	}
    	break;        

    	case ENC_NULL: //стадия последней части импульса энкодера
      	if (flag_end_delay == RESET) //если таймаут истёк
      	{
        	if (val_new == val_old) //если показания энкодера не изменилось
	      	{ 
          		if (cnt_timeout > MAX_CNT_TIMEOUT) //если количество таймаутов ожидания изменения показаний энкодера превышено
          		{
            		cnt_timeout = 0;  //сброс счётчика таймаутов
            		flag_rev = flag_dir = 0; //сброс флагов поворота энкодера
            		state_machine = ENC_RESET; //переход в начальную стадию конечного автомата
          		}
          		else
         	 	{  
					flag_end_delay = SET;
					xTimerEncDelay_Reload(ENC_DELAY_TIME); ///начало таймаута ожидания изменений показаний энкодера 
				} 
        	}       
        	else
        	{ 
          		if (val_new ==  PULSE_STATE_LAST_REV) //если данные от экодера соотвествуют ожидаемым
          		{
            		if (flag_rev == 1) //если установлен флаг реверсного поворота энкодера
            		{ 
                  		state_machine = ENC_LAST;  //переход на следующую стадию автомата
                  		flag_end_delay = SET;
						xTimerEncDelay_Reload(ENC_DELAY_TIME); ///начало таймаута ожидания изменений показаний энкодера 
            		}              
          		}
          		else
          		{
            		if (val_new == PULSE_STATE_LAST_DIR) //если данные от экодера соотвествуют ожидаемым
            		{
              			if (flag_dir == 1) //если установлен флаг прямого поворота энкодера
              			{  
                			state_machine = ENC_LAST;  //переход на следующую стадию автомата
                			flag_end_delay = SET;
							xTimerEncDelay_Reload(ENC_DELAY_TIME); ///начало таймаута ожидания изменений показаний энкодера 
              			}
            		}
           			else //если данные от энкодера не соотвествуют ожидаемым
            		{
              			flag_rev = flag_dir = 0; //сброс флагов поворота энкодера
              			state_machine = ENC_RESET;  //переход на стадию сброса показаний энкодера
            		}
          		}
          		val_old = val_new; //сохранение текущего показания энкодера
          		cnt_timeout = 0; //сброс счётчика таймаутов
        	}
      	}
      	break;

    	case ENC_LAST: //проверка стадии импульса энкодера по умолчанию
      	if (flag_end_delay == RESET) //если таймаут истёк
      	{
        	if (val_new == val_old) //если показания энкодера не изменилось
	      	{ 
          		if (cnt_timeout > MAX_CNT_TIMEOUT) //если количество таймаутов ожидания изменения показаний энкодера превышено
          		{
            		cnt_timeout = 0;  //сброс счётчика таймаутов
            		flag_rev = flag_dir = 0; 
            		state_machine = ENC_RESET; //переход к начальной стадии конечного автомата
          		}
          		else
          		{  
					flag_end_delay = SET;
					xTimerEncDelay_Reload(ENC_DELAY_TIME); ///начало таймаута ожидания изменений показаний энкодера 
				} 
        	}       
        	else
        	{               
          		if (flag_rev == 1) //если установлен флаг реверсного поворота энкодера
          		{
            		enc_count--;  
          		}
          		else
          		{
            		if (flag_dir == 1) //если установлен флаг прямого поворота энкодера
            		{
              			enc_count++;  
            		} 
          		} 
          		flag_rev = flag_dir = 0; //сброс флагов поворота энкодера
          		val_old = val_new; //сохранение текущего показания энкодера
          		cnt_timeout = 0; //сброс счётчика таймаутов
          		state_machine = ENC_WAIT; //переход к начальной стадии конечного автомата
        	}            
      	} 
      	break;  

    	case ENC_RESET: //стадия ожидания сброса показанрий энкодера в состояние по умолчанию
      	if (flag_end_delay == RESET) //если таймаут истёк
      	{
        	if (val_new != val_old) //если показания энкодера изменились
        	{
          		if (val_new == PULSE_STATE_DEFAULT) //если энкодер в состоянии по умолчанию (поворот закончен)
	        	{ 
            		state_machine = ENC_WAIT; //переход к начальной стадии конечного автомата
          		}
          		val_old = val_new; //сохранение текущего показания энкодера
        	}               
        	flag_end_delay = SET;
			xTimerEncDelay_Reload(ENC_DELAY_TIME); ///начало таймаута ожидания изменений показаний энкодера 
      	}
      	break;

    	default:
      	break;         
  	}
	return HandleEncData->currCounter_SetClick ;
}

//---------------------------сброс показаний энкодера---------------------------//
void encoder_reset (encoder_data_t * HandleEncData) 
{
	HandleEncData->currCounter_SetClick = 0; 
	HandleEncData->prevCounter_SetClick = HandleEncData->currCounter_SetClick; //сохранение преобразованного текущего показания энкодера в структуру установки шага поворота	
	HandleEncData->delta = 0; //показания от энкодера обнуляются
}

//-----------------------------------------------------------------------------//
static uint8_t read_gray_code_from_encoder(void) 
{ 
    volatile uint8_t value = 0; 
    volatile uint8_t flag = 1;

    if((flag = LL_GPIO_IsInputPinSet(ENC_IN1_GPIO_Port, ENC_IN1_Pin)) == SET) 
    { value |= (1<<1); }

    if((flag =LL_GPIO_IsInputPinSet(ENC_IN2_GPIO_Port, ENC_IN2_Pin)) == SET) 
    { value |= (1<<0); }

  return value; 
}

//-----------------------------------------------------------------------------//
void EncoderDelayCallback (void)
{
	flag_end_delay = RESET;
}

