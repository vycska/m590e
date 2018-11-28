#include "m590e.h"
#include "fifos.h"
#include "main.h"
#include "mrt.h"
#include "output.h"
#include "uart.h"
#include "utils.h"
#include "lpc824.h"
#include "pt.h"
#include "timer.h"
#include <string.h>

extern volatile unsigned int wakeup_cause;
extern struct Fifo fifo_command_parser;
extern struct Fifo fifo_m590e_responses;

struct pt pt_m590e_smsparse,
          pt_m590e_smssend,
          pt_m590e_smsinit,
          pt_m590e_send;
struct timer timer_m590e_ring,
             timer_m590e_sms;
struct M590E_Data m590e_data;

void M590E_Init(void) {
   //Ring on PIO0_17
   PINENABLE0 |= (1<<22); //ADC_9 disabled on PIO0_17
   PIO0_17 = (1<<3 | 0<<5 | 0<<6 | 0<<10 | 0<<11 | 0<<13); //pd resistor, no hysteresis, input not inverted, no open-drain mode, input filter bypassed, clock IOCONCLKDIV0
   DIR0 &= (~(1<<17)); //direction is input
   PINTSEL1 = 17; //select PIO0_17 for pin interrupt
   ISEL &= (~(1<<1)); //edge sensitive interrupt mode for PINTSEL1
   IST = (1<<1); //clear detected edges
   CIENR = (1<<1); //disable rising edge interrupt
   SIENF = (1<<1); //enable falling edge interrupt
   IPR6 = (IPR6 & (~(3<<14))) | (1<<14); //interrupt priority = 1
   ISER0 = (1<<25); //enable PININT1 interrupt

   //TXD on PIO0_2, RXD on PIO0_3 (UART1)
   UART1_Init();

   m590e_data.mutex = 1;
}

int Ring_Active(void) {
   return ((PIN0>>17)&1) == 0;
}

void PININT1_IRQHandler(void) {
   CIENF = (1<<1); //disable falling edge interrupt
   FALL = (1<<1); //clear detected falling edge
   wakeup_cause |= (1<<eWakeupCauseRingActive);
   if(m590e_data.ring_active == 0) {
      m590e_data.ring_active = 1;
      m590e_data.ring_delay = 0;
      m590e_data.ring_duration = 0;
   }
}

void M590E_Send_Blocking(char *msg, int len, int k, int delay) {
   char *s;
   int i;
   struct timer timer;
   UART1_Transmit(msg, len);
   for(i=0; i<k; i++) {
      timer_set(&timer, delay);
      while(!(Fifo_Peek(&fifo_m590e_responses, &s)==1 || timer_expired(&timer)));
      if(s != 0) {
         strcpy(m590e_data.response[i], s);
         Fifo_Remove(&fifo_m590e_responses);
      }
      else {
         strcpy(m590e_data.response[i],"\0");
         break;
      }
   }
}

PT_THREAD(M590E_Send(struct pt *pt, char *msg, int len, int k, int delay)) {
   char *s;
   static int i;
   static struct timer timer;

   PT_BEGIN(pt);
   UART1_Transmit(msg, len);
   for(i=0; i<k; i++) {
      timer_set(&timer, delay);
      PT_WAIT_UNTIL(pt, Fifo_Peek(&fifo_m590e_responses, &s)==1 || timer_expired(&timer));
      if(s!=0) {
         strcpy(m590e_data.response[i], s);
         Fifo_Remove(&fifo_m590e_responses);
      }
      else strcpy(m590e_data.response[i],"\0");
   }
   PT_END(pt);
}

PT_THREAD(M590E_SMSInit(struct pt *pt, int *result)) {
   static char buf[32];
   static int l;

   PT_BEGIN(pt);

   *result = 1;

   l = mysprintf(buf, "ATE0\r");
   PT_SPAWN(pt, &pt_m590e_send, M590E_Send(&pt_m590e_send, buf, l, 2, 2000));
   if((strcmp(m590e_data.response[0],"ATE0")==0 && strcmp(m590e_data.response[1],"OK")==0) || strcmp(m590e_data.response[0],"OK")==0) {
      strcpy(buf+l-1, " ok");
      output(buf, eOutputSubsystemM590E, eOutputLevelDebug);
   }
   else {
      *result = 0;
      strcpy(buf+l-1, " error");
      output(buf, eOutputSubsystemM590E, eOutputLevelDebug);
      PT_EXIT(pt);
   }

   l = mysprintf(buf, "AT+CMGF=1\r");
   PT_SPAWN(pt, &pt_m590e_send, M590E_Send(&pt_m590e_send, buf, l, 1, 2000));
   if(strcmp(m590e_data.response[0],"OK")==0) {
      strcpy(buf+l-1, " ok");
      output(buf, eOutputSubsystemM590E, eOutputLevelDebug);
   }
   else {
      *result = 0;
      strcpy(buf+l-1, " error");
      output(buf, eOutputSubsystemM590E, eOutputLevelDebug);
      PT_EXIT(pt);
   }

   l = mysprintf(buf, "AT+CSCS=\"8859-1\"\r");
   PT_SPAWN(pt, &pt_m590e_send, M590E_Send(&pt_m590e_send, buf, l, 1, 2000));
   if(strcmp(m590e_data.response[0],"OK")==0) {
      strcpy(buf+l-1, " ok");
      output(buf, eOutputSubsystemM590E, eOutputLevelDebug);
   }
   else {
      *result = 0;
      strcpy(buf+l-1, " error");
      output(buf, eOutputSubsystemM590E, eOutputLevelDebug);
      PT_EXIT(pt);
   }

   PT_END(pt);
}

PT_THREAD(M590E_SMSParse(struct pt *pt)) {
   char *s;
   static char buf[32];
   static int i, l, status;

   PT_BEGIN(pt);

   PT_WAIT_UNTIL(pt, m590e_data.mutex==1);

   m590e_data.mutex = 0;

   Fifo_Clear(&fifo_m590e_responses);

   PT_SPAWN(pt, &pt_m590e_smsinit, M590E_SMSInit(&pt_m590e_smsinit, &status));

   if(status == 1) {
      for(i=1; status==1; i++) {
         l = mysprintf(buf,"AT+CMGR=%d\r",i);
         PT_SPAWN(pt, &pt_m590e_send, M590E_Send(&pt_m590e_send, buf, l, 3, 2000));
         if(strcmp(m590e_data.response[2], "OK")==0) {
            if((s=strstr(m590e_data.response[0], "REC UNREAD"))!=0 && (s=strchr(s, '+'))!=0) {
               strncpy(m590e_data.source_number, s, MAX_SRC_SIZE-1);
               m590e_data.source_number[MAX_SRC_SIZE-1] = 0;
               Fifo_Put(&fifo_command_parser, m590e_data.response[1]);
               PT_YIELD(pt); //susispenduojam, kad ivyktu komandos apdorojimas
            }
         }
         else {
            status = 0;
            strcpy(buf+l-1, " error");
            output(buf, eOutputSubsystemM590E, eOutputLevelDebug);
         }
      }

      l = mysprintf(buf, "AT+CMGD=1,1\r");
      PT_SPAWN(pt, &pt_m590e_send, M590E_Send(&pt_m590e_send, buf, l, 1, 2000));
      if(strcmp(m590e_data.response[0],"OK")==0) {
         strcpy(buf+l-1, " ok");
         output(buf, eOutputSubsystemM590E, eOutputLevelDebug);
      }
      else {
         strcpy(buf+l-1, " error");
         output(buf, eOutputSubsystemM590E, eOutputLevelDebug);
      }
   }

   m590e_data.mutex = 1;

   PT_END(pt);
}

PT_THREAD(M590E_SMSSend(struct pt *pt)) {
   static char buf[32], sending_sms[MAX_SMS_SIZE];
   static int l, status, sending_sms_len;
   static struct timer timer;
   char *interim_sms;
   int i, *interim_sms_len;

   PT_BEGIN(pt);

   PT_WAIT_UNTIL(pt, m590e_data.mutex==1);

   m590e_data.mutex = 0;

   Fifo_Clear(&fifo_m590e_responses);

   PT_SPAWN(pt, &pt_m590e_smsinit, M590E_SMSInit(&pt_m590e_smsinit, &status));

   while(status) {
      switch(status) {
         case 1: //suformuojama sms'a
            for(sending_sms_len=0; Fifo_SMS_Count()>0 && sending_sms_len<MAX_SMS_SIZE-1; ) { //-1 nes sms'o gale dar pridesim \x1a
               Fifo_SMS_Peek(&interim_sms, &interim_sms_len);
               l = MIN2(MAX_SMS_SIZE-1-sending_sms_len, *interim_sms_len);
               strncpy(sending_sms+sending_sms_len, interim_sms, l);
               sending_sms_len += l;
               if(l == *interim_sms_len) { //tilpo pilnas sms'as
                  if(sending_sms_len < MAX_SMS_SIZE-1) sending_sms[sending_sms_len++] = '\n';
                  Fifo_SMS_Remove();
                  timer_set(&timer, 1000);
                  PT_WAIT_UNTIL(pt, Fifo_SMS_Count()>0 || timer_expired(&timer));
               }
               else { //pakoreguojame fifo esancia zinute [nes dali jos jau idejome i siunciama sms]
                  for(i=l; i<*interim_sms_len; i++)
                     interim_sms[i-l] = interim_sms[i];
                  *interim_sms_len -= l;
               }
            }
            for(i=0; i<sending_sms_len; i++)
               if(sending_sms[i]=='\r') sending_sms[i]=' ';
            sending_sms[sending_sms_len++] = '\x1a';
            status = 2;
            break;
         case 2: //issiunciam numeri
            l = mysprintf(buf, "AT+CMGS=\"%s\"\r", strlen(m590e_data.source_number)==MAX_SRC_SIZE-1 ? m590e_data.source_number : "+37061525451");
            PT_SPAWN(pt, &pt_m590e_send, M590E_Send(&pt_m590e_send, buf, l, 1, 2000));
            if(strcmp(m590e_data.response[0],">") == 0) {
               status = 3;
               strcpy(buf+l-1, " ok");
               output(buf, eOutputSubsystemM590E, eOutputLevelDebug);
            }
            else {
               status = 0;
               strcpy(buf+l-1, " error");
               output(buf, eOutputSubsystemM590E, eOutputLevelDebug);
            }
            break;
         case 3: //issiunciam zinute
            PT_SPAWN(pt, &pt_m590e_send, M590E_Send(&pt_m590e_send, sending_sms, sending_sms_len, 2, 5000));
            if(strcmp(m590e_data.response[1],"OK") == 0) {
               status = Fifo_SMS_Count()>0;
               output(m590e_data.response[0], eOutputSubsystemM590E, eOutputLevelDebug);
               output(m590e_data.response[1], eOutputSubsystemM590E, eOutputLevelDebug);
            }
            else {
               status = 0;
            }
            break;
      }
   }

   strcpy(m590e_data.source_number, "\0");
   m590e_data.mutex = 1;

   PT_END(pt);
}
