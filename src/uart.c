#include "uart.h"
#include "clocks.h"
#include "fifos.h"
#include "main.h"
#include "output.h"
#include "utils.h"
#include "utils-asm.h"
#include "lpc824.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

extern volatile unsigned int gInterruptCause;
extern struct Fifo fifo_command_parser;
extern struct Fifo fifo_m590e_responses;

static struct UART0_Data uart0_data;
static struct UART1_Data uart1_data;

void UART0_Init(void) {
   USART0CFG &= (~(1<<0)); //disable usart0
   PINENABLE0 |= (1<<0 | 1<<24); //disable ACMP_I1 on PIO0_0, disable ADC_11 on PIO0_4
   PINASSIGN0 = (PINASSIGN0&(~(0xff<<0 | 0xff<<8))) | (4<<0 | 0<<8); //U0TXD assigned to PIO0_4, U0RXD assigned to PIO0_0
   USART0BRG = ((UART_CLOCK+8*UART0_BAUDRATE)/(16*UART0_BAUDRATE)-1); //this value+1 is used to divide clock to determine the baud rate
   USART0OSR= 0xf; //oversample 16
   USART0CTL = (0<<1 | 0<<2 | 0<<6 | 0<<16); //no break, no address detect mode, transmit not disabled, autobaud disabled
   USART0INTENSET = (1<<0); //interrupt when there is a received character
   IPR0 = (IPR0&(~(3u<<30))) | (2u<<30); //UART0 interrupt priority 2 (0 = highest, 3 = lowest)
   ISER0 = (1<<3); //UART0 interrupt enable
   USART0CFG = (1<<0 | 1<<2 | 0<<4 | 0<<6 | 0<<9 | 0<<11 | 0<<15); //USART0 enable, 8b data length, no parity, 1 stop bit, no flow control, asynchronous mode, no loopback mode
}

void UART0_IRQHandler(void) {
   unsigned char c;
   if(USART0INTSTAT&(1<<0)) { //RXRDY
      c = USART0RXDAT&0xff;
      switch(uart0_data.mode) {
         case eUART0ReceivingModeCommands:
            if(isprint(c)) {
               uart0_data.s[uart0_data.i++] = c;
               if(uart0_data.i>=UART0_IN_MAX)
                  uart0_data.i = 0;
            }
            else if(uart0_data.i != 0) {
               uart0_data.s[uart0_data.i] = 0;
               uart0_data.i = 0;
               Fifo_Put(&fifo_command_parser, uart0_data.s);
            }
            break;
      }
   }
}

void UART0_Transmit(char *s,int k,int flag_addcrlf) {
   int i;
   for(i=0;i<k+(flag_addcrlf?2:0);i++) { //+2 nes gale pridesim \r\n
      while((USART0STAT&(1<<2))==0); //wait until TXRDY
      USART0TXDAT = (i == k ? '\r' : (i == k + 1 ? '\n' : s[i]));
   }
}

void UART0_ReceivingMode_Change(enum UART0ReceivingMode m) {
   uart0_data.mode = m;
}

void UART0_ReceivingData_Reset(void) {
   uart0_data.i = 0;
}

void UART1_Init(void) {
   USART1CFG = 0; //disable USART1
   PINENABLE0 |= (1<<4 | 1<<5); //SWCLK disabled on PIO0_3, SWDIO disabled on PIO0_2
   PINASSIGN1 = (PINASSIGN1 & (~(0xff<<8 | 0xff<<16))) | (2<<8 | 3<<16); //U1_TXD is PIO0_2, U1_RXD is PIO0_3
   USART1BRG = (UART_CLOCK+8*UART1_BAUDRATE)/(16*UART1_BAUDRATE)-1;
   USART1OSR = (0xf<<0); //oversample 16
   USART1CTL = (0<<1 | 0<<2 | 0<<6 | 0<<16); //no continuous break sending, disable address detect mode, transmitter not disabled, autobaud disabled
   USART1INTENCLR = (1<<2 | 1<<3 | 1<<5 | 1<<6 | 1<<8 | 1<<11 | 1<<12 | 1<<13 | 1<<14 | 1<<15 | 1<<16); //clear all interrupts except RXRDY
   USART1INTENSET = (1<<0); //interrupt when there is a received character available
   IPR1 = (IPR1 & (~(0x3<<6))) | (2<<6);
   ISER0 = (1<<4); //UART1 interrupt enable
   USART1CFG = (1<<0 | 1<<2 | 0<<4 | 0<<6 | 0<<9 | 0<<11 | 0<<15); //enable USART1, 8b data length, no parity, 1 stop bit, no flow control, asynchronous mode, no loopback mode
}

void UART1_Transmit(char *s,int k) {
   int i;
   for(i=0;i<k;i++) {
      while((USART1STAT&(1<<2))==0); //wait until TXRDY
      USART1TXDAT = s[i];
   }
}

void UART1_IRQHandler(void) {
   unsigned char c;
   if((USART1STAT>>0)&1) { //RXRDY
      c = USART1RXDAT&0xff;
      if(isprint(c)) {
         uart1_data.s[uart1_data.i++] = c;
         if(uart1_data.i >= UART1_IN_MAX)
            uart1_data.i = 0;
      }
      else if(uart1_data.i != 0) {
         uart1_data.s[uart1_data.i] = 0;
         uart1_data.i = 0;
         Fifo_Put(&fifo_m590e_responses, uart1_data.s);
      }
   }
}

int UART1_Get(unsigned char **s) {
   *s = (unsigned char*)uart1_data.s;
   return uart1_data.i;
}
