
#ifndef __ENCODER_H__
#define __ENCODER_H__

#ifdef __cplusplus
extern "C" {
#endif

// Includes ---------------------------------------------------------------------//
#include "main.h"
#include "typedef.h"

// Defines ---------------------------------------------------------------------//

// Prototypes ------------------------------------------------------------------//
uint8_t read_encoder (encoder_data_t * );
void encoder_reset (encoder_data_t * );
void EncoderDelayCallback (void);

// Variables ------------------------------------------------------------------//

#ifdef __cplusplus
}
#endif

#endif 

