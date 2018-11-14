#include "fifos.h"
#include <string.h>

void Fifo_Init(struct Fifo *fifo) {
   fifo->count = 0;
   fifo->i_get = 0;
   fifo->i_put = 0;
}

int Fifo_Get(struct Fifo *fifo, char **pString) {
   int res;
   if(fifo->count > 0) {
      *pString = (char*)fifo->buffer[fifo->i_get];
      fifo->i_get = (fifo->i_get + 1) & (FIFO_ITEMS-1);
      fifo->count -= 1;
      res = 1;
   }
   else res = 0;
   return res;
}

void Fifo_Put(struct Fifo *fifo, char *pString) {
   if(fifo->count<FIFO_ITEMS) {
      strcpy((char*)fifo->buffer[fifo->i_put], pString);
      fifo->i_put = (fifo->i_put + 1) & (FIFO_ITEMS-1);
      fifo->count += 1;
   }
}
