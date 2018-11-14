#include "adc.h"
#include "clocks.h"
#include "main.h"
#include "utils.h"
#include "lpc824.h"

void ADC_Init(void) {
   ADCSEQA_CTRL &= (~(1u<<31)); //disable sequence A
   ADCSEQB_CTRL &= (~(1u<<31)); //disable sequence B
   PIO0_14 = 0; //the most important here is disabling pd/pu resistors
   PINENABLE0 &= (~(1<<15)); //ADC_2 enabled on PIO0_14
   ADCTHR0_LOW = ADCTHR1_LOW = ADCTHR0_HIGH = ADCTHR1_HIGH = 0; //threshold intervals not used
   ADCCHAN_THRSEL = 0; //threshold intervals not used, but set THR0 (not THR1) to be used for all channels
   ADCTRM &= (~(1<<5)); //VDD = 2.7V to 3.6V
   ADCCTRL = ((SYSTEM_CLOCK/500000-1)<<0 | 0<<10 | 1<<30); //divider for 500kHz, low power mode, calibration mode
   while(ADCCTRL&(1<<30)); //wait for calibration cycle to complete
   ADCCTRL = (1<<0 | 1<<10); //divider 1+1, low power mode
   ADCSEQA_CTRL = (1<<2 | 0<<12 | 1<<18 | 0<<19 | 0<<26 | 0<<27 | 0<<28 | 0<<29 | 1<<30 | 1u<<31); //select channel 2, conversions are initiated by software, polarity is positive edge, enable synchronization, no start, no burst, no singlestep, low priority for sequence A, end of sequence mode, sequence enabled
   ADCINTEN = 0; //interrupts disabled
}
