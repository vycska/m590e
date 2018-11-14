#ifndef __VSWITCH_H__
#define __VSWITCH_H__

struct VSwitch_Data {
   volatile int active,delay,duration;
};

void VSwitch_Init(void);
int VSwitch_Pressed(void);

#endif
