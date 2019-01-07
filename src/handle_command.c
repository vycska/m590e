#include "handle_command.h"
#include "adc.h"
#include "boozer.h"
#include "config.h"
#include "ds18b20.h"
#include "dump.h"
#include "iap.h"
#include "m590e.h"
#include "main.h"
#include "mrt.h"
#include "output.h"
#include "systick.h"
#include "uart.h"
#include "utils.h"
#include "lpc824.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

extern char _flash_start, _flash_end, _ram_start, _ram_end, _heap_start;
extern struct M590E_Data m590e_data;
extern struct Main_Data main_data;
extern struct Output_Data output_data;
extern struct Systick_Data systick_data;

void Handle_Command(char *pString) {
   char buf[256];
   unsigned char data[9];
   int i, j, l;
   unsigned int t, params[12] = {0};
   enum Config_Result config_save_result;

   mysprintf(buf, "<< %s >>", pString);
   output(buf, eOutputSubsystemSystem, eOutputLevelImportant);

   params_fill(pString, params);

   switch(crc16((unsigned char *)params[1], strlen((char *)params[1]))) {
      case 0x6bd8: //reset
         System_Reset();
         break;
      case 0x57e6: //millis
         mysprintf(buf, "%u", systick_data.millis);
         output(buf, eOutputSubsystemSystem, eOutputLevelImportant);
         break;
      case 0x426e: //config_save
         config_save_result = config(eConfigModeSave);
         output(config_save_result==eConfigResultOK ? "ok" : "error",eOutputSubsystemSystem, eOutputLevelImportant);
         break;
      case 0x1804: //iflash_write
         if(params_count(params)>2 && params_count(params)<=6) {
            unsigned char data[4];
            int res;
            for(i=0; i<params_count(params)-2; i++)
               data[i] = params[3+i];
            res = iflash_write(params[2], data, i);
            mysprintf(buf, res?"ok":"error");
            output(buf, eOutputSubsystemSystem, eOutputLevelImportant);
         }
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
      case 0x042e: //ip [init print]
         Init_Print();
         break;
      case 0x350b: //cause
         mysprintf(buf, "0x%x", main_data.wakeup_cause);
         output(buf, eOutputSubsystemSystem, eOutputLevelImportant);
         break;
      case 0x7327: //time
         t = M590E_Get_UnixTime();
         mysprintf(buf, "%u", t);
         output(buf, eOutputSubsystemSystem, eOutputLevelImportant);
         break;
      case 0x6ede: // pir [pir detected motion]
         output("motion detected", eOutputSubsystemSystem, eOutputLevelImportant);
         break;
      case 0xaded: //om [output mask]
         if(params_count(params)==1) {
            mysprintf(buf,"eOutputChannelUART %d",(int)eOutputChannelUART);
            output(buf, eOutputSubsystemSystem, eOutputLevelImportant);
            mysprintf(buf,"eOutputChannelSMS %d",(int)eOutputChannelSMS);
            output(buf, eOutputSubsystemSystem, eOutputLevelImportant);
            mysprintf(buf,"M590E %d",(int)eOutputSubsystemM590E);
            output(buf, eOutputSubsystemSystem, eOutputLevelImportant);
            mysprintf(buf,"System %d",(int)eOutputSubsystemSystem);
            output(buf, eOutputSubsystemSystem, eOutputLevelImportant);
            mysprintf(buf,"VSwitch %d",(int)eOutputSubsystemVSwitch);
            output(buf, eOutputSubsystemSystem, eOutputLevelImportant);
            mysprintf(buf,"HCSR501 %d",(int)eOutputSubsystemHCSR501);
            output(buf, eOutputSubsystemSystem, eOutputLevelImportant);
            mysprintf(buf,"Debug %d",(int)eOutputLevelDebug);
            output(buf, eOutputSubsystemSystem, eOutputLevelImportant);
            mysprintf(buf,"Normal %d",(int)eOutputLevelNormal);
            output(buf, eOutputSubsystemSystem, eOutputLevelImportant);
            mysprintf(buf,"Important %d",(int)eOutputLevelImportant);
            output(buf, eOutputSubsystemSystem, eOutputLevelImportant);
            for(i=0; i<(int)eOutputChannelLast; i++)
               for(j=0; j<(int)eOutputSubsystemLast; j++) {
                  mysprintf(buf, "[%d][%d] : %u", i, j, output_data.mask[i][j]);
                  output(buf, eOutputSubsystemSystem, eOutputLevelImportant);
               }
         }
         if(params_count(params)==2 && params[2]<(1<<3)) {
            for(i=0; i<(int)eOutputChannelLast; i++)
               for(j=0; j<(int)eOutputSubsystemLast; j++)
                  output_data.mask[i][j] = params[2];
         }
         else if(params_count(params)==3 && params[2]<eOutputChannelLast && params[3]<(1<<3)) {
            for(i=0; i<(int)eOutputSubsystemLast; i++)
               output_data.mask[params[2]][i] = params[3];
         }
         else if(params_count(params)==4 && params[2]<eOutputChannelLast && params[3]<eOutputSubsystemLast && params[4]<(1<<3)) {
            output_data.mask[params[2]][params[3]] = params[4];
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
            double v = ADC_Get()/4095.0*3.3 * 2; //*2 del itampos pasidalinimo tarp dvieju vienodu [3.3k atrodo] varzu
            mysprintf(buf, "%f2 V", (char*)&v);
            output(buf, eOutputSubsystemSystem, eOutputLevelImportant);
         }
         break;
      case 0x67bf: //t [ds18b20 temperature]
         DS18B20_ConvertTAll();
         MRT0_Delay(750*1000); //conversion time
         if(DS18B20_ReadScratchpad(0,data)==DS18B20_OK) {
            double v;
            v = DS18B20_GetTemperature(data);
            mysprintf(buf, "%f1 C",(char*)&v);
            output(buf, eOutputSubsystemSystem, eOutputLevelImportant);
         }
         break;
      case 0xad7e: //m [m590e]
         for(l=0,i=2; i<=params_count(params); i++) {
            if(params_integer(i, params))
               l += mysprintf(buf+l, i<params_count(params)?"%u ":"%c", params[i]);
            else
               l += mysprintf(buf+l, "%s%c", (char*)params[i], i<params_count(params)?' ':'\r');
         }
         M590E_Send_Blocking(buf, l, 9, 5000);
         break;
      case 0xa4be: //p [periodic sms]
         t = strncmp(m590e_data.source_number, "", 1); //sitas bus naudojamas nustatyti ar komanda yra is gauto sms
         switch(params_count(params)) {
            case 1:
               if(t == 0) {
                  mysprintf(buf, "period: %d", m590e_data.periodic_sms_interval);
                  output(buf, eOutputSubsystemSystem, eOutputLevelImportant);
                  for(i=0; i<PERIODIC_SMS_RECIPIENTS; i++) {
                     if(strncmp(m590e_data.periodic_sms[i].src, "", 1) != 0) {
                        l = mysprintf(buf, "%s: ", m590e_data.periodic_sms[i].src);
                        for(j=0; j<PERIODIC_SMS_COMMANDS; j++) {
                           if(strncmp(m590e_data.periodic_sms[i].commands[j], "", 1) != 0)
                              l += mysprintf(buf+l, "%s ", m590e_data.periodic_sms[i].commands[j]);
                        }
                        output(buf, eOutputSubsystemSystem, eOutputLevelImportant);
                     }
                  }
               }
               else {
                  M590E_Periodic_Add(m590e_data.source_number, ""); //pridejimas bet be komandos reiks, kad bus gaunami pir signalai
               }
               break;
            case 2:
               if(t == 0) {
                  if(params_integer(2, params))
                     M590E_Periodic_Interval(params[2]);
                  else {
                     if(strlen((char*)params[2]) == MAX_SRC_SIZE-1) {
                        M590E_Periodic_Add((char*)params[2], "");
                     }
                  }
               }
               else { //gautas sms
                  if(params_integer(2, params)) {
                     if(params[2] == 0) {
                        M590E_Periodic_Clear(m590e_data.source_number);
                     }
                     else {
                        M590E_Periodic_Interval(params[2]);
                     }
                  }
                  else {
                     M590E_Periodic_Add(m590e_data.source_number, (char*)params[2]);
                  }
               }
               break;
            case 3:
               if(t==0 && !params_integer(2, params) && strlen((char*)params[2])==MAX_SRC_SIZE-1) {
                  if(params_integer(3, params)) {
                     if(params[3] == 0) {
                        M590E_Periodic_Clear((char*)params[2]);
                     }
                  }
                  else {
                     M590E_Periodic_Add((char*)params[2], (char*)params[3]);
                  }
               }
               break;
         }
         break;
      case 0xba23: //dump
         dump_print();
         break;
      case 0xa33e: //z [boozer]
         if(params_count(params)==2 && params_integer(2, params)) {
            Boozer_On(params[2]);
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
