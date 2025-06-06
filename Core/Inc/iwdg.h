//Define to prevent recursive inclusion --------------------------------------//
#ifndef __IWDG_H__
#define __IWDG_H__

#ifdef __cplusplus
extern "C" {
#endif

//Includes --------------------------------------------------------------------//
#include "main.h"

//-------- --------------------------------------------------------------------//
void IWDG_Init(void);
void IWDG_Reset (void);

#ifdef __cplusplus
}
#endif

#endif /* __IWDG_H__ */

