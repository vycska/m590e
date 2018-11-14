#ifndef __CONFIG_H__
#define __CONFIG_H__

#define CONFIG_VERSION (5)
#define CONFIG_PAGE (511)

struct Config_Data {
   char *address_in_ram;
   short size;
};

int config_load(void);
void config_save(void);

#endif
