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
   int l=strlen(buf), ll;
   //for(i=l-1;i>=0 && !isprint(buf[i]); l--, i--);
   if((output_data.mask[eOutputChannelUART][subsystem] & (1<<level)) != 0)
      UART0_Transmit(buf, l, 1);

   if((output_data.mask[eOutputChannelSMS][subsystem]&(1<<level))!=0 && strlen(m590e_data.source_number)==MAX_SRC_SIZE-1) { //sms gali buti siunciamas tik tada, kai tai yra atsakymas i atsiusta sms uzklausa/komanda
      while(l>0) {
         ll = MIN2(l, FIFO_SMS_MSG_SIZE-1); //-1 nes buferyje dar talpinama '\0'
         Fifo_SMS_Put(buf, ll, m590e_data.source_number);
         l -= ll;
         buf += ll;
      }
      _disable_irq();
      main_data.wakeup_cause |= (1<<eWakeupCauseSmsSending);
      _enable_irq();
   }
}
