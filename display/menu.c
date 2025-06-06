
// Includes -------------------------------------------------------------------------------------------//
#include "menu.h"
#include "typedef.h"
#include "tim.h"
#include "button.h"
#include "drive.h"
#include "ssd1306.h"
#include "fonts.h"
#include "calc_value.h"
#include "encoder.h"
#include "usart.h"
#include "gpio.h"
#include "iwdg.h"

// Declarations and definitions ----------------------------------------------------------------------//

// Variables -----------------------------------------------------------------------------------------//
uint8_t count_delay = 0; //количество итераций по 10 мс для задержки запуска основного цикла
__IO uint8_t drive_mode = PRESET1; //номер пресета

// Functions -----------------------------------------------------------------------------------------//
void init_loop (void)	
{
	__IO uint16_t key_code = NO_KEY; //код кнопки
	while (count_delay < START_DELAY) //задержка 1с
	{
		if ((key_code = start_scan_key_PEDAL()) == NO_KEY)  //если педаль нажата
		{	count_delay++; }
		else
		{
			release_pedal_menu (); //предупреждение о нажатие педали
			while ((key_code = start_scan_key_PEDAL()) != NO_KEY) {}
		}	
		delay_us (10000);
	}
	init_status_flags_drives();
}

//-----------------------------------------------------------------------------------------------------//
void main_loop (encoder_data_t * HandleEncData, coil_data_t ** HandleCoilData)
{
	__IO uint16_t key_code = NO_KEY; //код кнопки
	if ((key_code = scan_keys()) != NO_KEY)
	{
		switch (key_code) //обработка кода нажатой кнопки
		{	
			case KEY_PEDAL_LONG:
				start_drives_turn (drive_mode, *(HandleCoilData + (drive_mode-1))); //выпонение пресета
				while (scan_button_PEDAL() != OFF) {}	//ожидание отключения педали
				break;
					
			case KEY_SETUP_SHORT:	
				drive_mode = menu_select_preset(HandleEncData, HandleCoilData);	//редактирование пресетов и возврат номера выполняемого пресета 
				break;
			
			case KEY_SETUP_LONG:
				if (drive_mode != NO_PRESET) //если установлен номер выполняемого пресета (1..4)
				{	settings_preset_reset (drive_mode, *(HandleCoilData + (drive_mode-1))); }//сброс выполненного пресета
				break;
				
			case KEY_NULL_SHORT:
			case KEY_NULL_LONG:
				mode_setup_null_screen(); //заставка режима установки сдвигающего двигеля в нулевую позицию
				setup_null_position ();  //установка сдвигающего двигеля в нулевую позицию
				main_menu_select_preset_screen(); //заставка главного меню
				break;
			
			case KEY_MANUAL_SHORT:
			case KEY_MANUAL_LONG:
				mode_setup_manual_dir_screen(); //вывод заставки выбора направления режима ручной установки сдвигающего двигателя
				while (select_direction() == ERROR) {} //ожидания нажатия кнопки выбора направления сдвигающего двигателя
				mode_setup_manual_screen (); //вывод заставки режима ручной установки сдвигающего двигателя
				manual_control_drive1 ();	//ф-я ручной установки положения сдвигающего двигателя
				main_menu_select_preset_screen(); //вывод заставки режима выбора пресета
				break;
		
			default:
				break;	
		}			
	}
}

//-----------------------------------------------------------------------------------------------------//
void setup_menu (encoder_data_t * HandleEncData, coil_data_t * HandleCoilData)
{
	uint16_t key_code = NO_KEY;	
	
	//ввод скорости
	if(HandleCoilData->drive2_turn_in_minute == 0) 
	{	HandleCoilData->drive2_turn_in_minute = BASE_TURN_IN_MINUTE; }
	
	Stop_Count_Timers(); //остановка счётных таймеров
	setup_speed_screen (HandleCoilData->drive2_turn_in_minute);
	while (1)
	{
		if (read_encoder (HandleEncData) == ON) //если данные от энкодера изменилось
		{
		//	HandleCoilData->rotation_speed += 5*HandleEncData->delta; //увелечиние/уменьшение множителя скорости в импульсах (шаг изменения - 5)
			HandleCoilData->drive2_turn_in_minute += 3*HandleEncData->delta;
			if (HandleCoilData->drive2_turn_in_minute > MAX_VALUE_TURN) //если количество оборотов в минуту превысило максимально допустимое значение
			{	HandleCoilData->drive2_turn_in_minute = MIN_VALUE_TURN;	}
			else
			{
				if (HandleCoilData->drive2_turn_in_minute <	MIN_VALUE_TURN)
				{	HandleCoilData->drive2_turn_in_minute = MAX_VALUE_TURN;	}
			}
			HandleCoilData->pulse_frequency = calc_rotation_speed(HandleCoilData->drive2_turn_in_minute); //расчёт количества оборотов в минуту
			drive2_turn (HandleCoilData);	//запуск вращающего двигателя
			setup_speed_screen (HandleCoilData->drive2_turn_in_minute);
		}	
		if ((key_code = scan_keys()) != NO_KEY) //если была нажата кнопка
		{
			if (key_code ==  KEY_SETUP_SHORT) //короткое нажатие кнопки энкодера - переход к вводу количества витков следующей обмотки
			{	
				SoftStop_Drives() ;
				HardStop_Drives () ;
				status_drives.end_turn_drive2 = DRIVE_FREE;
				break;
			}
		}
	}	
	
	//установка передаточного соотношения
	if(HandleCoilData->gear_ratio == 0) 
	{	HandleCoilData->gear_ratio = DEFAULT_GEAR_RATIO; }	
	setup_ratio_screen (HandleCoilData->gear_ratio);
	
	while (1)
	{
		if (read_encoder (HandleEncData) == ON) //если данные от энкодера изменилось
		{
			HandleCoilData->gear_ratio += 5*HandleEncData->delta;
			if (HandleCoilData->gear_ratio > MAX_VALUE_RATIO)
			{	HandleCoilData->gear_ratio = MIN_VALUE_RATIO;	}
			else
			{
				if (HandleCoilData->gear_ratio < MIN_VALUE_RATIO)
				{	HandleCoilData->gear_ratio = MAX_VALUE_RATIO;	}
			}
			drive1_turn (HandleCoilData);	//запуск сдвигающего двигателя
			drive2_turn (HandleCoilData);	//запуск вращающего двигателя
			setup_ratio_screen (HandleCoilData->gear_ratio);
		}	
		if ((key_code = scan_keys()) != NO_KEY) //если была нажата кнопка
		{
			if (key_code == KEY_SETUP_SHORT) //короткое нажатие кнопки энкодера - переход к вводу количества витков следующей обмотки
			{	
				SoftStop_Drives() ;
				HardStop_Drives () ;
				status_drives.end_turn_drive1 = DRIVE_FREE;		
				status_drives.end_turn_drive2 = DRIVE_FREE;
				break;
			}
		}
	}	

	//установка количества витков обмоток
	for (uint8_t count = 0; count < MAX_NUMBER_COIL; count++)
	{
		setup_coil_screen (count+1, HandleCoilData->set_coil[count]); //заставка режима установки количества витков
		while (1)
		{
			if (read_encoder (HandleEncData) == ON) //если данные от энкодера изменилось
			{
				HandleCoilData->set_coil[count] += HandleEncData->delta;	//прибавление приращения показаний энкодера
				if (HandleCoilData->set_coil[count] < MIN_VALUE_COIL)
				{	HandleCoilData->set_coil[count] = MAX_VALUE_COIL;	}
				else
				{
					if (HandleCoilData->set_coil[count] > MAX_VALUE_COIL)
					{	HandleCoilData->set_coil[count] = MIN_VALUE_COIL;	}
				}
				setup_coil_screen (count+1, HandleCoilData->set_coil[count]); //заставка режима установки количества витков
			}
			if ((key_code = scan_keys()) != NO_KEY) //если была нажата кнопка
			{
				if (key_code == KEY_SETUP_LONG) //длинное нажатие кнопки энкодера - выход из режима установки количества витков
				{		
					HandleCoilData->remains_coil[count] = HandleCoilData->set_coil[count]; //установка оставшегося количества витков обмотки					
					HandleCoilData->set_numb_winding = count+1; //необходимое количество обмоток
					HandleCoilData->complet_winding = 0; //количество полностью намотанных обмоток				
					return;
				}	
				else
				{
					if (key_code == KEY_SETUP_SHORT) //короткое нажатие кнопки энкодера - переход к вводу количества витков следующей обмотки
					{
						HandleCoilData->remains_coil[count] = HandleCoilData->set_coil[count]; //установка оставшегося количества витков обмотки
						break;
					}
				}
			}			
		}
	}
	HandleCoilData->set_numb_winding = MAX_NUMBER_COIL; //необходимое количество обмоток
	HandleCoilData->complet_winding = 0;	//количество полностью намотанных обмоток
	return;
}

//-----------------------------------------------------------------------------------------------------//
uint8_t menu_select_preset (encoder_data_t * HandleEncData, coil_data_t ** HandleBufCoilData)
{
	__IO uint16_t key_code = NO_KEY;
	
	menu_select_preset_screen (drive_mode);
	while (1)
	{		
		if (read_encoder (HandleEncData) == ON) //чтение данных энкодера
		{
			drive_mode += HandleEncData->delta; //изменение указателя на номер пресета
			if (drive_mode > PRESET4)
			{	drive_mode = PRESET1;	}
			else
			{	
				if (drive_mode < PRESET1) 
				{	drive_mode = PRESET4;	}
			}	
			HandleEncData->delta = 0;	
			menu_select_preset_screen (drive_mode);				
		}		
			
		if ((key_code = scan_keys()) != NO_KEY)
		{
			switch (key_code) //обработка кода нажатой кнопки
			{	
				case KEY_SETUP_SHORT:
					setup_menu (HandleEncData, *(HandleBufCoilData+drive_mode-1)); //переход в меню настройки пресета
					break;
				
				case KEY_SETUP_LONG:
					turn_drive_menu (drive_mode, HandleBufCoilData[drive_mode-1], 0); //отображение номера пресета, и количества оставшихся и установленного количества витков 1 обмотки
					return drive_mode; //выход в главное меню
					break;
			}
			menu_select_preset_screen (drive_mode);		
		}
	}
}


//-----------------------------------------------------------------------------------------------------//
void turn_drive_menu (uint8_t numb_preset, coil_data_t * HandleCoilData, uint8_t numb_coil)
{
	default_screen (numb_preset, HandleCoilData->remains_coil[numb_coil], HandleCoilData->set_coil[numb_coil], numb_coil+1);	
}
