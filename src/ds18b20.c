#include "ds18b20.h"
#include "onewire.h"
#include "utils-asm.h"

//select devices -- all or specific [using a serial number with "match ROM" 55h] or binary search ["search ROM" F0h] -- these are ROM commands
//if only one device -- "read rom" 33h or "skip ROM" CCh
//device is now isolated
//function commands: "convert T" 44h, "write scratchpad" 4Eh, "read scratchpad" BEh, "copy scratchpad" 48h

void DS18B20_Init(void) {
   onewire_init();
}

int DS18B20_ReadROM(unsigned char *d) { //d t.b. 8 elementu baitu masyvas
   int i;
   unsigned int primask;

   primask = _start_critical();
   onewire_reset();
   onewire_sendbyte(0x33);      //read rom
   for(i = 0; i < 8; i++)       //8 baitai duomenu, paskutinis crc
      d[i] = onewire_receivebyte();
   _end_critical(primask);
   return onewire_crc8(d, 7) == d[7] ? DS18B20_OK : DS18B20_ERROR;
}

void DS18B20_ConvertTAll(void) {
   unsigned int primask;
   primask = _start_critical();
   onewire_reset();
   onewire_sendbyte(0xcc);      //skip rom
   onewire_sendbyte(0x44);      //convert T
   _end_critical(primask);
}

int DS18B20_ReadScratchpad(unsigned char *m, unsigned char *d) { //m yra NULL arba 8 baitu masyvas, data - 9 baitu masyvas
   int i;
   unsigned int primask;
   
   primask = _start_critical();
   onewire_reset();
   if(m == 0)
      onewire_sendbyte(0xcc);   //skip rom
   else {                       //match rom
      onewire_sendbyte(0x55);
      for(i = 0; i < 8; i++)
         onewire_sendbyte(m[i]);
   }
   onewire_sendbyte(0xbe);      //read scratchpad
   for(i = 0; i < 9; i++)
      d[i] = onewire_receivebyte();
   _end_critical(primask);
   return onewire_crc8(d, 8) == d[8] ? DS18B20_OK : DS18B20_ERROR;
}

double DS18B20_GetTemperature(unsigned char *d) {
   return (*(short *)d) / 16.0; //pirmi du baitai yra temperatura
}
