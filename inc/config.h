#ifndef __CONFIG_H__
#define __CONFIG_H__

#define CONFIG_VERSION (5)

struct Config_Data {
   unsigned char *address_in_ram;
   int size;
};

enum Config_Mode {
   eConfigModeLoad = 0,
   eConfigModeSave = 1
};

enum Config_Result {
   eConfigResultError   = 0,
   eConfigResultOK      = 1
};

enum Config_Result config(enum Config_Mode);

#endif
