#include "fifos.h"
#include <string.h>

static struct Fifo_SMS fifo_sms;

void Fifo_SMS_Init(void) {
   fifo_sms.count = 0;
   fifo_sms.i_get = 0;
   fifo_sms.i_put = 0;
}

void Fifo_SMS_Put(char *msg, int len, char *src) {
   if(fifo_sms.count < FIFO_SMS_ITEMS) {
      strncpy(fifo_sms.item[fifo_sms.i_put].msg, msg, len);
      fifo_sms.item[fifo_sms.i_put].msg[len] = '\0';
      strcpy(fifo_sms.item[fifo_sms.i_put].src, src);
      fifo_sms.item[fifo_sms.i_put].len = len;
      fifo_sms.count += 1;
      fifo_sms.i_put += 1;
      if(fifo_sms.i_put == FIFO_SMS_ITEMS)
         fifo_sms.i_put = 0;
   }
}

int Fifo_SMS_Peek(struct Fifo_SMS_Item *sms) {
   int res;
   if(fifo_sms.count > 0) {
      strcpy(sms->msg, fifo_sms.item[fifo_sms.i_get].msg);
      strcpy(sms->src, fifo_sms.item[fifo_sms.i_get].src);
      sms->len = fifo_sms.item[fifo_sms.i_get].len;
      res = 1;
   }
   else res = 0;
   return res;
}

void Fifo_SMS_Change(char *msg, int len, char *src) {
   if(fifo_sms.count > 0) {
      strcpy(fifo_sms.item[fifo_sms.i_get].msg, msg);
      strcpy(fifo_sms.item[fifo_sms.i_get].src, src);
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
