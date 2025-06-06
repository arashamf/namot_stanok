
#ifndef __MENU_H__
#define __MENU_H__

#ifdef __cplusplus
extern "C" {
#endif

// Includes ---------------------------------------------------------------------//
#include "main.h"
#include "typedef.h"
#include "fonts.h"

// Defines ---------------------------------------------------------------------//

// Prototypes ------------------------------------------------------------------//
void init_loop(void);
void main_loop (encoder_data_t * , coil_data_t ** );
void setup_menu (encoder_data_t *, coil_data_t * );
uint8_t menu_select_preset (encoder_data_t * , coil_data_t ** );
void turn_drive_menu (uint8_t , coil_data_t * , uint8_t );
// Variables ------------------------------------------------------------------//

#ifdef __cplusplus
}
#endif

#endif 

