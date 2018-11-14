#include "vswitch.h"
#include "main.h"
#include "mrt.h"
#include "utils-asm.h"
#include "lpc824.h"

extern volatile unsigned int gInterruptCause;

struct VSwitch_Data vswitch_data;

void VSwitch_Init(void) {
   PIO0_12 = (2<<3 | 1<<5 | 1<<6 | 0<<10 | 0<<11 | 0<<13); //pull-up resistor enabled, hysteresis enabled, input inverted, open-drain mode disabled, bypass input filter, IOCONCLKDIV0
   DIR0 &= (~(1<<12)); //direction is input
   PINTSEL0 = (12<<0); //PIO0_12 selected for pin interrupt
   ISEL &= (~(1<<0)); //pin interrupt selected in PINTSEL0 is edge sensitive
   IST = (1<<0); //clear rising and falling edge detection for pin selected in PINTSEL0
   SIENR = (1<<0); //enable rising edge interrupt for pin selected in PINTSEL0; this is indirect register which operates on IENR register
   CIENF = (1<<0); //disable falling edge interrupt for pin selected in PINTSEL0
   IPR6 = (IPR6&(~(3<<6))) | (1<<6); //PININT0 priority 1 (0 = highest, 3 = lowest)
   ISER0 = (1<<24); //enable PININT0
}

int VSwitch_Pressed(void) {
   return (PIN0>>12)&1;
}

void PININT0_IRQHandler(void) {
   CIENR = (1<<0); //disable rising edge interrupt for pin selected in PINTSEL0
   RISE = (1<<0); //clear rising edge detection
   if(gInterruptCause&(1<<0))
      gInterruptCause &= (~(1<<0));
   else
      gInterruptCause |= (1<<0);
   if(vswitch_data.active==0) {
      vswitch_data.active = 1;
      vswitch_data.delay = 0;
      vswitch_data.duration = 0;
   }
}
