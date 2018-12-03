#ifndef __LED_H__
#define __LED_H__

enum Led_Mode {
   eLedModeOff = 0,
   eLedModeOn  = 1
};

void LED_Init(enum Led_Mode);
void LED_On(void);
void LED_Off(void);
void LED_Toggle(void);

#endif
