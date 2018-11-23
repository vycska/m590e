#ifndef __OUTPUT_H__
#define __OUTPUT_H__

enum eOutputChannel {
   eOutputChannelUART         = 0,
   eOutputChannelSMS          = 1,
   eOutputChannelLast         = 2
};

enum eOutputSubsystem {
   eOutputSubsystemADC        = 0,
   eOutputSubsystemDS18B20    = 1,
   eOutputSubsystemM590E      = 2,
   eOutputSubsystemSystem     = 3,
   eOutputSubsystemVSwitch    = 4,
   eOutputSubsystemLast       = 5
};

enum eOutputLevel { //kuris bitas kaukeje; reiksme 1 tame bite reiskia uzblokavima
   eOutputLevelDebug          = 0,
   eOutputLevelNormal         = 1,
   eOutputLevelImportant      = 2
};

struct Output_Data {
   unsigned char mask[eOutputChannelLast][eOutputSubsystemLast];
};

void output(char*, enum eOutputSubsystem, enum eOutputLevel);

#endif
