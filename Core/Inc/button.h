
#ifndef __BUTTON_H__
#define __BUTTON_H__

#ifdef __cplusplus
extern "C" {
#endif

// Includes --------------------------------------------------------------------------//
#include "main.h"

// Exported types -------------------------------------------------------------------//

//Private defines ------------------------------------------------------------------//

// Prototypes ---------------------------------------------------------------------//
uint16_t start_scan_key_PEDAL(void);
uint16_t scan_keys (void);
uint8_t scan_button_PEDAL (void);
void ButtonBounceCallback (void);
#ifdef __cplusplus
}
#endif

#endif /* __BUTTON_H__ */

