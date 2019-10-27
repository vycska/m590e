#include "hcsr501.h"
#include "boozer.h"
#include "m590e.h"
#include "main.h"
#include "siren.h"
#include "lpc824.h"

extern struct Boozer_Data boozer_data;
extern struct M590E_Data m590e_data;
extern struct Main_Data main_data;
extern struct Siren_Data siren_data;

struct HCSR501_Data hcsr501_data;

void HCSR501_Init(void) {
   PINENABLE0 |= (1<<6); //XTALIN disabled on PIO0_8
   PIO0_8 = (1<<3 | 0<<5 | 0<<6 | 0<<10 | 0<<11 | 0<<13); //pd resistor, no hysteresis, input not inverted, no OD mode, no input filter
   DIR0 &= (~(1<<8)); //direction is input
   PINTSEL2 = (8<<0); //PIO0_8 selected for pin interrupt
   ISEL &= (~(1<<2)); //pin interrupt selected in PINTSEL2 is edge sensitive
   IST = (1<<2); //clear rising and falling edge detection for pin selected in PINTSEL2
   SIENR = (1<<2); //enable rising edge interrupt for pin selected in PINTSEL2; this is indirect register which operates on IENR register
   CIENF = (1<<2); //disable falling edge interrupt for pin selected in PINTSEL2
   IPR6 = (IPR6&(~(3<<22))) | (1<<22); //PININT2 priority 1 (0 = highest, 3 = lowest)
   ICPR0 = (1<<26); //clear pending interrupts
   ISER0 = (1<<26); //enable PININT2
}

int HCSR501_Active(void) {
   return (PIN0>>8)&1;
}

void PININT2_IRQHandler(void) {
   CIENR = (1<<2); //disable rising edge interrupt for pin selected in PINTSEL2
   RISE = (1<<2); //clear rising edge detection
   ICPR0 = (1<<26); //clear pending interrupts
   if(hcsr501_data.active == 0) {
      main_data.wakeup_cause |= (1<<eWakeupCauseHCSR501Start);
      if(boozer_data.enabled) {
         Boozer_On(-1);
      }
      hcsr501_data.active = 1;
      hcsr501_data.delay = 0;
      hcsr501_data.duration = 0;
   }
}
