#ifndef _SIREN_H_
#define _SIREN_H_

struct Siren_Data {
   int enabled, pir_time;
   volatile int active, duration;
};

void Siren_Init(void);
void Siren_On(int);
void Siren_Off(void);

#endif
