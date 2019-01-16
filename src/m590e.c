#include "m590e.h"
#include "boozer.h"
#include "fifos.h"
#include "main.h"
#include "mrt.h"
#include "output.h"
#include "uart.h"
#include "utils.h"
#include "lpc824.h"
#include "pt.h"
#include "timer.h"
#include <stdlib.h>
#include <string.h>

extern struct Fifo fifo_command_parser;
extern struct Fifo fifo_m590e_responses;
extern struct Boozer_Data boozer_data;
extern struct Main_Data main_data;

struct pt pt_m590e_smsparse,
          pt_m590e_smssend,
          pt_m590e_smsperiodic,
          pt_m590e_smspir,
          pt_m590e_smsinit,
          pt_m590e_send;
struct M590E_Data m590e_data;

void M590E_Init(void) {
   //DTR (for sleep) on PIO0_17
   PINENABLE0 |= (1<<22); //ADC_9 disabled on PIO0_17
   PIO0_17 = (0<<3 | 0<<5 | 0<<6 | 0<<10 | 0<<11 | 0<<13); //no pu/pd resistors, hysteresis disabled, input not inverted, no open-drain mode, bypass input filter, clock for input filter IOCONCLKDIV0
   DIR0 |= (1<<17); //direction is output
   SET0 = (1<<17); //set output high [DTR will also be high]

   //Ring on PIO0_13
   PINENABLE0 |= (1<<23); //ADC_10 disabled on PIO0_13
   PIO0_13 = (2<<3 | 0<<5 | 0<<6 | 0<<10 | 0<<11 | 0<<13); // pu resistor, no hysteresis, input not inverted, no open-drain mode, input filter bypassed, clock IOCONCLKDIV0
   DIR0 &= (~(1<<13)); //direction is input
   PINTSEL1 = 13; //select PIO0_13 for pin interrupt
   ISEL &= (~(1<<1)); //edge sensitive interrupt mode for PINTSEL1
   IST = (1<<1); //clear detected edges
   CIENR = (1<<1); //disable rising edge interrupt
   SIENF = (1<<1); //enable falling edge interrupt
   IPR6 = (IPR6 & (~(3<<14))) | (1<<14); //interrupt priority = 1
   ISER0 = (1<<25); //enable PININT1 interrupt

   //TXD on PIO0_2, RXD on PIO0_3 (UART1)
   UART1_Init();

   m590e_data.mutex = 1;
   main_data.wakeup_cause |= (1<<eWakeupCauseM590EInit);
}

int Ring_Active(void) {
   return ((PIN0>>13)&1) == 0;
}
void M590E_Sleep_Enter(void) {
   M590E_Send_Blocking("AT+ENPWRSAVE=1\r", 15, -1, 5000);
   CLR0 = (1<<17);
}

void M590E_Sleep_Exit(void) {
   SET0 = (1<<17);
   M590E_Send_Blocking("AT+ENPWRSAVE=0\r", 15, -1, 5000);
}

void PININT1_IRQHandler(void) {
   CIENF = (1<<1); //disable falling edge interrupt
   FALL = (1<<1); //clear detected falling edge
   main_data.wakeup_cause |= (1<<eWakeupCauseRingActive);
   if(m590e_data.ring_active == 0) {
      if(!boozer_data.active) Boozer_On(200);
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
   output(msg, eOutputSubsystemSystem, k>0?eOutputLevelImportant:eOutputLevelDebug);
   for(i=0; i<ABS(k) && i<MAX_RESPONSES; i++) {
      timer_set(&timer, delay);
      while(!(Fifo_Peek(&fifo_m590e_responses, &s)==1 || timer_expired(&timer)));
      if(s != 0) {
         strncpy(m590e_data.response[i], s, MAX_RESPONSE_SIZE-1);
         m590e_data.response[i][MAX_RESPONSE_SIZE-1] = '\0';
         Fifo_Remove(&fifo_m590e_responses);
         output(m590e_data.response[i], eOutputSubsystemSystem, k>0?eOutputLevelImportant:eOutputLevelDebug);
      }
      else {
         while(i<ABS(k) && i<MAX_RESPONSES) strcpy(m590e_data.response[i++],"\0");
      }
   }
}

PT_THREAD(M590E_Send(struct pt *pt, char *msg, int len, int k, int delay)) {
   char *s;
   static int i;
   static struct timer timer;

   PT_BEGIN(pt);
   UART1_Transmit(msg, len);
   for(i=0; i<k && i<MAX_RESPONSES; i++) {
      timer_set(&timer, delay);
      PT_WAIT_UNTIL(pt, Fifo_Peek(&fifo_m590e_responses, &s)==1 || timer_expired(&timer));
      if(s!=0) {
         strncpy(m590e_data.response[i], s, MAX_RESPONSE_SIZE-1);
         m590e_data.response[i][MAX_RESPONSE_SIZE-1] = '\0';
         Fifo_Remove(&fifo_m590e_responses);
      }
      else {
         while(i<k && i<MAX_RESPONSES) strcpy(m590e_data.response[i++],"\0");
      }
   }
   PT_END(pt);
}

PT_THREAD(M590E_SMSInit(struct pt *pt)) {
   static char buf[32];
   static int l, status, tries;
   static struct timer timer;

   PT_BEGIN(pt);

   PT_WAIT_UNTIL(pt, m590e_data.mutex==1);

   m590e_data.mutex = 0;

   Fifo_Clear(&fifo_m590e_responses);

   status = 2;
   while(status) {
      switch(status) {
         case 1: //palaukimas
            timer_set(&timer, 5000);
            PT_WAIT_UNTIL(pt, timer_expired(&timer));
            tries += 1;
            if(tries >= MAX_INIT_TRIES) {
               status = 0;
               tries = 0;
            }
            else {
               status = 2;
            }
            break;
         case 2:
            l = mysprintf(buf, "ATE0\r");
            PT_SPAWN(pt, &pt_m590e_send, M590E_Send(&pt_m590e_send, buf, l, 2, 2000));
            if((strcmp(m590e_data.response[0],"ATE0")==0 && strcmp(m590e_data.response[1],"OK")==0) || strcmp(m590e_data.response[0],"OK")==0) {
               status = 3;
               strcpy(buf+l-1, " ok");
               output(buf, eOutputSubsystemM590E, eOutputLevelDebug);
            }
            else {
               status = 1;
               strcpy(buf+l-1, " error");
               output(buf, eOutputSubsystemM590E, eOutputLevelDebug);
            }
            break;
         case 3:
            l = mysprintf(buf, "AT+CMGF=1\r");
            PT_SPAWN(pt, &pt_m590e_send, M590E_Send(&pt_m590e_send, buf, l, 1, 2000));
            if(strcmp(m590e_data.response[0],"OK")==0) {
               status = 4;
               strcpy(buf+l-1, " ok");
               output(buf, eOutputSubsystemM590E, eOutputLevelDebug);
            }
            else {
               status = 1;
               strcpy(buf+l-1, " error");
               output(buf, eOutputSubsystemM590E, eOutputLevelDebug);
            }
            break;
         case 4:
            l = mysprintf(buf, "AT+CSCS=\"8859-1\"\r");
            PT_SPAWN(pt, &pt_m590e_send, M590E_Send(&pt_m590e_send, buf, l, 1, 2000));
            if(strcmp(m590e_data.response[0],"OK")==0) {
               status = 5;
               strcpy(buf+l-1, " ok");
               output(buf, eOutputSubsystemM590E, eOutputLevelDebug);
            }
            else {
               status = 1;
               strcpy(buf+l-1, " error");
               output(buf, eOutputSubsystemM590E, eOutputLevelDebug);
            }
            break;
         case 5:
            l = mysprintf(buf, "AT+CPMS=\"SM\",\"SM\",\"SM\"\r");
            PT_SPAWN(pt, &pt_m590e_send, M590E_Send(&pt_m590e_send, buf, l, 2, 2000));
            if(strcmp(m590e_data.response[1],"OK")==0) {
               status = 6;
               strcpy(buf+l-1, " ok");
               output(buf, eOutputSubsystemM590E, eOutputLevelDebug);
            }
            else {
               status = 1;
               strcpy(buf+l-1, " error");
               output(buf, eOutputSubsystemM590E, eOutputLevelDebug);
            }
            break;
         case 6:
            l = mysprintf(buf, "AT+CPBS=\"SM\"\r");
            PT_SPAWN(pt, &pt_m590e_send, M590E_Send(&pt_m590e_send, buf, l, 1, 2000));
            if(strcmp(m590e_data.response[0],"OK")==0) {
               status = 7;
               strcpy(buf+l-1, " ok");
               output(buf, eOutputSubsystemM590E, eOutputLevelDebug);
            }
            else {
               status = 1;
               strcpy(buf+l-1, " error");
               output(buf, eOutputSubsystemM590E, eOutputLevelDebug);
            }
            break;
         case 7:
            l = mysprintf(buf, "AT+CCLK?\r");
            PT_SPAWN(pt, &pt_m590e_send, M590E_Send(&pt_m590e_send, buf, l, 2, 2000));
            if(strcmp(m590e_data.response[1],"OK")==0) {
               m590e_data.pir_last_time = str2unixtime(m590e_data.response[0]);
               status = 8;
               strcpy(buf+l-1, " ok");
               output(buf, eOutputSubsystemM590E, eOutputLevelDebug);
            }
            else {
               status = 1;
               strcpy(buf+l-1, " error");
               output(buf, eOutputSubsystemM590E, eOutputLevelDebug);
            }
            break;
         case 8:
            l = mysprintf(buf, "AT+CNMI=2,1,2,0,0\r");
            PT_SPAWN(pt, &pt_m590e_send, M590E_Send(&pt_m590e_send, buf, l, 1, 2000));
            if(strcmp(m590e_data.response[0],"OK")==0) {
               m590e_data.ready = 1;
               status = 0;
               tries = 0;
               strcpy(buf+l-1, " ok");
               output(buf, eOutputSubsystemM590E, eOutputLevelDebug);
            }
            else {
               status = 1;
               strcpy(buf+l-1, " error");
               output(buf, eOutputSubsystemM590E, eOutputLevelDebug);
            }
            break;
      }
   }

   m590e_data.mutex = 1;

   PT_END(pt);
}

PT_THREAD(M590E_SMSParse(struct pt *pt)) {
   static char buf[32];
   static int l, ik, k, it, t, status;
   char *s;

   PT_BEGIN(pt);

   PT_WAIT_UNTIL(pt, m590e_data.mutex==1);

   m590e_data.mutex = 0;

   Fifo_Clear(&fifo_m590e_responses);

   status = 1;
   while(status) {
      switch(status) {
         case 1: //paziurim kiek yra zinuciu
            l = mysprintf(buf, "AT+CPMS?\r");
            PT_SPAWN(pt, &pt_m590e_send, M590E_Send(&pt_m590e_send, buf, l, 2, 2000));
            if(strcmp(m590e_data.response[1], "OK")==0 && (s=strstr(m590e_data.response[0], "\"SM\","))!=NULL && (k=atoi(s+5),(s=strstr(s+5,","))!=NULL)) {
               status = (k>0 ? 2 : 0);
               t = atoi(s+1);
               mysprintf(buf+l-1, " ok [%d, %d]", k, t);
               output(buf, eOutputSubsystemM590E, eOutputLevelDebug);
            }
            else {
               status = 0;
               strcpy(buf+l-1, " error");
               output(buf, eOutputSubsystemM590E, eOutputLevelDebug);
            }
            break;
         case 2: //perskaitom zinutes
            for(ik=0,it=1; ik<k && it<=t; it++) {
               l = mysprintf(buf,"AT+CMGR=%d\r",it);
               PT_SPAWN(pt, &pt_m590e_send, M590E_Send(&pt_m590e_send, buf, l, 3, 2000));
               if(strcmp(m590e_data.response[2], "OK")==0) {
                  ik += 1;
                  strcpy(buf+l-1, " ok");
                  output(buf, eOutputSubsystemM590E, eOutputLevelDebug);
                  output(m590e_data.response[0], eOutputSubsystemM590E, eOutputLevelDebug);
                  output(m590e_data.response[1], eOutputSubsystemM590E, eOutputLevelDebug);
                  if((s=strstr(m590e_data.response[0], "REC UNREAD"))!=NULL && (s=strstr(s, "+370"))!=NULL) {
                     Fifo_Put(&fifo_command_parser, m590e_data.response[1]);
                     s[MAX_SRC_SIZE-1] = '\0';
                     m590e_data.source_number = M590E_PhoneBook_Add(s); //sitas pakeis response[] masyva
                     PT_YIELD(pt); //susispenduojam, kad ivyktu komandos apdorojimas
                     m590e_data.source_number = 0;
                  }
               }
               else {
                  strcpy(buf+l-1, " error");
                  output(buf, eOutputSubsystemM590E, eOutputLevelDebug);
               }
            }
            status = 3;
            break;
         case 3:
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
            status = 1;
            break;
      }
   }

   m590e_data.mutex = 1;

   PT_END(pt);
}

PT_THREAD(M590E_SMSSend(struct pt *pt)) {
   static char *p_src, buf[32], sending_sms[MAX_SMS_SIZE];
   static int l, status, sending_sms_len, sending_sms_src;
   char *interim_sms;
   int i, *interim_sms_len, *interim_sms_src;

   PT_BEGIN(pt);

   PT_WAIT_UNTIL(pt, m590e_data.mutex==1);

   m590e_data.mutex = 0;

   Fifo_Clear(&fifo_m590e_responses);

   status = 1;
   while(status) {
      switch(status) {
         case 1: //suformuojama sms'a
            for(sending_sms_len=0; Fifo_SMS_Peek(&interim_sms, &interim_sms_len, &interim_sms_src)==1 && sending_sms_len<MAX_SMS_SIZE-1 && (sending_sms_len==0 ? (sending_sms_src=*interim_sms_src) : sending_sms_src==*interim_sms_src); ) { //-1 nes sms'o gale dar pridesim \x1a; interim_sms_src negali buti 0 (tikrinama kai dedama i fifo)
               l = MIN2(MAX_SMS_SIZE-1-sending_sms_len, *interim_sms_len);
               strncpy(sending_sms+sending_sms_len, interim_sms, l);
               sending_sms_len += l;
               if(l == *interim_sms_len) { //tilpo pilnas sms'as
                  if(sending_sms_len < MAX_SMS_SIZE-1) sending_sms[sending_sms_len++] = '\n';
                  Fifo_SMS_Remove();
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
         case 2: //nuskaitome phone book'a i kuri numeri siusime (cia turiu tik indeksa)
            l = mysprintf(buf, "AT+CPBR=%d\r", sending_sms_src);
            PT_SPAWN(pt, &pt_m590e_send, M590E_Send(&pt_m590e_send, buf, l, 2, 2000));
            if(strcmp(m590e_data.response[1],"OK")==0 && (p_src=strstr(m590e_data.response[0],"+370"))!=NULL) {
               p_src[MAX_SRC_SIZE-1] = '\0';
               status = 3;
               strcpy(buf+l-1, " ok");
               output(buf, eOutputSubsystemM590E, eOutputLevelDebug);
            }
            else if(strcmp(m590e_data.response[0],"OK")==0) { //atsakymas gautas, bet tokio indekso numerio nera
               status = Fifo_SMS_Count()>0;
               strcpy(buf+l-1, " ok?");
               output(buf, eOutputSubsystemM590E, eOutputLevelDebug);
            }
            else {
               status = 0;
               strcpy(buf+l-1, " error");
               output(buf, eOutputSubsystemM590E, eOutputLevelDebug);
            }
            break;
         case 3: //issiunciam numeri
            l = mysprintf(buf, "AT+CMGS=\"%s\"\r", p_src);
            PT_SPAWN(pt, &pt_m590e_send, M590E_Send(&pt_m590e_send, buf, l, 1, 2000));
            if(strcmp(m590e_data.response[0],">") == 0) {
               status = 4;
               strcpy(buf+l-1, " ok");
               output(buf, eOutputSubsystemM590E, eOutputLevelDebug);
            }
            else {
               status = 0;
               strcpy(buf+l-1, " error");
               output(buf, eOutputSubsystemM590E, eOutputLevelDebug);
            }
            break;
         case 4: //issiunciam zinute
            PT_SPAWN(pt, &pt_m590e_send, M590E_Send(&pt_m590e_send, sending_sms, sending_sms_len, 2, 10000));
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

   m590e_data.mutex = 1;

   PT_END(pt);
}

PT_THREAD(M590E_SMSPeriodic(struct pt *pt)) {
   static int i, j;

   PT_BEGIN(pt);

   PT_WAIT_UNTIL(pt, m590e_data.mutex==1);

   m590e_data.mutex = 0;

   for(i=0; i<PERIODIC_SMS_RECIPIENTS; i++) {
      if(m590e_data.periodic_sms[i].src_index != 0) {
         m590e_data.source_number = m590e_data.periodic_sms[i].src_index;
         for(j=0; j<PERIODIC_SMS_COMMANDS; j++) {
            if(strcmp(m590e_data.periodic_sms[i].commands[j],"")!=0 && strcmp(m590e_data.periodic_sms[i].commands[j],"pir")!=0) {
               Fifo_Put(&fifo_command_parser, m590e_data.periodic_sms[i].commands[j]);
               PT_YIELD(pt);
            }
         }
         m590e_data.source_number = 0;
      }
   }

   m590e_data.mutex = 1;

   PT_END(pt);
}

PT_THREAD(M590E_SMSPIR(struct pt *pt)) {
   static char buf[16];
   static int i, j, l, status;
   int t;

   PT_BEGIN(pt);

   PT_WAIT_UNTIL(pt, m590e_data.mutex==1);

   m590e_data.mutex = 0;

   status = 1;
   while(status) {
      switch(status) {
         case 1:
            l = mysprintf(buf, "AT+CCLK?\r");
            PT_SPAWN(pt, &pt_m590e_send, M590E_Send(&pt_m590e_send, buf, l, 2, 2000));
            if(strcmp(m590e_data.response[1], "OK")==0) {
               status = 2;
               strcpy(buf+l-1, " ok");
               output(buf, eOutputSubsystemM590E, eOutputLevelDebug);
            }
            else {
               status = 0;
               strcpy(buf+l-1, " error");
               output(buf, eOutputSubsystemM590E, eOutputLevelDebug);
            }
            break;
         case 2:
            t = str2unixtime(m590e_data.response[0]);
            if(t-m590e_data.pir_last_time >= m590e_data.pir_sms_interval) {
               status = 3;
               m590e_data.pir_last_time = t;
            }
            else status = 0;
            break;
         case 3:
            for(i=0; i<PERIODIC_SMS_RECIPIENTS; i++) { //PIR zinute gaus tie kas uzsisake periodini "pir" gavima
               if(m590e_data.periodic_sms[i].src_index != 0) {
                  for(j=0; j<PERIODIC_SMS_COMMANDS && strcmp(m590e_data.periodic_sms[i].commands[j], "pir")!=0; j++);
                  if(j<PERIODIC_SMS_COMMANDS) {
                     m590e_data.source_number = m590e_data.periodic_sms[i].src_index;
                     Fifo_Put(&fifo_command_parser, m590e_data.periodic_sms[i].commands[j]);
                     PT_YIELD(pt);
                     m590e_data.source_number = 0;
                  }
               }
            }
            status = 0;
            break;
      }
   }

   m590e_data.mutex = 1;

   PT_END(pt);
}

void M590E_Periodic_Interval(int n) {
   m590e_data.periodic_sms_interval=n;
   WKT_Set(m590e_data.periodic_sms_interval);
}

int M590E_Periodic_Add(int src_index, char *cmd) { //cmd gali buti "\0"
   int i, j, res;
   for(i=0; i<PERIODIC_SMS_RECIPIENTS && m590e_data.periodic_sms[i].src_index!=src_index; i++);
   if(i == PERIODIC_SMS_RECIPIENTS) //jau esantis numeris nerastas -- ieskosim laisvos vietos
      for(i=0; i<PERIODIC_SMS_RECIPIENTS && m590e_data.periodic_sms[i].src_index!=0; i++);
   if(i<PERIODIC_SMS_RECIPIENTS) { //rastas jau esantis gavejas arba sukurtas naujas
      m590e_data.periodic_sms[i].src_index = src_index;
      for(j=0; j<PERIODIC_SMS_COMMANDS && strncmp(m590e_data.periodic_sms[i].commands[j],"",1)!=0; j++);
      if(j < PERIODIC_SMS_COMMANDS) { //rasta laisva vieta
         strncpy(m590e_data.periodic_sms[i].commands[j], cmd, PERIODIC_SMS_COMMAND_SIZE-1);
         m590e_data.periodic_sms[i].commands[j][PERIODIC_SMS_COMMAND_SIZE-1] = '\0';
         res = OK;
      }
      else res = ER;
   }
   else res = ER;
   return res;
}

void M590E_Periodic_Clear(int src_index) {
   int i, j;
   for(i=0; i<PERIODIC_SMS_RECIPIENTS && m590e_data.periodic_sms[i].src_index!=src_index; i++);
   if(i<PERIODIC_SMS_RECIPIENTS) {
      m590e_data.periodic_sms[i].src_index = 0;
      for(j=0; j<PERIODIC_SMS_COMMANDS; j++)
         strcpy(m590e_data.periodic_sms[i].commands[j], "\0");
   }
}

int M590E_PhoneBook_Add(char *src) { //funkcija grazina telefono indeksa phone book'e [jei reikia, iraso]
   char *p, buf[64];
   int pb_index, l;
   l = mysprintf(buf, "AT+CPBF=\"%s\"\r", src);
   M590E_Send_Blocking(buf, l, -2, 10000);
   if(strcmp(m590e_data.response[0], "OK")==0) {
      l = mysprintf(buf, "AT+CPBS?\r");
      M590E_Send_Blocking(buf, l, -2, 2000);
      if(strcmp(m590e_data.response[1], "OK")==0 && (p=strstr(m590e_data.response[0], "\"SM\","))!=NULL) {
         pb_index = atoi(p+5) + 1; //5 yra "'SM'," ilgis
         l = mysprintf(buf, "AT+CPBW=%d,\"%s\",145,\"%s\"\r", pb_index, src, src);
         M590E_Send_Blocking(buf, l, -1, 2000);
         if(strcmp(m590e_data.response[0], "OK")!=0) pb_index = 0;
      }
      else pb_index = 0;
   }
   else if(strcmp(m590e_data.response[1], "OK")==0 && (p=strstr(m590e_data.response[0], "+CPBF: "))!=NULL) {
      pb_index = atoi(p+7); //7 yra "+CPBF: " ilgis
   }
   else {
      pb_index = 0;
   }
   return pb_index;
}
