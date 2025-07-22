
// Includes --------------------------------------------------------------------//
#include "eeprom.h"
#include "typedef.h"
#include "usart.h"
#include "i2c.h"
#include <stdio.h>

// Defines ---------------------------------------------------------------------//                                   
#define 	EEPROM_MEMORY_PAGE 		0x0001

// Private typedef ------------------------------------------------------------//

// Private variables ---------------------------------------------------------//
uint8_t EEPROM_TX_Buf[NUMB_COIL_DATA] = {0} ;
uint8_t EEPROM_RX_Buf[NUMB_COIL_DATA] = {0} ;

// Functions -------------------------------------------------------------------//

//--------------------------------------------------------------------------------------//
void SaveByte_In_EEPROM (uint8_t byte)
{
	I2C_WriteByte ( EEPROM_I2C1_ADDRESS, EEPROM_MEMORY_PAGE, byte);
	#ifdef __USE_DBG
		sprintf ((char *)DBG_buffer,  "WRIGHT:%d\r\n", byte);
		DBG_PutString(DBG_buffer);
	#endif	
}

//--------------------сохранение данных настройки пресета в EEPROM--------------------//
void SaveCoilData (uint8_t * data)
{
	uint8_t bytes_OK = 0;
	
	for (uint8_t count = 0; count < NUMB_COIL_DATA; count++)
	{	
		EEPROM_TX_Buf[count] = *(data+count); 

		#ifdef __USE_DBG
		sprintf ((char *)DBG_buffer,  "%u", *(data+count));
		DBG_PutString(DBG_buffer);
		#endif	
	}
	
	bytes_OK = I2C_WriteBuffer (EEPROM_I2C1_ADDRESS, EEPROM_MEMORY_PAGE, EEPROM_TX_Buf, NUMB_COIL_DATA);
	
	#ifdef __USE_DBG
	sprintf ((char *)DBG_buffer,  "\r\n%u\r\n", bytes_OK );
	DBG_PutString(DBG_buffer);
	#endif		
}

//------------------------------------------------------------------------------------//
void GetCoilData (uint8_t * data)
{
	I2C_ReadBuffer (EEPROM_I2C1_ADDRESS, EEPROM_MEMORY_PAGE, EEPROM_RX_Buf, NUMB_COIL_DATA);	
	
	for (uint8_t i = 0; i < NUMB_COIL_DATA; i++)
	{	
		data[i] = EEPROM_RX_Buf[i];	

		#ifdef __USE_DBG
		sprintf ((char *)DBG_buffer,  "%u", data[i]);
		DBG_PutString(DBG_buffer);
		#endif	
	}	
		#ifdef __USE_DBG
		sprintf ((char *)DBG_buffer,  "\r\n");
		DBG_PutString(DBG_buffer);
		#endif		
}
