#include "m590e.h"
#include "fifos.h"
#include "mrt.h"
#include "uart.h"
#include "lpc824.h"
#include "output.h"
#include "pt.h"
#include "timer.h"
#include "utils.h"
#include <string.h>

extern volatile unsigned int gInterruptCause;
extern struct Fifo fifo_command_parser;
extern struct Fifo fifo_m590e_responses;

struct pt pt_m590e_ring;
struct timer timer_m590e_ring;
struct M590E_Data m590e_data;
struct Ring_Data ring_data;

void M590E_Init(void) {
   //Ring on PIO0_17
   PINENABLE0 |= (1<<22); //ADC_9 disabled on PIO0_17
   PIO0_17 = (2<<3 | 0<<5 | 0<<6 | 0<<10 | 0<<11 | 0<<13); //pu resistor, no hysteresis, input not inverted, no open-drain mode, input filter bypassed, clock IOCONCLKDIV0
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
}

int Ring_Active(void) {
   return ((PIN0>>17)&1) == 0;
}

void PININT1_IRQHandler(void) {
   CIENF = (1<<1); //disable falling edge interrupt
   FALL = (1<<1); //clear detected falling edge
   gInterruptCause |= (1<<1);
   if(ring_data.active == 0) {
      ring_data.active = 1;
      ring_data.delay = 0;
   }
}

PT_THREAD(Handle_M590E_Ring(struct pt *pt)) {
   static int i, empty;
   char *p, *s, buf[12];
   int l;
   PT_BEGIN(pt);
   while(Fifo_Get(&fifo_m590e_responses, &s));
   UART1_Transmit("ATE0\r",5);
   timer_set(&timer_m590e_ring, 1000);
   PT_WAIT_UNTIL(pt, (s=0, Fifo_Get(&fifo_m590e_responses, &s)==1) || timer_expired(&timer_m590e_ring));
   if(s!=0 && (strcmp(s,"OK")==0 || (strcmp(s,"ATE0")==0 && Fifo_Get(&fifo_m590e_responses, &s)==1 && strcmp(s,"OK")==0))) {
      UART1_Transmit("AT+CMGF=1\r",10);
      timer_set(&timer_m590e_ring, 1000);
      PT_WAIT_UNTIL(pt, (s=0, Fifo_Get(&fifo_m590e_responses, &s)==1) || timer_expired(&timer_m590e_ring));
      if(s!=0 && strcmp(s,"OK")==0) {
         UART1_Transmit("AT+CSCS=\"GSM\"\r", 14);
         timer_set(&timer_m590e_ring, 1000);
         PT_WAIT_UNTIL(pt, (s=0, Fifo_Get(&fifo_m590e_responses, &s)==1) || timer_expired(&timer_m590e_ring));
         if(s!=0 && strcmp(s, "OK")==0) {
            for(empty=0,i=1; !empty; i++) {
               l = mysprintf(buf,"AT+CMGR=%d\r",i);
               UART1_Transmit(buf, l);
               timer_set(&timer_m590e_ring, 1000);
               PT_WAIT_UNTIL(pt, (s=0, Fifo_Get(&fifo_m590e_responses, &s)==1) || timer_expired(&timer_m590e_ring));
               if(s!=0 && strcmp(s,"ERROR")!=0) {
                  if((p=strstr(s,"REC UNREAD")) != 0) {
                     p = strchr(p, '+');
                     if(p != 0) {
                        strncpy(m590e_data.source_number, p, 12);
                        m590e_data.source_number[12] = 0;
                        timer_set(&timer_m590e_ring, 1000);
                        PT_WAIT_UNTIL(pt, (s=0, Fifo_Get(&fifo_m590e_responses, &s)==1) || timer_expired(&timer_m590e_ring));
                        if(s != 0) {
                           Fifo_Put(&fifo_command_parser, s);
                           PT_YIELD(pt); //susispenduojam, kad ivyktu komandos apdorojimas
                           timer_set(&timer_m590e_ring, 1000);
                           PT_WAIT_UNTIL(pt, (s=0, Fifo_Get(&fifo_m590e_responses, &s)==1) || timer_expired(&timer_m590e_ring));
                           if(s==0 || strcmp(s, "OK")!=0)
                              empty=1;
                        }
                        else empty=1;
                     }
                     else empty=1;
                  }
               }
               else empty=1;
            }
            UART1_Transmit("AT+CMGD=1,1\r", 12); //istrinam visas perskaitytas zinutes
            timer_set(&timer_m590e_ring, 1000);
            PT_WAIT_UNTIL(pt, (s=0, Fifo_Get(&fifo_m590e_responses, &s)==1) || timer_expired(&timer_m590e_ring));
            while(Fifo_Get(&fifo_m590e_responses, &s));
         }
      }
   }

   PT_END(pt);
}
