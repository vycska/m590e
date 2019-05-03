#include "siren.h"
#include "lpc824.h"

struct Siren_Data siren_data;

void Siren_Init(void) {
   PINENABLE0 |= (1<<1) | (1<<9); //ACMP_I2, CLKIN disabled on PIO0_1
   PIO0_1 = (0<<3 | 0<<5 | 0<<6 | 0<<10 | 0<<11 | 0<<13); //no pu/pd resistor, no hysteresis, input not inverted, no OD mode, no input filter
   DIR0 |= (1<<1); //direction is output
   Siren_Off();
}

void Siren_On(int dur) {
   SET0 = (1<<1);
   siren_data.duration = dur;
   siren_data.active = 1;
}

void Siren_Off(void) {
   CLR0 = (1<<1);
   siren_data.duration = 0;
   siren_data.active = 0;
}
