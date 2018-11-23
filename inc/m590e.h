#ifndef __M590E_H__
#define __M590E_H__

#include "pt.h"
#include <stdarg.h>

#define MAX_SMS_SIZE 140
#define MAX_SRC_SIZE 13

struct M590E_Data {
   char source_number[MAX_SRC_SIZE];
   int mutex;
};

struct Ring_Data {
   volatile int active, delay, duration;
};

void M590E_Init(void);
int Ring_Active(void);
PT_THREAD(M590E_Send(struct pt *pt, char *msg, int len, int k, char **lines, int delay));
PT_THREAD(M590E_SMSInit(struct pt *pt, int *ok));
PT_THREAD(M590E_SMSParse(struct pt *pt));
PT_THREAD(M590E_SMSSend(struct pt *pt));

#endif
