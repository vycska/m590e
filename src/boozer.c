#include "boozer.h"
#include "lpc824.h"

struct Boozer_Data boozer_data;

void Boozer_Init(void) {
   PINENABLE0 |= (1<<8); //RESETN disabled on PIO0_5
   PIO0_5 = (0<<3 | 0<<5 | 0<<6 | 0<<10 | 0<<11 | 0<<13); //no pu/pd resistor, no hysteresis, input not inverted, no OD mode, no input filter
   DIR0 |= (1<<5); //direction is output
   Boozer_Off();
}

void Boozer_On(int dur) {
   SET0 = (1<<5);
   boozer_data.duration = dur;
   boozer_data.active = 1;
}

void Boozer_Off(void) {
   CLR0 = (1<<5);
   boozer_data.duration = 0;
   boozer_data.active = 0;
}
