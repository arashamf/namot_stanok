
// Includes --------------------------------------------------------------------//
#include "eeprom.h"
#include "typedef.h"
#include "usart.h"
#include "i2c.h"
#include <stdio.h>
// Defines ---------------------------------------------------------------------//                                   

// Private typedef ------------------------------------------------------------//

// Private variables ---------------------------------------------------------//
uint8_t EEPROM_TX_Buf[30] = {0} ;
uint8_t EEPROM_RX_Buf[30] = {0} ;

// Functions -------------------------------------------------------------------//
static void EEPROM_WriteBytes	(uint16_t, uint8_t *, uint16_t);
static void EEPROM_ReadBytes	(uint16_t, uint8_t *, uint16_t); 
encoder_data_t ReadData_From_EEPROM (uint8_t , uint8_t);

//--------------------------------------------------------------------------------------//
void SaveByte_In_EEPROM (uint8_t byte, uint8_t adress)
{
	I2C_WriteByte (byte, adress);
	#ifdef __USE_DBG
		sprintf ((char *)DBG_buffer,  "WRIGHT:%d\r\n", byte);
		DBG_PutString(DBG_buffer);
	#endif	
}

//--------------------------------------------------------------------------------------//
void SaveCoilData (uint8_t * data, uint8_t number_byte, uint8_t EEPROM_adress)
{
	#ifdef __USE_DBG
	sprintf ((char *)DBG_buffer,  "WRIGHT: ");
	DBG_PutString(DBG_buffer);
	#endif	
	
	for (uint8_t i = 0; i < number_byte; i++)
	{
		EEPROM_TX_Buf[i] = (uint8_t)(*(data+i)); //количество витков обмоток
	}		
	
	#ifdef __USE_DBG
	sprintf ((char *)DBG_buffer,  "\r\n");
	DBG_PutString(DBG_buffer);
	#endif	
	
	I2C_WriteBuffer (EEPROM_adress, EEPROM_TX_Buf, number_byte);
}

//--------------------------------------------------------------------------------------//
void GetCoilData (uint8_t * data, uint8_t number_byte, uint8_t adress)
{
	I2C_ReadBuffer (adress, EEPROM_RX_Buf, number_byte);	
	
	for (uint8_t i = 0; i < number_byte; i++)
	{	
		data[i] = EEPROM_RX_Buf[i];	
	}		
}

