#include "output.h"
#include "fifos.h"
#include "m590e.h"
#include "main.h"
#include "uart.h"
#include "utils.h"
#include "utils-asm.h"
#include "lpc824.h"
#include <ctype.h>
#include <string.h>

extern struct M590E_Data m590e_data;
extern struct Main_Data main_data;

struct Output_Data output_data;

void output(char *buf, enum eOutputSubsystem subsystem, enum eOutputLevel level) {
   int i, l=strlen(buf);
   for(i=l-1;i>=0 && !isprint(buf[i]); l--, i--);
   if((output_data.mask[eOutputChannelUART][subsystem] & (1<<level)) != 0)
      UART0_Transmit(buf, l, 1);

   if((output_data.mask[eOutputChannelSMS][subsystem]&(1<<level))!=0 && m590e_data.source_number!=0) { //sms gali buti siunciamas tik tada, kai tai yra atsakymas i atsiusta sms uzklausa/komanda
      Fifo_SMS_Put(buf, l, m590e_data.source_number);
      _disable_irq();
      main_data.wakeup_cause |= (1<<eWakeupCauseSmsSending);
      _enable_irq();
   }
}
