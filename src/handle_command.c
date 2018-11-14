#include "handle_command.h"
#include "config.h"
#include "ds18b20.h"
#include "iap.h"
#include "m590e.h"
#include "main.h"
#include "mrt.h"
#include "output.h"
#include "uart.h"
#include "utils.h"
#include "lpc824.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

extern char _flash_start, _flash_end, _ram_start, _ram_end, _heap_start;
extern volatile unsigned int gInterruptCause;
extern volatile unsigned int millis;
extern struct Output_Data output_data;

void Handle_Command(char *pString) {
   char buf[256];
   unsigned char data[9];
   int i, l;
   unsigned int t,params[12] = {0};

   mysprintf(buf, "<< %s >>", pString);
   output(buf, eOutputSubsystemSystem, eOutputLevelImportant);

   params_fill(pString, params);

   switch(crc16((unsigned char *)params[1], strlen((char *)params[1]))) {
      case 0x6bd8: //reset
         SystemReset();
         break;
      case 0x57e6: //millis
         mysprintf(buf, "%u", millis);
         output(buf, eOutputSubsystemSystem, eOutputLevelImportant);
         break;
      case 0x426e: //config_save
         config_save();
         break;
      case 0x62bf: //x [value at address]
         if(params_count(params)==2 && !params_integer(2,params)) {
            for(t=0, l=1,i=strlen((char*)params[2])-1; i>=2; l*= 16, i--)
               t += l * (((char*)params[2])[i]>='0' && ((char*)params[2])[i]<='9' ? (((char*)params[2])[i]-'0') : (((char*)params[2])[i]>='a' && ((char*)params[2])[i]<='f' ? (10+((char*)params[2])[i]-'a') : (0)));
            if((t&3)==0 && ((t>=(unsigned int)&_flash_start && t<=(unsigned int)&_flash_end) || (t>=(unsigned int)&_ram_start && t<=(unsigned int)&_ram_end))) {
               mysprintf(buf,"0x%x : 0x%x",t,*((unsigned int*)t));
               output(buf, eOutputSubsystemSystem, eOutputLevelImportant);
            }
         }
         break;
      case 0x696c: //oc [output channel]
         if(params_count(params)==1) {
            mysprintf(buf, "channel_mask: %d", output_data.channel_mask);
            output(buf, eOutputSubsystemSystem, eOutputLevelImportant);
            mysprintf(buf, "UART: %d", (int)eOutputChannelUART);
            output(buf, eOutputSubsystemSystem, eOutputLevelImportant);

         }
         else if(params_count(params)==2 && params[2]<=((1<<eOutputChannelLast)-1)) {
            output_data.channel_mask = params[2];
         }
         break;
      case 0xaded: //om [output mask]
         if(params_count(params)==2) {
            for(i=0; i<(int)eOutputSubsystemLast; i++)
               output_data.subsystem_mask[i] = params[2];
         }
         else if(params_count(params)==3 && params[2]<eOutputSubsystemLast) {
            output_data.subsystem_mask[params[2]] = params[3];
         }
         else {
            mysprintf(buf,"ADC %d",(int)eOutputSubsystemADC);
            output(buf, eOutputSubsystemSystem, eOutputLevelImportant);
            mysprintf(buf,"DS18B20 %d",(int)eOutputSubsystemDS18B20);
            output(buf, eOutputSubsystemSystem, eOutputLevelImportant);
            mysprintf(buf,"System %d",(int)eOutputSubsystemSystem);
            output(buf, eOutputSubsystemSystem, eOutputLevelImportant);
            mysprintf(buf,"VSwitch %d",(int)eOutputSubsystemVSwitch);
            output(buf, eOutputSubsystemSystem, eOutputLevelImportant);
            mysprintf(buf,"Debug %d",(int)eOutputLevelDebug);
            output(buf, eOutputSubsystemSystem, eOutputLevelImportant);
            mysprintf(buf,"Normal %d",(int)eOutputLevelNormal);
            output(buf, eOutputSubsystemSystem, eOutputLevelImportant);
            mysprintf(buf,"Important %d",(int)eOutputLevelImportant);
            output(buf, eOutputSubsystemSystem, eOutputLevelImportant);
            for(i=0; i<(int)eOutputSubsystemLast; i++) {
               mysprintf(buf, "[%d] %u",i,(unsigned int)output_data.subsystem_mask[i]);
               output(buf, eOutputSubsystemSystem, eOutputLevelImportant);
            }
         }
         break;
      case 0x7f7e: //iap_info
         t = iap_read_part_id();
         l = mysprintf(&buf[0], "Part id: 0x%x\r\n", t);
         t = iap_read_boot_code_version();
         l += mysprintf(&buf[l], "Boot code version: %d.%d\r\n", (t >> 8) & 0xff, t & 0xff);
         t = (unsigned int)iap_read_uid();
         l += mysprintf(&buf[l], "UID: %u %u %u %u", *((unsigned int *)t + 0), *((unsigned int *)t + 1), *((unsigned int *)t + 2), *((unsigned int *)t + 3));
         output(buf, eOutputSubsystemSystem, eOutputLevelImportant);
         break;
      case 0x57e5: //crc [crc16]
         mysprintf(buf, "0x%x", (unsigned int)crc16((unsigned char *)params[2], strlen((char *)params[2])));
         output(buf, eOutputSubsystemSystem, eOutputLevelImportant);
         break;
      case 0xa93e: //b [battery]
         if(params_count(params)==1) {
            float v;
            ADCSEQA_CTRL |= (1<<26); //start conversion on adc sequence A
            while((ADCDAT2&(1u<<31))==0);
            v = ((ADCDAT2>>4)&0xfff)/4095.0f*3.3f * 2;
            mysprintf(buf, "%f2 V", (char*)&v);
            output(buf, eOutputSubsystemSystem, eOutputLevelImportant);
         }
         break;
      case 0x67bf: //t [ds18b20 temperature]
         DS18B20_ConvertTAll();
         MRT0_Delay(750*1000); //conversion time
         if(DS18B20_ReadScratchpad(0,data)==DS18B20_OK) {
            float v;
            v = DS18B20_GetTemperature(data);
            mysprintf(buf, "ds18b20 t: %f2 C",(char*)&v);
            output(buf, eOutputSubsystemDS18B20, eOutputLevelDebug);
         }
         break;
      case 0xad7e: //m [m590e]
         if(params_count(params)>1 && !params_integer(2, params)) {
            l = strlen((char*)params[2]);
            strcpy((char*)buf, (char*)params[2]);
            if(params_count(params) == 2) {
               buf[l++] = 13;
            }
            else {
               for(i=3; i<=params_count(params); i++) {
                  if(params_integer(i, params)) {
                     buf[l++] = params[i];
                  }
                  else {
                     buf[l++] = ' ';
                     strcpy((char*)(buf+l), (char*)params[i]);
                     l += strlen((char*)params[i]);
                  }
               }
            }
            UART1_Transmit(buf, l);
         }
         break;
      default:
         output("unknown command", eOutputSubsystemSystem, eOutputLevelImportant);
   }
}

void params_fill(char *s, unsigned int *params) {
   char *p,                     //pointer
     l,                         //length
     d,                         //all digits
     k;                         //# params
   int i;                       //iterator

   for(p=s, d=1, k=0, l=strlen(s), i=0; i<=l; i++) {
      if(s[i]==' ' || i==l) {
         s[i] = 0;
         params[k+1] = d ? (params[0]|=(1<<(16+k+1)),atoi(p)) : (unsigned int)p;
         k += 1;
         d = 1;
         p = &s[i+1];
      }
      else {
         d = d && isdigit(s[i]);
      }
   }
   params[0] |= (k&0xff);
}

int params_count(unsigned int *params) { //kiek parametru uzpildyta po params_fill ivykdymo
   return params[0] & 0xff;
}

int params_integer(char k, unsigned int *params) { //ar paremetras #k yra integer'is, jei ne -- jis yra pointeris i stringa
   return ((params[0] >> 16) & (1 << k)) != 0;
}
