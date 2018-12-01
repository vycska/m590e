#include "main.h"
#include "adc.h"
#include "clocks.h"
#include "config.h"
#include "ds18b20.h"
#include "fifos.h"
#include "handle_command.h"
#include "iap.h"
#include "led.h"
#include "m590e.h"
#include "mrt.h"
#include "systick.h"
#include "output.h"
#include "vswitch.h"
#include "uart.h"
#include "utils.h"
#include "utils-asm.h"
#include "lpc824.h"
#include "pt.h"
#include "timer.h"
#include <string.h>

void DeepSleep_Init(void);

extern char _data_start_lma, _data_start, _data_end, _bss_start, _bss_end;
extern char _flash_start, _flash_end, _ram_start, _ram_end;
extern char _flash_size, _ram_size, _intvecs_size, _text_size, _rodata_size, _data_size, _bss_size, _stack_size, _heap_size;

extern struct pt pt_m590e_smsparse,
                 pt_m590e_smssend,
                 pt_m590e_smsperiodic;
extern struct M590E_Data m590e_data;
extern struct VSwitch_Data vswitch_data;

volatile unsigned int wakeup_cause;
struct Fifo fifo_command_parser, fifo_m590e_responses;

void main(void) {
   char *s, buf[32];
   int t;
   unsigned int cause;

   _disable_irq();

   //power to various analog blocks
   PDRUNCFG = (0<<0 | 0<<1 | 0<<2 | 1<<3 | 0<<4 | 1<<5 | 1<<6 | 1<<7 | 1<<15); //IRC output, IRC, flash, ADC powered; BOD, crystal oscillator, watchdog oscillator, system PLL, analog comparator powered down

   Clocks_Init();

   PRESETCTRL = (1<<2 | 1<<3 | 1<<4 | 1<<7 | 1<<9 | 1<<10 | 1<<11 | 1<<24); //clear USART FRG, USART0, USART1, MRT, WKT, GPIO, flash controller, ADC reset

   t = config(0); //load config

   ADC_Init(); //ADC pin P0.14
   DeepSleep_Init();
   LED_Init(1);
   SysTick_Init(1);
   UART0_Init();
   VSwitch_Init(); //virtual switch on pin P0.12

   DS18B20_Init(); //this initializes one-wire pin P0.9
   M590E_Init();

   Fifo_Init(&fifo_command_parser);
   Fifo_Init(&fifo_m590e_responses);

   PT_INIT(&pt_m590e_smsparse);
   PT_INIT(&pt_m590e_smssend);
   PT_INIT(&pt_m590e_smsperiodic);

   ICPR0 = (3<<0 | 7<<3 | 0xffff<<7 | 0xff<<24); //clear pending interrupts
   _enable_irq();

   Init_Print(t);

   while(1) {
      while((cause=wakeup_cause)!=0) { //atomic interrupt safe read
         if(Fifo_Peek(&fifo_command_parser, &s)) {
            Handle_Command(s);
            Fifo_Remove(&fifo_command_parser);
         }

         if((cause&(1<<eWakeupCauseVSwitchReleased)) != 0) {
            mysprintf(buf, "vswitch duration: %d",vswitch_data.duration);
            output(buf, eOutputSubsystemVSwitch, eOutputLevelDebug);
            _disable_irq();
            wakeup_cause &= (~(1<<eWakeupCauseVSwitchReleased));
            _enable_irq();
         }

         if((cause&(1<<eWakeupCauseRingActive))==0 && (cause&(1<<eWakeupCauseSmsSending))==0 && Fifo_Peek(&fifo_m590e_responses, &s)) {
            output(s, eOutputSubsystemSystem, eOutputLevelNormal);
            Fifo_Remove(&fifo_m590e_responses);
         }

         if((cause&(1<<eWakeupCauseRingActive)) != 0) {
            if(!m590e_data.ring_active) {
               if(PT_SCHEDULE(M590E_SMSParse(&pt_m590e_smsparse)) == 0) {
                  _disable_irq();
                  wakeup_cause &= (~(1<<eWakeupCauseRingActive));
                  _enable_irq();
               }
            }
         }

         if((cause&(1<<eWakeupCauseRingEnded)) != 0) {
            mysprintf(buf, "ring duration: %d", m590e_data.ring_duration);
            output(buf, eOutputSubsystemM590E, eOutputLevelDebug);
            _disable_irq();
            wakeup_cause &= (~(1<<eWakeupCauseRingEnded));
            _enable_irq();
         }

         if((cause&(1<<eWakeupCauseSmsSending)) != 0) {
            if(PT_SCHEDULE(M590E_SMSSend(&pt_m590e_smssend)) == 0) {
               _disable_irq();
               wakeup_cause &= (~(1<<eWakeupCauseSmsSending));
               _enable_irq();
            }
         }
         if((cause&(1<<eWakeupCauseTimer)) != 0) {
            if(PT_SCHEDULE(M590E_SMSPeriodic(&pt_m590e_smsperiodic)) == 0) {
               _disable_irq();
               wakeup_cause &= (~(1<<eWakeupCauseTimer));
               _enable_irq();
            }
         }
      }

      _disable_irq();
      if(wakeup_cause == 0 && !vswitch_data.active) {
         _enable_irq();
         SysTick_Stop();
         LED_Off();
         _wfi();
         LED_On();
         SysTick_Start();
      }
      _enable_irq();
   }
}

void init(void) {
   char *dst, *src;
   //copy data to ram
   for(src = &_data_start_lma, dst = &_data_start; dst < &_data_end; src++, dst++)
      *dst = *src;
   //zero bss
   for(dst = &_bss_start; dst < &_bss_end; dst++)
      *dst = 0;
}

void WKT_Set(int sec) {
   if(sec) //t.b. bent 1 min
      WKT_COUNT= sec*10000;
}

void WKT_Init(void) {
   DPDCTRL = (0<<0 | 1<<1 | 1<<2 | 0<<3 | 0<<4 | 0<<5); //WAKEUP pin hysteresis disabled, WAKEUP pin disabled, low-power oscillator enabled, low-power oscillator in DPD mode disabled, WKTCLKIN pin hysteresis disabled, WKTCLKIN disabled
   WKT_CTRL = (1<<0 | 1<<1 | 1<<2 | 0<<3); //clock source is low power clock (10kHz), clear alarm flag, clear the counter, clock source is internal
   IPR3 = (IPR3 & (~(0x3<<30))) | (1<<30);
   ISER0 = (1<<15);
   WKT_Set(m590e_data.periodic_sms_interval);
}

void DeepSleep_Init(void) {
   PCON = (PCON & (~(0x7<<0))) | (1<<0); //deep-sleep mode
   PDSLEEPCFG |= (1<<3 | 1<<6); //BOD for deep-sleep powered down, watchdog oscillator for deep-sleep powered down
   PDAWAKECFG = (0<<0 | 0<<1 | 0<<2 | 1<<3 | 0<<4 | 1<<5 | 1<<6 | 1<<7 | 0xd<<8 | 0x6<<12 | 1<<15); //power configuration after wake up: IRC oscillator output powered, IRC oscillator power-down powered, flash powered, BOD powered down, ADC powered, crystal oscillator powered down, watchdog oscillator powered down, system PLL powered down, ACMP powered down
   STARTERP0 = (1<<0 | 1<<1); //GPIO pint interrupt 0 and 1 wake-up enabled
   STARTERP1 = (1<<15); //self-wake-up timer interrupt wake-up
   SCR = (SCR&(~(0x1<<1 | 0x1<<2))) | (0<<1 | 1<<2); //do not sleep when returning to thread mode, deep sleep is processor's low power mode
   WKT_Init();
}

void System_Reset(void) {
   _dsb();
   AIRCR = (AIRCR & (~(1<<1 | 1<<2 | 0xffffu<<16))) | (1<<2 | 0x5fau<<16);
   _dsb();
   while(1);
}

void Init_Print(int config_load_result) {
   char buf[64];
   unsigned char data[8];
   int i, j, l;
   float used_value;
   for(i=0; i<=14; i++) {
      switch(i) {
         case 0:
            mysprintf(buf,"build time: %s %s",__DATE__,__TIME__);
            break;
         case 1:
            mysprintf(buf,"VERSION: %d",VERSION);
            break;
         case 2:
            l = mysprintf(buf, "%s", "config load: ");
            mysprintf(buf+l, "%s", config_load_result==0?"ok":(config_load_result==1?"error":"unknown"));
            break;
         case 3:
            mysprintf(buf, "_flash_size: %u [0x%x - 0x%x]", (unsigned int)&_flash_size, (unsigned int)&_flash_start, (unsigned int)&_flash_end);
            break;
         case 4:
            mysprintf(buf, "_ram_size: %u [0x%x - 0x%x]", (unsigned int)&_ram_size, (unsigned int)&_ram_start, (unsigned int)&_ram_end);
            break;
         case 5:
            mysprintf(buf, "_intvecs_size: %u", (unsigned int)&_intvecs_size);
            break;
         case 6:
            mysprintf(buf, "_text_size: %u", (unsigned int)&_text_size);
            break;
         case 7:
            mysprintf(buf, "_rodata_size: %u", (unsigned int)&_rodata_size);
            break;
         case 8:
            mysprintf(buf, "_data_size: %u", (unsigned int)&_data_size);
            break;
         case 9:
            mysprintf(buf, "_bss_size: %u", (unsigned int)&_bss_size);
            break;
         case 10:
            mysprintf(buf, "_stack_size: %u", (unsigned int)&_stack_size);
            break;
         case 11:
            mysprintf(buf, "_heap_size: %u", (unsigned int)&_heap_size);
            break;
         case 12:
            l = (int)&_intvecs_size + (int)&_text_size + (int)&_rodata_size + (int)&_data_size;
            used_value = 100.0f * (float)l / (int)&_flash_size;
            mysprintf(buf, "flash used: %d [%f1%%]", l, (char*)&used_value);
            break;
         case 13:
            l = (int)&_stack_size + (int)&_data_size + (int)&_bss_size; //heap'o nepridedu, nes jam priskiriu siaip vietos tiek kiek lieka
            used_value = 100.0f * (float)l / (int)&_ram_size;
            mysprintf(buf, "ram used: %d [%f1%%]", l, (char*)&used_value);
            break;
         case 14: //DS18B20
            l = mysprintf(buf, "one-wire device: ");
            if(DS18B20_ReadROM(data) == DS18B20_OK) {
               for(j=0; j<8; j++)
                  l += mysprintf(&buf[l], "0x%x%s", (unsigned int)data[j], j == 7 ? " " : "-");
            }
            else mysprintf(buf+l, "none");
            break;
      }
      output(buf, eOutputSubsystemSystem, eOutputLevelImportant);
   }
}

void WKT_IRQHandler(void) {
   WKT_CTRL |= (1<<1);
   if(m590e_data.periodic_sms_interval >= 60) //t.b. bent 1 min
      WKT_COUNT= m590e_data.periodic_sms_interval*10000;
   wakeup_cause |= (1<<eWakeupCauseTimer);
}
