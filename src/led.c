#include "led.h"
#include "lpc824.h"

void LED_Init(int mode) {
   PINENABLE0 |= (1<<3 | 1<<16); //ACMP_I4 and ADC_3 disabled on pin PIO0_23
   PIO0_23 = (0<<3 | 0<<5 | 0<<6 | 0<<10 | 0<<11 | 0<<13); //no pu/pd, disable hysteresis, input not inverted, disable od, bypass input filter, peripheral clock divider 0
   DIR0 |= (1<<23); //set output direction
   if(mode == 0)
      LED_Off();
   else
      LED_On();
}

void LED_On(void) {
   SET0 = (1<<23);
}

void LED_Off(void) {
   CLR0 = (1<<23);
}

void LED_Toggle(void) {
   NOT0 = (1<<23);
}
