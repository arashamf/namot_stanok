
#ifndef __EEPROM_H__
#define __EEPROM_H__

#ifdef __cplusplus
extern "C" {
#endif

// Includes ---------------------------------------------------------------------//
#include "main.h"
#include "typedef.h"

// Defines ---------------------------------------------------------------------//
#define 	EEPROM_MEMORY_PAGE 	0x0001

// Prototypes -----------------------------------------------------------------//
void SaveByte_In_EEPROM (uint8_t );
void SaveCoilData (uint8_t * );
void GetCoilData (uint8_t * );

// Variables ------------------------------------------------------------------//

#ifdef __cplusplus
}
#endif

#endif 

