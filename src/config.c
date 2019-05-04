#include "config.h"
#include "boozer.h"
#include "iap.h"
#include "led.h"
#include "m590e.h"
#include "output.h"
#include "siren.h"
#include "uart.h"
#include "utils.h"
#include "lpc824.h"
#include <string.h>

extern struct Boozer_Data boozer_data;
extern struct M590E_Data m590e_data;
extern struct Output_Data output_data;
extern struct Siren_Data siren_data;

struct Config_Data config_data[] = {
   {(unsigned char*)output_data.mask,                    sizeof(output_data.mask)},
   {(unsigned char*)m590e_data.periodic_sms,             sizeof(m590e_data.periodic_sms)},
   {(unsigned char*)&m590e_data.periodic_sms_interval,   sizeof(m590e_data.periodic_sms_interval)},
   {(unsigned char*)&m590e_data.pir_sms_interval,        sizeof(m590e_data.pir_sms_interval)},
   {(unsigned char*)&boozer_data.enabled,                sizeof(boozer_data.enabled)},
   {(unsigned char*)&siren_data.enabled,                 sizeof(siren_data.enabled)},
   {(unsigned char*)&siren_data.pir_time,                sizeof(siren_data.pir_time)}
};

enum Config_Result config(enum Config_Mode mode) {
   int i, error;
   short total_size, config_version;
   unsigned int p;
   for(config_version=CONFIG_VERSION,total_size=0,i=0; i<sizeof(config_data)/sizeof(struct Config_Data); i++)
      total_size += config_data[i].size;
   for(error=0,p=32768,i=-2; i<(int)(sizeof(config_data)/sizeof(struct Config_Data)) && !error; i++) {
      switch(i) {
         case -2:
            p -= 2;
            if(mode==eConfigModeLoad)
               error = *((short*)p)!=config_version;
            else
               error = iflash_write(p, (unsigned char*)&config_version, 2)!=1;
            break;
         case -1:
            p -= 2;
            if(mode==eConfigModeLoad)
               error = *((short*)p)!=total_size;
            else
               error = iflash_write(p, (unsigned char*)&total_size, 2)!=1;
            break;
         default:
            p -= config_data[i].size;
            if(mode==eConfigModeLoad)
               memcpy(config_data[i].address_in_ram, (void*)p, config_data[i].size);
            else
               error = iflash_write(p, config_data[i].address_in_ram, config_data[i].size)!=1;
            break;
      }
   }
   return error==0 ? eConfigResultOK : eConfigResultError;
}
