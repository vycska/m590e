#include "fifos.h"
#include <string.h>

static struct Fifo_SMS fifo_sms;

void Fifo_Init(struct Fifo *fifo) {
   fifo->count = 0;
   fifo->i_get = 0;
   fifo->i_put = 0;
}

void Fifo_Clear(struct Fifo *fifo) {
   fifo->count = 0;
   fifo->i_get = 0;
   fifo->i_put = 0;
}

int Fifo_Peek(struct Fifo *fifo, char **ps) {
   int res;
   if(fifo->count > 0) {
      *ps = (char*)fifo->buffer[fifo->i_get];
      res = 1;
   }
   else {
      *ps = 0;
      res = 0;
   }
   return res;
}

void Fifo_Remove(struct Fifo *fifo) {
   if(fifo->count > 0) {
      fifo->i_get += 1;
      if(fifo->i_get == FIFO_ITEMS)
         fifo->i_get = 0;
      fifo->count -= 1;
   }
}

void Fifo_Put(struct Fifo *fifo, char *s) {
   if(fifo->count < FIFO_ITEMS) {
      strncpy((char*)fifo->buffer[fifo->i_put], s, FIFO_ITEM_SIZE-1);
      fifo->buffer[fifo->i_put][FIFO_ITEM_SIZE-1] = '\0';
      fifo->i_put += 1;
      if(fifo->i_put == FIFO_ITEMS)
         fifo->i_put = 0;
      fifo->count += 1;
   }
}

void Fifo_SMS_Init(void) {
   fifo_sms.count = 0;
   fifo_sms.i_get = 0;
   fifo_sms.i_put = 0;
}

void Fifo_SMS_Put(char *msg, int len) {
   if(fifo_sms.count < FIFO_SMS_ITEMS) {
      strncpy(fifo_sms.item[fifo_sms.i_put].msg, msg, len);
      fifo_sms.item[fifo_sms.i_put].msg[len] = '\0';
      fifo_sms.item[fifo_sms.i_put].len = len;
      fifo_sms.count += 1;
      fifo_sms.i_put += 1;
      if(fifo_sms.i_put == FIFO_SMS_ITEMS)
         fifo_sms.i_put = 0;
   }
}

int Fifo_SMS_Peek(char **msg, int **len) {
   int res;
   if(fifo_sms.count > 0) {
      *msg = fifo_sms.item[fifo_sms.i_get].msg;
      *len = &fifo_sms.item[fifo_sms.i_get].len;
      res = 1;
   }
   else res = 0;
   return res;
}

void Fifo_SMS_Change(char *msg, int len) {
   if(fifo_sms.count > 0) {
      strcpy(fifo_sms.item[fifo_sms.i_get].msg, msg);
      fifo_sms.item[fifo_sms.i_get].len = len;
   }
}

void Fifo_SMS_Remove(void) {
   if(fifo_sms.count > 0) {
      fifo_sms.i_get += 1;
      if(fifo_sms.i_get == FIFO_SMS_ITEMS)
         fifo_sms.i_get = 0;
      fifo_sms.count -= 1;
   }
}

int Fifo_SMS_Count(void) {
   return fifo_sms.count;
}
