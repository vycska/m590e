#ifndef __M590E_H__
#define __M590E_H__

#include "pt.h"
#include <stdarg.h>

#define MAX_SMS_SIZE 140
#define MAX_SRC_SIZE 13

#define PERIODIC_SMS_RECIPIENTS 3
#define PERIODIC_SMS_COMMANDS 3
#define PERIODIC_SMS_COMMAND_SIZE 16

struct Periodic_SMS {
   char src[MAX_SRC_SIZE];
   char commands[PERIODIC_SMS_COMMANDS][PERIODIC_SMS_COMMAND_SIZE];
};

struct M590E_Data {
   char response[3][64],
        source_number[MAX_SRC_SIZE];
   int mutex;
   volatile int ring_active, ring_delay, ring_duration;
   int periodic_sms_interval;
   struct Periodic_SMS periodic_sms[PERIODIC_SMS_RECIPIENTS];
};

void M590E_Init(void);
int Ring_Active(void);
void M590E_Sleep_Enter(void);
void M590E_Sleep_Exit(void);
void M590E_Send_Blocking(char *msg, int len, int k, int delay);
PT_THREAD(M590E_Send(struct pt *pt, char *msg, int len, int k, int delay));
PT_THREAD(M590E_SMSInit(struct pt *pt, int *ok));
PT_THREAD(M590E_SMSParse(struct pt *pt));
PT_THREAD(M590E_SMSSend(struct pt *pt));
PT_THREAD(M590E_SMSPeriodic(struct pt *pt));

#endif
