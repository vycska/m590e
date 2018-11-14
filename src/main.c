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

void DeepSleep_Init(void);

extern char _data_start_lma, _data_start, _data_end, _bss_start, _bss_end;
extern char _flash_start, _flash_end, _ram_start, _ram_end;
extern char _flash_size, _ram_size, _intvecs_size, _text_size, _rodata_size, _data_size, _bss_size, _stack_size, _heap_size;

extern struct pt pt_m590e_ring;
extern struct Output_Data output_data;
extern struct Ring_Data ring_data;
extern struct VSwitch_Data vswitch_data;

volatile unsigned int gInterruptCause;
volatile unsigned int millis;
struct Fifo fifo_command_parser, fifo_m590e_responses;

void main(void) {
   char *s, buf[64];
   unsigned char data[8];
   int i, l, config_load_result;
   unsigned int cause;
   float used_value;

   _disable_irq();

   //power to various analog blocks
   PDRUNCFG = (0<<0 | 0<<1 | 0<<2 | 1<<3 | 0<<4 | 1<<5 | 1<<6 | 1<<7 | 1<<15); //IRC output, IRC, flash, ADC powered; BOD, crystal oscillator, watchdog oscillator, system PLL, analog comparator powered down

   Clocks_Init();

   PRESETCTRL = (1<<2 | 1<<3 | 1<<4 | 1<<7 | 1<<10 | 1<<11 | 1<<24); //clear USART FRG, USART0, USART1, MRT, GPIO, flash controller, ADC reset

   ADC_Init(); //ADC pin P0.14
   LED_Init(1);
   SysTick_Init();
   SysTick_Start();
   UART0_Init();
   VSwitch_Init(); //virtual switch on pin P0.12

   DeepSleep_Init();

   DS18B20_Init(); //this initializes one-wire pin P0.9
   M590E_Init();

   ICPR0 = (3<<0 | 7<<3 | 0xffff<<7 | 0xff<<24); //clear pending interrupts
   _enable_irq();

   config_load_result = config_load();

   Fifo_Init(&fifo_command_parser);
   Fifo_Init(&fifo_m590e_responses);

   PT_INIT(&pt_m590e_ring);

   for(i=0; i<=13; i++) {
      switch(i) {
         case 0:
            mysprintf(buf,"build time: %s %s",__DATE__,__TIME__);
            break;
         case 1:
            mysprintf(buf,"VERSION: %d",VERSION);
            break;
         case 2:
            mysprintf(buf,"%s",config_load_result?"config load error":"config load ok");
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
      }
      output(buf, eOutputSubsystemSystem, eOutputLevelImportant);
   }

   //DS18B20
   if(DS18B20_ReadROM(data) == DS18B20_OK) {
      l = mysprintf(buf, "one-wire device: ");
      for(i = 0; i < 8; i++)
         l += mysprintf(&buf[l], "0x%x%s", (unsigned int)data[i], i == 7 ? " " : "-");
      output(buf, eOutputSubsystemDS18B20, eOutputLevelNormal);
   }

   while(1) {
      while((cause=gInterruptCause)!=0) { //atomic interrupt safe read
         //if(cause & (1<<0)) { //button pressed and no deep-sleep mode active
            if(Fifo_Get(&fifo_command_parser, &s))
               Handle_Command(s);
            if((cause&(1<<1))==0 && Fifo_Get(&fifo_m590e_responses, &s))
               output(s, eOutputSubsystemSystem, eOutputLevelImportant);
         //}
         if(cause & (1<<1)) { //Ring
            if(!ring_data.active) {
               if(PT_SCHEDULE(Handle_M590E_Ring(&pt_m590e_ring)) == 0) {
                  output("ring processing ended", eOutputSubsystemSystem, eOutputLevelImportant);
                  _disable_irq();
                  gInterruptCause &= (~(1<<1));
                  _enable_irq();
               }
            }
         }
         if(cause & (1<<3)) { //button released
            mysprintf(buf, "switch %d",vswitch_data.duration);
            output(buf, eOutputSubsystemVSwitch, eOutputLevelDebug);
            _disable_irq();
            gInterruptCause &= (~(1<<3));
            _enable_irq();
         }
      }
      _disable_irq();
      if(gInterruptCause == 0 && !vswitch_data.active) {
         _enable_irq();
         SysTick_Stop();
         LED_Off();
         _wfi();
         LED_On();
         SysTick_Start();
         output("awake from dreaming", eOutputSubsystemSystem, eOutputLevelImportant);
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

void DeepSleep_Init(void) {
   PCON = (PCON & (~(0x7<<0))) | (1<<0); //deep-sleep mode
   PDSLEEPCFG |= (1<<3 | 1<<6); //BOD for deep-sleep powered down, watchdog oscillator for deep-sleep powered down
   PDAWAKECFG = (0<<0 | 0<<1 | 0<<2 | 1<<3 | 0<<4 | 1<<5 | 1<<6 | 1<<7 | 0xd<<8 | 0x6<<12 | 1<<15); //power configuration after wake up: IRC oscillator output powered, IRC oscillator power-down powered, flash powered, BOD powered down, ADC powered, crystal oscillator powered down, watchdog oscillator powered down, system PLL powered down, ACMP powered down
   STARTERP0 = (1<<0 | 1<<1); //GPIO pint interrupt 0 and 1 wake-up enabled
   STARTERP1 = 0; //all other interrupts for wake-up disabled
   SCR = (SCR&(~(0x1<<1 | 0x1<<2))) | (0<<1 | 1<<2); //do not sleep when returning to thread mode, deep sleep is processor's low power mode
}

void SystemReset(void) {
   _dsb();
   AIRCR = (AIRCR & (~(1<<1 | 1<<2 | 0xffffu<<16))) | (1<<2 | 0x5fau<<16);
   _dsb();
   while(1);
}
