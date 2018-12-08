#include "adc.h"
#include "clocks.h"
#include "main.h"
#include "utils.h"
#include "lpc824.h"

void ADC_Init(void) {
   ADCSEQA_CTRL = 0; //the most important here is to disable sequence A (b31)
   ADCSEQB_CTRL = 0; //the most important here is to disable sequence B (b31)
   PIO0_14 = 0; //the most important here is disabling pd/pu resistors
   PINENABLE0 &= (~(1<<15)); //ADC_2 enabled on PIO0_14
   ADCTHR0_LOW = ADCTHR1_LOW = ADCTHR0_HIGH = ADCTHR1_HIGH = 0; //threshold intervals not used
   ADCCHAN_THRSEL = 0; //threshold intervals not used, but set THR0 (not THR1) to be used for all channels
   ADCINTEN = 0; //interrupts disabled
   ADCTRM = (0<<5); //VDD = 2.7V to 3.6V
   ADC_Calibrate();
   ADCCTRL = (0<<0 | 1<<10 | 0<<30); //divider 0+1, low power mode, no calibration mode
   ADCSEQA_CTRL = (1<<2 | 0<<12 | 0<<26 | 0<<27 | 0<<28 | 0<<29 | 1<<30 | 1u<<31); //select channel 2, conversions are initiated by software, no start, no burst, no singlestep, low priority for sequence A, end of sequence mode, sequence enabled
}

void ADC_Calibrate(void) {
   volatile uint32_t adcctrl = ADCCTRL;
   ADCCTRL = ((SYSTEM_CLOCK/500000-1)<<0 | 0<<10 | 1<<30); //divider for 500kHz, low power mode, calibration mode
   while(ADCCTRL&(1<<30)); //wait for calibration cycle to complete
   ADCCTRL = adcctrl;
}


int ADC_Get(void) {
   ADCSEQA_CTRL |= (1<<26); //start conversion on adc sequence A
   while((ADCDAT2&(1u<<31))==0);
   return (ADCDAT2>>4)&0xfff;
}
