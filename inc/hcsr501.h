#ifndef __HCSR501_H__
#define __HCSR501_H__

struct HCSR501_Data {
   volatile int active,delay,duration;
};

void HCSR501_Init(void);
int HCSR501_Active(void);

#endif
