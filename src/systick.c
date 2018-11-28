#include "systick.h"
#include "clocks.h"
#include "m590e.h"
#include "main.h"
#include "vswitch.h"
#include "lpc824.h"

extern volatile unsigned int wakeup_cause;
extern struct M590E_Data m590e_data;
extern struct VSwitch_Data vswitch_data;

struct SysTick_Data systick_data;

void SysTick_Init(int start) {
   SYST_CSR = (0<<0 | 0<<1); //counter disabled, interrupt disabled
   SHPR3 = (SHPR3 & (~(0x3u<<30))) | (1u<<30); //set the priority
   if(start) SysTick_Start();
}

void SysTick_Start(void) {
   systick_data.millis = 0;
   SYST_RVR = SYSTEM_CLOCK/1000 -1; //1ms
   SYST_CVR = 0; //writing any value clear counter and the COUNTFLAG bit
   SYST_CSR = (1<<0 | 1<<1 | 1<<2); //enable counter, enable interrupt, clock source is system clock
}

void SysTick_Stop(void) {
   SYST_CSR = (0<<0 | 0<<1); //disable counter, disable interrupt
}

void SysTick_Handler(void) {
   systick_data.millis += 1;
   if(vswitch_data.active) {
      vswitch_data.duration += 1;
      if(VSwitch_Pressed()) {
         vswitch_data.delay = 0;
      }
      else if(++vswitch_data.delay==500) {
         vswitch_data.active = 0;
         vswitch_data.delay = 0;
         vswitch_data.duration -= 500;
         RISE = (1<<0);
         SIENR = (1<<0);
         wakeup_cause |= (1<<eWakeupCauseVSwitchReleased);
      }
   }
   if(m590e_data.ring_active) {
      m590e_data.ring_duration += 1;
      if(Ring_Active()) {
         m590e_data.ring_delay = 0;
      }
      else if(++m590e_data.ring_delay == 3000) {
         m590e_data.ring_active = 0;
         m590e_data.ring_delay = 0;
         m590e_data.ring_duration -= 3000;
         FALL = (1<<1);
         SIENF = (1<<1);
         wakeup_cause |= (1<<eWakeupCauseRingEnded);
      }
   }
}
