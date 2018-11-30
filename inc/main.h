#ifndef __MAIN_H__
#define __MAIN_H__

#define VERSION 1
#define WAKEUP_SEC 600

enum Wakeup_Cause {
   eWakeupCauseVSwitchOn         = 0,
   eWakeupCauseVSwitchReleased   = 1,
   eWakeupCauseRingActive        = 2,
   eWakeupCauseRingEnded         = 3,
   eWakeupCauseSmsSending        = 4,
   eWakeupCauseTimer             = 5
};

void System_Reset(void);
void Init_Print(int);

#endif
