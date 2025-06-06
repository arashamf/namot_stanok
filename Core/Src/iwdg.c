
// Includes ------------------------------------------------------------------//
#include "iwdg.h"
#include "stm32f1xx_ll_iwdg.h"

//----------------------------------------------------------------------------//
void IWDG_Init(void)
{
  LL_IWDG_Enable(IWDG);
  LL_IWDG_EnableWriteAccess(IWDG);
  LL_IWDG_SetPrescaler(IWDG, LL_IWDG_PRESCALER_128);
  LL_IWDG_SetReloadCounter(IWDG, 4095);
  while (LL_IWDG_IsReady(IWDG) != 1){}
  LL_IWDG_ReloadCounter(IWDG);

}

//----------------------------------------------------------------------------------------------------//
void IWDG_Reset (void)
{
	LL_IWDG_ReloadCounter(IWDG);
}