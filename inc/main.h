#ifndef __MAIN_H__
#define __MAIN_H__

#define VERSION 1

enum Wakeup_Cause {
   eWakeupCauseVSwitchOn         = 0,
   eWakeupCauseVSwitchReleased   = 1,
   eWakeupCauseRingActive        = 2,
   eWakeupCauseRingEnded         = 3,
   eWakeupCauseSmsSending        = 4
};

void SystemReset(void);

#endif
