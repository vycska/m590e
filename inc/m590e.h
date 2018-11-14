#ifndef __M590E_H__
#define __M590E_H__

#include "pt.h"

struct M590E_Data {
   char source_number[13];
};

struct Ring_Data {
   volatile int active, delay;
};

void M590E_Init(void);
int Ring_Active(void);

PT_THREAD(Handle_M590E_Ring(struct pt *pt));

#endif
