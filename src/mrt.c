#include "mrt.h"
#include "clocks.h"
#include "lpc824.h"

void MRT_IRQHandler(void);

void MRT0_Delay(int us) {
   CTRL0 = (0<<0 | 1<<1); //interrupt disable, one-shot interrupt mode
   INTVAL0 = (SYSTEM_CLOCK/1000000 * us) | (1u<<31);
   while(STAT0&(1<<1)); //wait while timer is running
}
