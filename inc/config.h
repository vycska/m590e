#ifndef __CONFIG_H__
#define __CONFIG_H__

#define CONFIG_VERSION (3)

struct Config_Data {
   unsigned char *address_in_ram;
   int size;
};

int config(int);

#endif
