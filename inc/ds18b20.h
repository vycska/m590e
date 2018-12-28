#ifndef __DS18B20_H__
#define __DS18B20_H__

#define DS18B20_OK 0
#define DS18B20_ERROR 1

void DS18B20_Init(void);
int DS18B20_ReadROM(unsigned char *);
void DS18B20_ConvertTAll(void);
int DS18B20_ReadScratchpad(unsigned char *, unsigned char *);
double DS18B20_GetTemperature(unsigned char *);

#endif
