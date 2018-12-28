#ifndef __UART_H__
#define __UART_H__

#define UART0_BAUDRATE (38400)
#define UART1_BAUDRATE (38400)
#define UART0_IN_MAX 80 //FIFO_ITEM_SIZE irgi 80
#define UART1_IN_MAX 80 //FIFO_ITEM_SIZE irgi 80

enum UART0ReceivingMode {
   eUART0ReceivingModeCommands = 0,
};

struct UART0_Data {
   enum UART0ReceivingMode mode;
   char s[UART0_IN_MAX];
   int i;
};

struct UART1_Data {
   char s[UART1_IN_MAX];
   int i;
};

void UART0_Init(void);
void UART0_Transmit(char*,int,int);
void UART0_ReceivingMode_Change(enum UART0ReceivingMode);
void UART0_ReceivingData_Reset(void);
void UART1_Init(void);
void UART1_Transmit(char*,int);

#endif
