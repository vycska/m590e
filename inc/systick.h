#ifndef __SYSTICK_H__
#define __SYSTICK_H__

struct SysTick_Data {
   unsigned int millis;
};

void SysTick_Init(int);
void SysTick_Start(void);
void SysTick_Stop(void);

#endif
