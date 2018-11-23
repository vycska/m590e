#include "output.h"
#include "fifos.h"
#include "m590e.h"
#include "main.h"
#include "uart.h"
#include "utils.h"
#include "utils-asm.h"
#include "lpc824.h"
#include <string.h>

extern volatile unsigned int wakeup_cause;
extern struct M590E_Data m590e_data;

struct Output_Data output_data;

void output(char *buf, enum eOutputSubsystem subsystem, enum eOutputLevel level) {
   int l = strlen(buf), ll;
   if((output_data.mask[eOutputChannelUART][subsystem] & (1<<level)) != 0)
      UART0_Transmit(buf, l, 1);

   if((output_data.mask[eOutputChannelSMS][subsystem]&(1<<level))!=0 && m590e_data.source_number[0]=='+' && level!=eOutputLevelDebug) {
      while(l>0) {
         ll = MIN2(l, FIFO_SMS_MSG_SIZE-1); //-1 nes buferyje dar talpinama '\0'
         Fifo_SMS_Put(buf, ll, m590e_data.source_number);
         l -= ll;
         buf += ll;
      }
      _disable_irq();
      wakeup_cause |= (1<<eWakeupCauseSmsSending);
      _enable_irq();
   }
}
