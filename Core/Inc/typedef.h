
#ifndef __TYPEDEF_H__
#define __TYPEDEF_H__

#ifdef __cplusplus
extern "C" {
#endif

// Includes -------------------------------------------------------------------------//
#include "main.h"

// Defines ----------------------------------------------------------------------//
#define 	I2C_REQUEST_WRITE       0x00
#define 	I2C_REQUEST_READ        0x01
#define 	SLAVE_OWN_ADDRESS       0xA0 //адрес EEPROM микросхемы

#define 	PULSE_IN_TURN			1600 	//количество микрошагов в одном полном обороте (360 гр) с учётом делителя драйвера
#define 	PULSE_IN_TIM_RCR		100
#define 	STEP_IN_TURN			200		//количество шагов (1,8гр) в одном полном обороте (360 гр)
#define 	STEP_DIV 				(PULSE_IN_TURN/STEP_IN_TURN)	//количество микрошагов (8) в одном шаге двигателя (1,8гр)

#define 	BASE_TURN_IN_MINUTE		30 		//начальное количество оборотов в минуту
#define   	STEP_TURN_IN_MINUTE		3 		//шаг приращение частоты вращения
#define 	MAX_VALUE_TURN 			150  	//максимальное количество оборотов в минуту
#define 	MIN_VALUE_TURN 			12 		//минимальное количество оборотов в минуту
#define 	BASE_PULSE_DR2			(PULSE_IN_TURN*BASE_TURN_IN_MINUTE)/60 	//800 ГЦ

#define 	MAX_VALUE_RATIO			300 
#define 	MIN_VALUE_RATIO			25 

#define 	MAX_VALUE_COIL			250 
#define 	MIN_VALUE_COIL			1 
#define 	DEFAULT_GEAR_RATIO		100
#define 	STEP18_IN_SEC			6480 		//количество секунд в одном шаге двигателя (1,8гр)
#define 	CIRCLE_IN_SEC			(STEP18_IN_SEC*CIRCLE_IN_STEP)	//количество секунд в одном полном обороте двигателя (360 гр)
#define 	SECOND_PER_MINUTE 		60
#define 	SECOND_PER_DEGREE 		3600

#define 	ON									1
#define 	OFF 								0
#define 	START								1
#define 	STOP 								0
#define 	FORWARD 							1
#define 	BACKWARD 							0
#define 	LEFT 								1
#define 	RIGHT 								0
#define 	DISP_CLEAR 							1
#define 	DISP_NOT_CLEAR 						0
#define 	DRIVE_FREE							1
#define 	DRIVE_BUSY							0
#define 	ERROR								-1
//#define 	EEPROM_NUMBER_BYTES 				4
#define 	CPU_CLOCK_VALUE						(72000000UL)		//частота контроллера 
#define 	MS_PER_SECOND						1000 
#define 	START_DELAY							0x64
#define 	MAX_NUMBER_COIL 					3
#define 	MAX_PRESET 							4

#define 	ENC_DELAY_TIME 						5
#define 	KEY_BOUNCE_TIME 					50 		// время дребезга в мс
#define 	KEY_AUTOREPEAT_TIME 				100 	// время автоповтора в мс

// Exported types ------------------------------------------------------------------//
typedef struct 
{
	uint8_t 	val_old; 	
	uint8_t 	val_new;	//значения битов энкодера
	uint8_t 	flag_dir; 	//флаг направления поворота
	uint8_t 	flag_rev;	
	int8_t 		delta; //текущее показание энкодера
} encoder_data_t;

//----------------------------------------------------------------------------------//
#pragma pack(1) //выравнивание битовых полей по одному байту
typedef struct 
{
	union
	{
		struct 
		{
			uint8_t 	set_numb_winding; //установленное количество обмоток
			uint8_t 	complet_winding; //оставшееся количество обмоток
			uint16_t	pulse_frequency; //количество импульсов в секунду
			uint16_t  	drive2_turn_in_minute; //количество оборотов в минуту
			uint16_t 	gear_ratio; //передаточное соотношение
			uint16_t 	set_coil[MAX_NUMBER_COIL]; //установленное количество витков обмотки
			uint16_t 	remains_coil[MAX_NUMBER_COIL]; //оставшееся количество витков обмотки		
		};
		uint8_t coil_buffer[MAX_NUMBER_COIL*2*2+5];
	};
	struct coil_data_t *next;
} coil_data_t;

//----------------------------------------------------------------------------------//
typedef union
{
	struct
	{
		uint8_t direction				: 	1;
		uint8_t end_turn_drive1			: 	1;
		uint8_t end_turn_drive2			: 	1;
		uint8_t reserve					:	5;
	};
	uint8_t flag;
}STATUS_FLAG_DRIVE_t;

//----------------------------------------------------------------------------------//
typedef struct 
{
	uint16_t Compare_Drive1; //значение сравнения двигателя 1 (длительность полупериода)
	uint16_t Compare_Drive2; //значение сравнения двигателя 2
	uint16_t Period_Drive1; //период импульса ШИМ двигателя 1
	uint16_t Period_Drive2; //период импульса ШИМ двигателя 2
	uint16_t number_cnt_PWM_DR1; //счётчик импульсов двигателя 1
	uint16_t turn_number; //количество совершённых оборотов
} PWM_data_t;

#pragma pack()

//код нажатой кнопки----------------------------------------------------------------//
typedef enum 
{
	NO_KEY 				= 	0x00,		//кнопка не нажата	
	KEY_PEDAL_SHORT 	= 	0x01,		//короткое нажатие педали
	KEY_PEDAL_LONG 		= 	0x02,		//длинное нажатие педали
	KEY_NULL_SHORT		= 	0x03,		//короткое нажатие кнопки выбора режима установки нулевой позиции
	KEY_NULL_LONG		=	0x04,		//длинное нажатие кнопки выбора режима установки нулевой позиции
	KEY_SETUP_SHORT		=	0x05,		//короткое нажатие выбора режима установки настройки обмоток катушки
	KEY_SETUP_LONG		=	0x06,		//длинное нажатие выбора режима установки настройки обмоток катушки
	KEY_MANUAL_SHORT	=	0x07,		//короткое нажатие выбора режима ручной установки сдвигающего двигателя
	KEY_MANUAL_LONG		=	0x08,		//длинное нажатие выбора режима ручной установки сдвигающего двигателя
} KEY_CODE_t; 					

//-----------------------------------------------------------------------------------//
typedef enum 
{
	KEY_STATE_OFF 				= 0	,		//режим - кнопка не нажата
	KEY_STATE_ON					,		//режим - кнопка нажата
	KEY_STATE_BOUNCE				, 		//режим -  дребезг кнопки
	KEY_STATE_AUTOREPEAT			,	 	//режим - режим ожидания (автоповтора) отжатия кнопки
	KEY_STATE_WAIT_TURNOFF
} KEY_STATE_t; 								//статус сканирования клавиатуры

//-----------------------------------------------------------------------------------//
typedef enum 
{
	NO_PRESET      	= 0 ,
	PRESET1 			,				
	PRESET2				,				
	PRESET3				, 			
	PRESET4				,	 			
} PRESET_t; 									

//-----------------------------------------------------------------------------------//
struct KEY_MACHINE_t
{
	KEY_CODE_t 		key_code;
	KEY_STATE_t 	key_state;
};

//-----------------------------------------------------------------------------------//
static enum 
{
	ENC_WAIT = 0,
	ENC_FIRST, 
	ENC_NULL,
	ENC_LAST,
	ENC_RESET,
} state_machine_enc;

//-----------------------------------------------------------------------------------//
static enum 
{
	PULSE_STATE_DEFAULT     = 0x00,
	PULSE_STATE_FIRST_DIR 	= 0x02,
	PULSE_STATE_FIRST_REV 	= 0x01, 
	PULSE_STATE_SECOND    	= 0x03,
	PULSE_STATE_LAST_DIR	= 0x01,
	PULSE_STATE_LAST_REV	= 0x02,
} pulse_enc_state;

// Private variables -----------------------------------------------------------//

#ifdef __cplusplus
}
#endif

#endif 