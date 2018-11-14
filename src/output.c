#include "output.h"
#include "uart.h"
#include "utils.h"
#include "utils-asm.h"
#include "lpc824.h"
#include <string.h>

struct Output_Data output_data;

void output(char *buf, enum eOutputSubsystem subsystem, enum eOutputLevel level) {
   int i;
   for(i=0; i<eOutputChannelLast; i++)
      if((output_data.channel_mask & (1u<<i))==0 && (output_data.subsystem_mask[subsystem] & (1u<<level))==0)
         switch(i) {
            case eOutputChannelUART:
               UART0_Transmit(buf,strlen(buf),1);
               break;
         }
}
