#include "clocks.h"
#include "lpc824.h"

void Clocks_Init(void) {
   MAINCLKSEL = (0<<0); //main clock is IRC oscillator (12MHz)
   MAINCLKUEN = (0<<0); //in order for the update to take effect, first write a zero
   MAINCLKUEN = (1<<0); //update clock source
   SYSAHBCLKDIV = (1<<0); //set system clock divider value 1
   UARTCLKDIV = (1<<0);
   UARTFRGDIV = (0xff<<0);
   UARTFRGMULT = (244<<0);
   //IOCONCLKDIV0 = 250; //this is for clock if glitch filter [MAIN_CLOCK/250 = 48000Hz -> 21us]
   SYSAHBCLKCTRL = (1<<0 | 1<<1 | 1<<2 | 1<<3 | 1<<4 | 1<<6 | 1<<7 | 1<<10 | 1<<14 | 1<<15  | 1<<18 | 1<<24); //enable clock for ROM, RAM0_1, FLASHREG, FLASH, GPIO, SWM, MRT, USART0, USART1, IOCON, ADC
}
