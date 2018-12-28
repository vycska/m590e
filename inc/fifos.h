#ifndef __FIFOS_H__
#define __FIFOS_H__

#include "m590e.h"

#define FIFO_ITEMS 8
#define FIFO_ITEM_SIZE 80

#define FIFO_SMS_ITEMS 24
#define FIFO_SMS_SRC_SIZE MAX_SRC_SIZE
#define FIFO_SMS_MSG_SIZE 32

struct Fifo {
   volatile char buffer[FIFO_ITEMS][FIFO_ITEM_SIZE];
   volatile int count;
   int i_get, i_put;
};

struct Fifo_SMS_Item {
   char src[FIFO_SMS_SRC_SIZE], msg[FIFO_SMS_MSG_SIZE];
   int len;
};

struct Fifo_SMS {
   struct Fifo_SMS_Item item[FIFO_SMS_ITEMS];
   volatile int count;
   int i_get, i_put;
};

void Fifo_Init(struct Fifo*);
void Fifo_Clear(struct Fifo*);
int Fifo_Peek(struct Fifo*, char**);
void Fifo_Remove(struct Fifo*);
void Fifo_Put(struct Fifo*, char*);

void Fifo_SMS_Init(void);
void Fifo_SMS_Put(char *msg, int len, char *src);
int Fifo_SMS_Peek(char **msg, int **len, char **src);
void Fifo_SMS_Change(char *msg, int len, char *src);
void Fifo_SMS_Remove(void);
int Fifo_SMS_Count(void);

#endif
