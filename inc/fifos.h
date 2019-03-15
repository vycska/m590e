#ifndef __FIFOS_H__
#define __FIFOS_H__

#include "m590e.h"

struct Fifo_Item {
   char *s;
   struct Fifo_Item *next;
};

struct Fifo {
   volatile int count;
   struct Fifo_Item *head;
};

struct Fifo_SMS_Item {
   char *msg;
   int src, len;
   struct Fifo_SMS_Item *next;
};

struct Fifo_SMS {
   volatile int count;
   struct Fifo_SMS_Item *head;
};

void Fifo_Init(struct Fifo*);
void Fifo_Clear(struct Fifo*);
int Fifo_Peek(struct Fifo*, char**);
void Fifo_Remove(struct Fifo*);
void Fifo_Put(struct Fifo*, char*);

void Fifo_SMS_Init(void);
void Fifo_SMS_Put(char *msg, int len, int src);
int Fifo_SMS_Peek(char **msg, int **len, int **src);
void Fifo_SMS_Remove(void);
int Fifo_SMS_Count(void);

#endif
