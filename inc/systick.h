#ifndef __SYSTICK_H__
#define __SYSTICK_H__

struct Systick_Data {
   unsigned int millis;
};

enum Systick_InitMode {
   eSystickInitModeOff  = 0,
   eSystickInitModeOn   = 1
};

void Systick_Init(enum Systick_InitMode);
void Systick_Start(void);
void Systick_Stop(void);

#endif
