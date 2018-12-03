#ifndef __MAIN_H__
#define __MAIN_H__

#include "config.h"

#define VERSION 1

struct Main_Data {
   enum Config_Result config_load_result;
   volatile unsigned int wakeup_cause;
};

enum Wakeup_Cause {
   eWakeupCauseVSwitchOn         = 0,
   eWakeupCauseVSwitchReleased   = 1,
   eWakeupCauseRingActive        = 2,
   eWakeupCauseRingEnded         = 3,
   eWakeupCauseSmsSending        = 4,
   eWakeupCauseTimer             = 5
};

void System_Reset(void);
void Init_Print(void);
void WKT_Set(int);

#endif
