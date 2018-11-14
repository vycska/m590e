#include "onewire.h"
#include "main.h"
#include "mrt.h"
#include "utils.h"
#include "lpc824.h"

void onewire_init(void) {
   //pin for 1-wire P0.9
   PINENABLE0 |= (1<<7); //XTALOUT disabled on PIO0_9
   PIO0_9 = (0<<3 | 0<<5 | 0<<6 | 1<<10 | 0<<11 | 0<<13); //no pd/pu, disable hysteresis, input not inverted, open drain mode, bypass input filter, IOCONCLKDIV0
   DIR0 |= (1<<9); //output direction
   SET0 = (1<<9); //at start line is released
}

void onewire_drivelinelow(void) {
   CLR0 = (1<<9);
}

void onewire_releaseline(void) {
   SET0 = (1<<9);
}

unsigned char onewire_getlinevalue(void) {
   return (PIN0>>9)&1;
}

void onewire_delay(int us) {
   MRT0_Delay(us);
}

int onewire_reset(void) { //1 - presence pulse detected, 0 - presence pulse not detected
   int r;
   onewire_drivelinelow();
   onewire_delay(480);
   onewire_releaseline();
   onewire_delay(70);
   r = onewire_getlinevalue();
   onewire_delay(410);
   return !r; //if line was drived low [value 0], we return that presence pulse detected
}

unsigned char onewire_readbit(void) {
   unsigned char b;
   onewire_drivelinelow();
   onewire_delay(6);
   onewire_releaseline();
   onewire_delay(9);
   b = onewire_getlinevalue();
   onewire_delay(55);
   return b;
}

void onewire_writebit(unsigned char b) {
   onewire_drivelinelow();
   onewire_delay(b ? 6 : 60);
   onewire_releaseline();
   onewire_delay(b ? 64 : 10);
}

unsigned char onewire_receivebyte(void) {
   unsigned char b = 0;
   int i;
   for(i = 0; i < 8; i++)
      if(onewire_readbit())
         b |= (1 << i);
   return b;
}

void onewire_sendbyte(unsigned char b) {
   int i;
   for(i = 0; i < 8; i++)
      onewire_writebit((b >> i) & 1);
}

unsigned char onewire_crc8(unsigned char *d, int k) {
   unsigned char m, r = 0, p = 0x31;
   int i, j;
   for(i = 0; i <= k; i++) {    //<=k because of additional byte of 0s
      for(j = 0; j < 8; j++) {  //bit order is reflected [started from LSB]
         m = (r >> 7) & 1;      //what is the msb (which will be shifted out)
         r <<= 1;               //shifting left
         if(i < k && d[i] & (1 << j))
            r |= 1;
         if(m)
            r ^= p;
      }
   }
   return reflect_byte(r);
}
