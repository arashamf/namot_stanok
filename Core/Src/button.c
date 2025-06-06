
// Includes ------------------------------------------------------------------//
#include "button.h"
#include "typedef.h"
#include "tim.h"
#include "usart.h"

// Exported types -------------------------------------------------------------//
static struct KEY_MACHINE_t Key_Machine;

// Prototypes ------------------------------------------------------------------------------//
static __IO uint8_t scan_buttons_GPIO (uint16_t );

//Private defines --------------------------------------------------------------------------//
#define COUNT_REPEAT_BUTTON 	5

// Private variables -----------------------------------------------------------------------//
uint8_t count_autorepeat = 0; //подсчёт удержания кнопки
volatile uint8_t end_bounce = OFF; //флаг окончания паузы на время дребезга и ожидания отпускания кнопки

//----------------------------------------------------------------------------------------//
uint16_t start_scan_key_PEDAL (void)
{
	static __IO uint8_t key_state = KEY_STATE_OFF; // начальное состояние кнопки - не нажата
	static __IO uint16_t key_code;
	static __IO uint8_t count_autorepeat = 0; //подсчёт удержания кнопки
	
	if(key_state == KEY_STATE_OFF) //если кнопка была отпущена - ожидание нажатия
	{
		if (LL_GPIO_IsInputPinSet(PEDAL_GPIO_Port, PEDAL_Pin) == OFF) //инверсная логика - если кнопка нажата, функция LL_GPIO_IsInputPinSet возвращает 0
		{
			key_state 	=  	KEY_STATE_BOUNCE; //переход на следующую стадию - режим ожидания окончания дребезга
			key_code 	= 	KEY_PEDAL_SHORT;
			end_bounce 	= 	RESET;
			xTimersButtonBounce_Reload(KEY_BOUNCE_TIME); //запуск ожидания окончания дребезга
		}
	}
	
	if (key_state ==  KEY_STATE_BOUNCE)  //режим ожидания окончания дребезга
	{
		if (end_bounce == SET) //если флаг окончания дребезга установлен
		{
			if(scan_buttons_GPIO(key_code) == OFF)	//если кнопка отпущена (нажатие менее 50 мс это дребезг)
			{
				key_state = KEY_STATE_OFF; //переход в начальное состояние ожидания нажатия кнопки
				return NO_KEY; //возврат 0 
			}	
		}
		else
		{
			end_bounce = RESET;
			xTimersButtonBounce_Reload (KEY_AUTOREPEAT_TIME); //установка таймера ожидания отключения кнопки
			key_state = KEY_STATE_AUTOREPEAT;   //переход в режим автоповтора 
		}		
	}
	
	if(key_state == KEY_STATE_AUTOREPEAT) //режим окончания дребезга
	{
		if (end_bounce == SET) //если флаг окончания дребезга установлен
		{
			if(scan_buttons_GPIO(key_code) == OFF)	//если кнопка отпущена (нажатие менее 50 мс это дребезг)
			{
				key_state = KEY_STATE_OFF; //переход в начальное состояние ожидания нажатия кнопки
				return NO_KEY; //возврат 0 
			}	
			else //если кнопка продолжает удерживаться
			{	
				key_state = KEY_STATE_WAIT_TURNOFF; //стадия ожидания отпускания кнопки
				end_bounce = RESET;
				xTimersButtonBounce_Reload (KEY_AUTOREPEAT_TIME); //установка таймера ожидания отключения кнопки
				return key_code;
			}
		}
	}
	
	if (key_state == KEY_STATE_WAIT_TURNOFF) //ожидание отпускания кнопки
	{	
		if (end_bounce == SET) //если флаг окончания дребезга установлен (устанавливается в прерывании таймера)
		{
			if(scan_buttons_GPIO(key_code) == OFF)	 // если кнопка была отпущена (короткое нажатие кнопки < 150 мс)
			{				
				key_code = NO_KEY;
				key_state = KEY_STATE_OFF; //переход в начальное состояние ожидания нажатия кнопки
			}
			else
			{
				end_bounce = RESET;
				xTimersButtonBounce_Reload (KEY_AUTOREPEAT_TIME); //установка таймера ожидания отключения кнопки
			}
		}
		return key_code;
	}
	return NO_KEY;
}


//----------------------------------------------------------------------------------------//
uint16_t scan_keys (void)
{
	static __IO uint8_t key_state = KEY_STATE_OFF; // начальное состояние кнопки - не нажата
	static __IO uint16_t key_code;
	static __IO uint16_t key_repeat_time; // счетчик времени повтора
	
	if(key_state == KEY_STATE_OFF) //если кнопка была отпущена - ожидание нажатия
	{
		if(LL_GPIO_IsInputPinSet(ENC_BTN_GPIO_Port, ENC_BTN_Pin) == OFF)
		{
			key_state =  KEY_STATE_ON; //переход на следующую стадию - режим проверки нажатия кнопки
			key_code = KEY_SETUP_SHORT; //сохранения кода кнопки
		}
		else
		{
			if(LL_GPIO_IsInputPinSet(RIGHT_BTN_GPIO_Port, RIGHT_BTN_Pin) == OFF)	//инверсная логика - если кнопка нажата, функция LL_GPIO_IsInputPinSet возвращает 0
			{
				key_state =  KEY_STATE_ON; //переход на следующую стадию - режим проверки нажатия кнопки
				key_code = KEY_NULL_SHORT; //сохранения кода кнопки
			}
			else
			{
				if (LL_GPIO_IsInputPinSet(LEFT_BTN_GPIO_Port, LEFT_BTN_Pin) == OFF)
				{
					key_state =  KEY_STATE_ON;
					key_code = KEY_MANUAL_SHORT;
				}
				else
				{
					if (LL_GPIO_IsInputPinSet(PEDAL_GPIO_Port, PEDAL_Pin) == OFF)
					{
						key_state =  KEY_STATE_ON;
						key_code = KEY_PEDAL_SHORT;
					}
				}
			}
		}
	}	
	if (key_state ==  KEY_STATE_ON)  //режим проверки нажатия кнопки
	{
		end_bounce = RESET;
		xTimersButtonBounce_Reload(KEY_BOUNCE_TIME); //запуск ожидания окончания дребезга
		key_state = KEY_STATE_BOUNCE; // переход в режим окончания дребезга
	}
	
	if(key_state == KEY_STATE_BOUNCE) //режим окончания дребезга
	{
		if (end_bounce == SET) //если флаг окончания дребезга установлен
		{
			if(scan_buttons_GPIO(key_code) == OFF)	 // если кнопка отпущена (нажатие менее 50 мс это дребезг)
			{
				key_state = KEY_STATE_OFF; //переход в начальное состояние ожидания нажатия кнопки
				return NO_KEY; //возврат 0 
			}	
			else //если кнопка продолжает удерживаться
			{	
				end_bounce = RESET;
				xTimersButtonBounce_Reload(KEY_BOUNCE_TIME); //запуск ожидания окончания дребезга
				key_state = KEY_STATE_AUTOREPEAT;   //переход в режим автоповтора 
				count_autorepeat = 0;
			}
		}
	}
	if (key_state == KEY_STATE_AUTOREPEAT) //режим автоповтора
	{
		if (end_bounce == SET) //если флаг окончания задержки (устанавливается в прерывании таймера)
		{
			if(scan_buttons_GPIO(key_code) == OFF)	 // если кнопка была отпущена (короткое нажатие кнопки < 150 мс)
			{
				key_state = KEY_STATE_OFF; //переход в начальное состояние ожидания нажатия кнопки
				return key_code; //возврата номера кнопки
			}
			else //если кнопка продолжает удерживаться
			{			
				if (count_autorepeat < COUNT_REPEAT_BUTTON) //ожидание 500 мс
				{	count_autorepeat++;	}
				else //если кнопка удерживалась более 650 мс
				{	
					switch (key_code)
					{
						
						case KEY_NULL_SHORT:
							key_code = KEY_NULL_LONG;	
							break;
						
						case KEY_SETUP_SHORT:
							key_code = KEY_SETUP_LONG;	
							break;	
						
						case KEY_MANUAL_SHORT:
							key_code = KEY_MANUAL_LONG;	
							break;	
						
						case KEY_PEDAL_SHORT:
							key_code = KEY_PEDAL_LONG;	
							break;	
						
						default:
							break;	
					}
					key_state = KEY_STATE_WAIT_TURNOFF; //стадия ожидания отпускания кнопки
					end_bounce = RESET;
					xTimersButtonBounce_Reload(KEY_BOUNCE_TIME); //запуск ожидания окончания дребезга
					return key_code;
				}
				end_bounce = RESET;
				xTimersButtonBounce_Reload(KEY_BOUNCE_TIME); //запуск ожидания окончания дребезга	
			} 	
		}					
	}
	
	if (key_state == KEY_STATE_WAIT_TURNOFF) //ожидание ожидания отпускания кнопки
	{	
		if (end_bounce == SET) //если флаг окончания дребезга установлен (устанавливается в прерывании таймера)
		{
			if(scan_buttons_GPIO(key_code) == OFF)	 // если кнопка была отпущена (короткое нажатие кнопки < 150 мс)
			{				
				key_code = NO_KEY;
				key_state = KEY_STATE_OFF; //переход в начальное состояние ожидания нажатия кнопки
			}
			else //установка таймера ожидания отключения кнопки
			{	
				end_bounce = RESET;
				xTimersButtonBounce_Reload(KEY_BOUNCE_TIME); //запуск ожидания окончания дребезга	
			}
		}
	}
	return NO_KEY;
}

//----------------------------------------------------------------------------------------//
static uint8_t scan_buttons_GPIO (uint16_t key_code)
{
	uint8_t pin_status = OFF; //статус кнопки - кнопка не нажата
	
	switch (key_code)
	{
						
		case KEY_SETUP_SHORT:
			if ((LL_GPIO_IsInputPinSet(ENC_BTN_GPIO_Port, ENC_BTN_Pin))	== OFF) //инверсная логика - если кнопка нажата, функция LL_GPIO_IsInputPinSet возвращает 0
			{	pin_status = ON;	} //статус кнопки - кнопка нажата
			break;
						
		case KEY_SETUP_LONG:
			if ((LL_GPIO_IsInputPinSet(ENC_BTN_GPIO_Port, ENC_BTN_Pin))	== OFF)
			{	pin_status = ON;	}
			break;	
						
		case KEY_NULL_SHORT:
			if ((LL_GPIO_IsInputPinSet(RIGHT_BTN_GPIO_Port, RIGHT_BTN_Pin))	== OFF)
			{	pin_status = ON;	}
			break;
						
		case KEY_NULL_LONG:
			if ((LL_GPIO_IsInputPinSet(RIGHT_BTN_GPIO_Port, RIGHT_BTN_Pin))	== OFF)
			{	pin_status = ON;	}
			break;
		
		case KEY_PEDAL_SHORT:
			if ((LL_GPIO_IsInputPinSet(PEDAL_GPIO_Port, PEDAL_Pin))	== OFF)
			{	pin_status = ON;	}
			break;
						
		case KEY_PEDAL_LONG:
			if ((LL_GPIO_IsInputPinSet(PEDAL_GPIO_Port, PEDAL_Pin))	== OFF)
			{	pin_status = ON;	}
			break;
			
		case KEY_MANUAL_SHORT:
			if ((LL_GPIO_IsInputPinSet(LEFT_BTN_GPIO_Port, LEFT_BTN_Pin))	== OFF)
			{	pin_status = ON;	}
			break;
						
		case KEY_MANUAL_LONG:
			if ((LL_GPIO_IsInputPinSet(LEFT_BTN_GPIO_Port, LEFT_BTN_Pin))	== OFF)
			{	pin_status = ON;	}
			break;	
			
		default:
			break;	
	}
	
	return pin_status;			
}

//----------------------------------------------------------------------------------------//
uint8_t scan_button_PEDAL (void)
{
	uint8_t status_PEDAL = OFF;
	if (LL_GPIO_IsInputPinSet(PEDAL_GPIO_Port, PEDAL_Pin) == OFF) //инверсная логика - если педаль нажата, функция LL_GPIO_IsInputPinSet возвращает 0
	{	status_PEDAL = ON; }
	return status_PEDAL;
}

//----------------------------------------------------------------------------------------//
void ButtonBounceCallback (void)
{
	end_bounce = ON;
}