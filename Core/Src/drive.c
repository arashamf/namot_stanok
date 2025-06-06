
// Includes ------------------------------------------------------------------//
#include "typedef.h"
#include "drive.h"
#include "gpio.h"
#include "tim.h"
#include "menu.h"
#include "usart.h"
#include "button.h"
#include "eeprom.h"
#include <stdio.h>

// Functions --------------------------------------------------------------------------------------//
static void init_drive1_setting (coil_data_t * );
static void init_drive2_setting (coil_data_t * );
static void init_drives_setting (coil_data_t * , uint8_t);
static void init_drives_for_full_turn (void);
static void drives_one_full_turn (void);

// Variables --------------------------------------------------------------------------------------//
__IO float step_unit = ((float)STEP18_IN_SEC/(float)STEP_DIV); //количество секунд в одном микрошаге(1,8гр/100=6480/100=64,8)
PWM_data_t Drives_PWM = {0};
volatile STATUS_FLAG_DRIVE_t status_drives; 
coil_data_t preset_null_pos = {0};

//-----------------------------------------------------------------------------------------------------//
void init_status_flags_drives (void) 
{
	status_drives.direction 	= 		RIGHT; //двигатели не активны, сдвиг катушки вправо
	status_drives.end_turn_drive1 = 	DRIVE_FREE; //двигатель 1 свободен
	status_drives.end_turn_drive2 = 	DRIVE_FREE; //двигатель 2 свободен
}

//-----------------------------------------------------------------------------------------------------//
static void init_drive2_setting (coil_data_t * HandleCoilData)
{
	uint32_t period_drive2 		=	1000000/HandleCoilData->pulse_frequency; //период импульса верхнего (мотающего) двигателя в мкс
	Drives_PWM.Compare_Drive2	=	(period_drive2/2) - 1; //длительность импульса верхнего (мотающего) двигателя 2
	Drives_PWM.Period_Drive2	=	(period_drive2-1); 	//сохранение периода верхнего (мотающего) двигателя 2
}

//-----------------------------------------------------------------------------------------------------//
static void init_drive1_setting (coil_data_t * HandleCoilData)
{
	uint32_t period_drive1 		=	((Drives_PWM.Period_Drive2+1)*100)/HandleCoilData->gear_ratio;			//период импульса нижнего (сдвигающего) двигателя
	Drives_PWM.Compare_Drive1	=	(period_drive1/2)-1; 	//длительность импульса нижнего (сдвигающего) двигателя 1
	Drives_PWM.Period_Drive1	=	(period_drive1-1); 	//сохранение периода нижнего (сдвигающего) двигателя 1
}

//-----------------------------------------------------------------------------------------------------//
static void init_drives_setting (coil_data_t * HandleCoilData, uint8_t number)
{
	init_drive2_setting (HandleCoilData);
	init_drive1_setting (HandleCoilData);
	Drives_PWM.turn_number 		= (HandleCoilData->remains_coil[number]); //сохранение количества полных оборотов 
	Drives_PWM.number_cnt_PWM_DR1 = (PULSE_IN_TURN*HandleCoilData->gear_ratio)/100; //отсечка для таймера считающего количество импульсов мотающего двигателя
	
	#ifdef __USE_DBG
		sprintf (DBG_buffer,  "ratio=%u,Period=%u,Compare=%u\r\n", HandleCoilData->gear_ratio, Drives_PWM.Period_Drive1, Drives_PWM.Compare_Drive1);
		DBG_PutString(DBG_buffer);
	#endif
}

//------------------------------инициализация двигателей перед запуском------------------------------//
static void init_drives_for_full_turn (void) 
{
	DRIVE2_ENABLE(START);		//разрешение включения двигателя 2
	DIR_DRIVE2 (BACKWARD); //направление вращения двигателя 2
	DRIVE1_ENABLE(START); //разрешение включения двигателя 1
	DIR_DRIVE1 (status_drives.direction); //направление вращения двигателя 1
	delay_us (2);	
}

//------------------------запуск вращения для полного оборота всех двигателей-------------------------//
static void drives_one_full_turn (void) 
{
	Drives_PWM_start (&Drives_PWM); //загрузка настроек в таймеры
}

//-----------------------инициализация и запуск вращения сдвигающего двигателя 1-----------------------//
void drive1_turn (coil_data_t * HandleCoilData)
{
	status_drives.end_turn_drive1 = DRIVE_BUSY;
	init_drive1_setting (HandleCoilData);
	DRIVE1_ENABLE(START); //разрешение запуска двигателя 1
	DIR_DRIVE1 (status_drives.direction); //установка направления вращения двигателя 1
	delay_us (2);		
	Drive1_PWM_start(&Drives_PWM);
}

//-----------------------инициализация и запуск вращения вращающего двигателя 2-----------------------//
void drive2_turn (coil_data_t * HandleCoilData)
{
	status_drives.end_turn_drive2 = DRIVE_BUSY;
	init_drive2_setting (HandleCoilData);
	DRIVE2_ENABLE(START); //разрешения запуска двигателя 1
	DIR_DRIVE2 (BACKWARD); //установка направления вращения двигателя 2
	delay_us (2);		
	Drive2_PWM_start (&Drives_PWM); //загрузка настроек в таймеры
}

//--------------------------------------------------------------------------------------------------//
void SoftStop_Drives(void) 
{
	Drive1_PWM_stop() ;
	Drive2_PWM_stop();
}


//--------------------------------------------------------------------------------------------------//
void HardStop_Drives (void) 
{
	DRIVE1_ENABLE(STOP); 
	DRIVE2_ENABLE(STOP);	
}

//-----------------------------------------------------------------------------------------------------//
void settings_preset_reset (uint8_t numb_preset, coil_data_t * HandleCoilData)
{
	for (uint8_t count = 0; count < HandleCoilData->set_numb_winding; count++)
	{
		HandleCoilData->remains_coil[count] = HandleCoilData->set_coil[count]; //сброс количества витков обмотки
	}
	HandleCoilData->complet_winding = 0; //оставшееся количество обмоток равно 0
	turn_drive_menu (numb_preset, HandleCoilData, HandleCoilData->complet_winding); //вывод меню
}

//-----------------------------------------------------------------------------------------------------//
int8_t start_drives_turn (uint8_t numb_preset, coil_data_t * HandleCoilData)
{
	uint8_t count = 0;
	if(HandleCoilData->complet_winding < HandleCoilData->set_numb_winding) //если количество намотанных обмоток меньше необходимого количества обмоток
	{
		count = HandleCoilData->complet_winding; //текущий номер обмотки
		init_drives_setting (HandleCoilData, count); //расчёт настроек двигателей
		init_drives_for_full_turn();
		
		while (HandleCoilData->remains_coil[count] > 0) //пока количество оставшихся витков обмотки больше нуля
		{
			if ((status_drives.end_turn_drive2 == DRIVE_FREE) && (status_drives.end_turn_drive1 == DRIVE_FREE)) //проверка флагов двигателей 
			{
				HandleCoilData->remains_coil[count] = (Drives_PWM.turn_number-1); //сохранение количества оставшихся обмоток				
				status_drives.end_turn_drive2 = DRIVE_BUSY; //двигатель 2 занят (активен)
				status_drives.end_turn_drive1 = DRIVE_BUSY; //двигатель 1 занят (активен)
				drives_one_full_turn(); //запуск вращения на один полный оборот
				
				#ifdef __USE_DBG
					sprintf (DBG_buffer,  "turn_start, numb=%u\r\n", Drives_PWM.turn_number);
					DBG_PutString(DBG_buffer);
				#endif
				
				turn_drive_menu (numb_preset, HandleCoilData, count); //вывод меню			
			}
			if	(scan_button_PEDAL() == OFF) //если педаль была отпущена
			{	
				HardStop_Drives ();
				return (HandleCoilData->set_numb_winding - HandleCoilData->complet_winding); //возврат количества намотанных обмоток			
			}
		}
		HandleCoilData->complet_winding++; //увеличиние на 1 количества намотанных обмоток			
		while	((status_drives.end_turn_drive2 != DRIVE_FREE) && 
		(status_drives.end_turn_drive1 != DRIVE_FREE)) {} //ожидание завершения оборотов двигателей
		HardStop_Drives ();
		return (HandleCoilData->set_numb_winding - HandleCoilData->complet_winding);	
	}
	return -1;
}

//-----------------------------------------------------------------------------------------------------//
int8_t select_direction(void)
{
	__IO uint16_t key_code = NO_KEY;
	while ((key_code = scan_keys()) == NO_KEY) {} //ожидание нажатия кнопки
	switch (key_code) //обработка кода нажатой кнопки
	{	
			case KEY_MANUAL_SHORT:
			case KEY_MANUAL_LONG:
				status_drives.direction = RIGHT;
				break;
			
			case KEY_NULL_SHORT:
			case KEY_NULL_LONG:
				status_drives.direction = LEFT;
				break;
			
			default:
				return ERROR;
				break; 
	}
	return status_drives.direction;
}

//-----------------------------------------------------------------------------------------------------//
void setup_null_position (void)
{	
	Stop_Count_Timers();
	preset_null_pos.pulse_frequency = 2*PULSE_IN_TURN;
	preset_null_pos.gear_ratio = DEFAULT_GEAR_RATIO;	
	
	if (status_drives.end_turn_drive1 == DRIVE_FREE)
	{
		status_drives.direction = LEFT;
		init_drive2_setting (&preset_null_pos);
		status_drives.end_turn_drive1 = DRIVE_BUSY; //статус флага двигателя 1 - занят 
		drive1_turn(&preset_null_pos); //запуск вращения 
		while (status_drives.direction != RIGHT){} //сдвиг до правого датчика
		while ((LL_GPIO_IsInputPinSet(RIGHT_IN_GPIO_Port, RIGHT_IN_Pin)) == ON){}
		Drive1_PWM_stop(); //остановка генерации ШИМа
		DRIVE1_ENABLE(STOP); //запрет запуска двигателя 1
		status_drives.end_turn_drive1 = DRIVE_FREE; //статус флаг двигателя 1 - свободен		
	}
}

//-----------------------------------------------------------------------------------------------------//
void manual_control_drive1 (void)
{	
	uint16_t key_code = NO_KEY;
	Stop_Count_Timers();
	preset_null_pos.pulse_frequency = 2*PULSE_IN_TURN;
	preset_null_pos.gear_ratio = DEFAULT_GEAR_RATIO;	
	
	while (scan_button_PEDAL() != ON)  //ожидание нажатия педали
	{
		if ((scan_keys()) != NO_KEY)
		{	return;	}	
	} 
	while (((key_code = scan_keys()) == NO_KEY) || (key_code == KEY_PEDAL_SHORT) || (key_code == KEY_PEDAL_LONG))//пока не нажата никакая кнопка
	{
		if (status_drives.end_turn_drive1 == DRIVE_FREE) //проверка флага двигателя 1
		{
			init_drive2_setting (&preset_null_pos); //необходимо получить значение Period_Drive2
			status_drives.end_turn_drive1 = DRIVE_BUSY; //статус флага двигателя 1 - занят 
			drive1_turn(&preset_null_pos); //запуск вращения на один полный оборот
			while (scan_button_PEDAL() == ON){} //ожидание отпускания педали
			Drive1_PWM_stop(); //остановка генерации ШИМа
			DRIVE1_ENABLE(STOP); //запрет запуска двигателя 1
			status_drives.end_turn_drive1 = DRIVE_FREE; //статус флаг двигателя 1 - свободен
		}
	}
}

//------------------------------------------------------------------------------------------------------//
void Counter_PWM_Drive1_Callback (void)
{
	#ifdef __USE_DBG
		sprintf (DBG_buffer,  "drive1_full_turn_end\r\n");
		DBG_PutString(DBG_buffer);
	#endif
	
	Drive1_PWM_stop() ;
	status_drives.end_turn_drive1 = DRIVE_FREE; //двигатель 1 не активен
}

//------------------------------------------------------------------------------------------------------//
void Counter_PWM_Drive2_Callback (void)
{
	#ifdef __USE_DBG
		sprintf (DBG_buffer,  "drive2_full_turn_end\r\n");
		DBG_PutString(DBG_buffer);
	#endif
	
	Drive2_PWM_stop() ;
	status_drives.end_turn_drive2 = DRIVE_FREE; //двигатель 2 не активен
	if (Drives_PWM.turn_number > 0) //если количество оборотов верхнего двигателя больше нуля
	{	Drives_PWM.turn_number--; } //уменьшение количества оборотов на единицу
}

//-------------------------------------------LL_EXTI_LINE_12-------------------------------------------//
void Left_Sensor_Callback (void)
{
	DRIVE1_ENABLE(STOP);  //выключение двигателя 1
	status_drives.direction = (RIGHT); //левый датчик меняет направление намотки направо
	DIR_DRIVE1 (status_drives.direction);
	DRIVE1_ENABLE(START);  //включение двигателя 1
}

//--------------------------------------------LL_EXTI_LINE_2--------------------------------------------//
void Right_Sensor_Callback (void)
{	
	DRIVE1_ENABLE(STOP); //выключение двигателя 1
	status_drives.direction = (LEFT); //правый датчик меняет направление намотки налево
	DIR_DRIVE1 (status_drives.direction);
	DRIVE1_ENABLE(START); //включение двигателя 1
}
