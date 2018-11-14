#ifndef __OUTPUT_H__
#define __OUTPUT_H__

enum eOutputChannel {
   eOutputChannelUART         = 0,
   eOutputChannelLast         = 1
};

enum eOutputSubsystem {
   eOutputSubsystemADC        = 0,
   eOutputSubsystemDS18B20    = 1,
   eOutputSubsystemSystem     = 2,
   eOutputSubsystemVSwitch    = 3,
   eOutputSubsystemLast       = 4
};

enum eOutputLevel {
   eOutputLevelDebug          = 0,
   eOutputLevelNormal         = 1,
   eOutputLevelImportant      = 2
};

struct Output_Data {
   unsigned char channel_mask;
   unsigned char subsystem_mask[eOutputSubsystemLast];
};

void output(char*, enum eOutputSubsystem, enum eOutputLevel);

#endif
