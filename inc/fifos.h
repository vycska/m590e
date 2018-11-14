#ifndef __FIFOS_H__
#define __FIFOS_H__

#define FIFO_ITEMS 4
#define FIFO_ITEM_SIZE 64

struct Fifo {
   volatile char buffer[FIFO_ITEMS][FIFO_ITEM_SIZE];
   volatile int count;
   int i_get, i_put;
};
void Fifo_Init(struct Fifo*);
int Fifo_Get(struct Fifo*, char**);
void Fifo_Put(struct Fifo*, char*);

#endif
