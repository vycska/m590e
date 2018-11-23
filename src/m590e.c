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
struct Ring_Data ring_data;

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
   if(ring_data.active == 0) {
      ring_data.active = 1;
      ring_data.delay = 0;
      ring_data.duration = 0;
   }
}

PT_THREAD(M590E_Send(struct pt *pt, char *msg, int len, int k, char **lines, int delay)) {
   char *s;
   static int i;
   static struct timer timer_m590e_recieve;

   PT_BEGIN(pt);
   UART1_Transmit(msg, len);
   for(i=0; i<k; i++) {
      timer_set(&timer_m590e_recieve, delay);
      PT_WAIT_UNTIL(pt, ((s=0, Fifo_Get(&fifo_m590e_responses, &s)==1) || timer_expired(&timer_m590e_recieve)));
      lines[i] = (s!=0 ? s : 0);
   }
   PT_END(pt);
}

PT_THREAD(M590E_SMSInit(struct pt *pt, int *ok)) {
   static char buf[32], *lines[3];
   static int l;

   PT_BEGIN(pt);

   *ok = 1;

   l = mysprintf(buf, "ATE0\r");
   PT_SPAWN(pt, &pt_m590e_send, M590E_Send(&pt_m590e_send, buf, l, 2, lines, 2000));
   if((strcmp(lines[0],"ATE0")==0 && strcmp(lines[1],"OK")==0) || strcmp(lines[0],"OK")==0) {
      strcpy(buf+l-1, " ok");
      output(buf, eOutputSubsystemM590E, eOutputLevelDebug);
   }
   else {
      *ok = 0;
      strcpy(buf+l-1, " error");
      output(buf, eOutputSubsystemM590E, eOutputLevelDebug);
      PT_EXIT(pt);
   }

   l = mysprintf(buf, "AT+CMGF=1\r");
   PT_SPAWN(pt, &pt_m590e_send, M590E_Send(&pt_m590e_send, buf, l, 1, lines, 2000));
   if(strcmp(lines[0],"OK")==0) {
      strcpy(buf+l-1, " ok");
      output(buf, eOutputSubsystemM590E, eOutputLevelDebug);
   }
   else {
      *ok = 0;
      strcpy(buf+l-1, " error");
      output(buf, eOutputSubsystemM590E, eOutputLevelDebug);
      PT_EXIT(pt);
   }

   l = mysprintf(buf, "AT+CSCS=\"GSM\"\r");
   PT_SPAWN(pt, &pt_m590e_send, M590E_Send(&pt_m590e_send, buf, l, 1, lines, 2000));
   if(strcmp(lines[0],"OK")==0) {
      strcpy(buf+l-1, " ok");
      output(buf, eOutputSubsystemM590E, eOutputLevelDebug);
   }
   else {
      *ok = 0;
      strcpy(buf+l-1, " error");
      output(buf, eOutputSubsystemM590E, eOutputLevelDebug);
      PT_EXIT(pt);
   }

   PT_END(pt);
}

PT_THREAD(M590E_SMSParse(struct pt *pt)) {
   char *s;
   static char buf[32], *lines[3];
   static int i, l, init_ok, error;

   PT_BEGIN(pt);

   PT_WAIT_UNTIL(pt, m590e_data.mutex==1);

   m590e_data.mutex = 0;

   Fifo_Clear(&fifo_m590e_responses);

   PT_SPAWN(pt, &pt_m590e_smsinit, M590E_SMSInit(&pt_m590e_smsinit, &init_ok));

   if(!init_ok) {
      m590e_data.mutex = 1;
      PT_EXIT(pt);
   }

   for(error=0, i=1; !error; i++) {
      l = mysprintf(buf,"AT+CMGR=%d\r",i);
      PT_SPAWN(pt, &pt_m590e_send, M590E_Send(&pt_m590e_send, buf, l, 3, lines, 2000));
      if(strcmp(lines[2], "OK")==0) {
         if((s=strstr(lines[0], "REC UNREAD"))!=0 && (s=strchr(s, '+'))!=0) {
            strncpy(m590e_data.source_number, s, 12);
            m590e_data.source_number[12] = 0;
            Fifo_Put(&fifo_command_parser, lines[1]);
            PT_YIELD(pt); //susispenduojam, kad ivyktu komandos apdorojimas
            m590e_data.source_number[0] = 0;
         }
      }
      else {
         error = 1;
         strcpy(buf+l-1, " error");
         output(buf, eOutputSubsystemM590E, eOutputLevelDebug);
      }
   }

   l = mysprintf(buf, "AT+CMGD=1,1\r");
   PT_SPAWN(pt, &pt_m590e_send, M590E_Send(&pt_m590e_send, buf, l, 1, lines, 2000));
   if(strcmp(lines[0],"OK") == 0) {
      strcpy(buf+l-1, " ok");
      output(buf, eOutputSubsystemM590E, eOutputLevelDebug);
   }
   else {
      strcpy(buf+l-1, " error");
      output(buf, eOutputSubsystemM590E, eOutputLevelDebug);
   }

   m590e_data.mutex = 1;

   PT_END(pt);
}

PT_THREAD(M590E_SMSSend(struct pt *pt)) {
   struct Fifo_SMS_Item sms;
   static char buf[32], msg[MAX_SMS_SIZE], src[MAX_SRC_SIZE], *lines[2];
   static int k ,l, init_ok;
   static struct timer timer_fifo_get;
   int i;

   PT_BEGIN(pt);

   PT_WAIT_UNTIL(pt, m590e_data.mutex==1);

   m590e_data.mutex = 0;

   Fifo_Clear(&fifo_m590e_responses);

   k = 0;

   while(Fifo_SMS_Peek(&sms) && k<MAX_SMS_SIZE-1 && (k==0?(strcpy(src,sms.src),1) : (strcmp(src,sms.src)==0))) { //-1 nes sms'o gale dar pridesim \x1a
      l = MIN2(MAX_SMS_SIZE-1-k,sms.len);
      strncpy(msg+k, sms.msg, l);
      k += l;
      if(l == sms.len) { //tilpo pilnas sms'as
         if(k<MAX_SMS_SIZE-1) msg[k++] = '\n';
         Fifo_SMS_Remove();
         timer_set(&timer_fifo_get, 1000);
         PT_WAIT_UNTIL(pt, Fifo_SMS_Peek(&sms)==1 || timer_expired(&timer_fifo_get));
      }
      else {
         for(i=l; i<sms.len; i++)
            sms.msg[i-l] = sms.msg[i];
         sms.len -= l;
         Fifo_SMS_Change(sms.msg, sms.len, sms.src);
      }
   }

   msg[k++] = '\x1a';
   for(i=0; i<k; i++)
      if(msg[i]=='\r') msg[i]=' ';

   PT_SPAWN(pt, &pt_m590e_smsinit, M590E_SMSInit(&pt_m590e_smsinit, &init_ok));

   if(!init_ok) {
      m590e_data.mutex = 1;
      PT_EXIT(pt);
   }

   l = mysprintf(buf, "AT+CMGS=\"%s\"\r", src);
   PT_SPAWN(pt, &pt_m590e_send, M590E_Send(&pt_m590e_send, buf, l, 1, lines, 2000));
   if(strcmp(lines[0],">") == 0) {
      strcpy(buf+l-1, " ok");
      output(buf, eOutputSubsystemM590E, eOutputLevelDebug);
   }
   else {
      strcpy(buf+l-1, " error");
      output(buf, eOutputSubsystemM590E, eOutputLevelDebug);
      m590e_data.mutex = 1;
      PT_EXIT(pt);
   }

   PT_SPAWN(pt, &pt_m590e_send, M590E_Send(&pt_m590e_send, msg, k, 2, lines, 5000));
   if(strcmp(lines[1],"OK") == 0) {
      output("SMS send ok", eOutputSubsystemM590E, eOutputLevelDebug);
   }
   else {
      output("SMS send error", eOutputSubsystemM590E, eOutputLevelDebug);
   }

   if(Fifo_SMS_Peek(&sms)) {
      m590e_data.mutex = 1;
      PT_RESTART(pt);
   }

   m590e_data.mutex = 1;

   PT_END(pt);
}
