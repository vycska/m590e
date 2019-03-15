#include "fifos.h"
#include "utils.h"
#include "tlsf.h"
#include <string.h>

extern tlsf_t tlsf;

static struct Fifo_SMS fifo_sms;

void Fifo_Init(struct Fifo *fifo) {
   fifo->count = 0;
   fifo->head = 0;
}

void Fifo_Clear(struct Fifo *fifo) {
   struct Fifo_Item *cursor;
   fifo->count = 0;
   for(cursor=fifo->head; cursor!=0; fifo->head=fifo->head->next, tlsf_free(tlsf, cursor->s), tlsf_free(tlsf, cursor), cursor=fifo->head);
}

int Fifo_Peek(struct Fifo *fifo, char **ps) {
   int res;
   if(fifo->count > 0) {
      *ps = fifo->head->s;
      res = 1;
   }
   else {
      *ps = 0;
      res = 0;
   }
   return res;
}

void Fifo_Remove(struct Fifo *fifo) {
   struct Fifo_Item *cursor;
   if(fifo->count > 0) {
      cursor = fifo->head;
      fifo->head = fifo->head->next;
      tlsf_free(tlsf, cursor->s);
      tlsf_free(tlsf, cursor);
      fifo->count -= 1;
   }
}

void Fifo_Put(struct Fifo *fifo, char *s) {
   struct Fifo_Item *new, *cursor;
   if((new=tlsf_malloc(tlsf, sizeof(struct Fifo_Item))) != 0) {
      if((new->s=tlsf_malloc(tlsf, strlen(s)+1)) != 0) {
         strcpy(new->s, s);
         new->next = 0;
         if(fifo->head == 0) {
            fifo->head = new;
         }
         else {
            for(cursor=fifo->head; cursor->next != 0; cursor=cursor->next);
            cursor->next = new;
         }
         fifo->count += 1;
      }
      else {
         tlsf_free(tlsf, new);
      }
   }
}

void Fifo_SMS_Init(void) {
   fifo_sms.count = 0;
   fifo_sms.head = 0;
}

int Fifo_SMS_Peek(char **msg, int **len, int **src) {
   int res;
   if(fifo_sms.count > 0) {
      *msg = fifo_sms.head->msg;
      *len = &fifo_sms.head->len;
      *src = &fifo_sms.head->src;
      res = 1;
   }
   else res = 0;
   return res;
}

void Fifo_SMS_Remove(void) {
   struct Fifo_SMS_Item *cursor;
   if(fifo_sms.count > 0) {
      cursor = fifo_sms.head;
      fifo_sms.head = fifo_sms.head->next;
      tlsf_free(tlsf, cursor->msg);
      tlsf_free(tlsf, cursor);
      fifo_sms.count -= 1;
   }
}

void Fifo_SMS_Put(char *msg, int len, int src) {
   struct Fifo_SMS_Item *new, *cursor;
   if((new=tlsf_malloc(tlsf, sizeof(struct Fifo_SMS_Item))) != 0) {
      if((new->msg=tlsf_malloc(tlsf, strlen(msg)+1)) != 0) {
         strcpy(new->msg, msg);
         new->len = len;
         new->src = src;
         new->next = 0;
         if(fifo_sms.head == 0) {
            fifo_sms.head = new;
         }
         else {
            for(cursor=fifo_sms.head; cursor->next != 0; cursor=cursor->next);
            cursor->next = new;
         }
         fifo_sms.count += 1;
      }
      else {
         tlsf_free(tlsf, new);
      }
   }
}

int Fifo_SMS_Count(void) {
   return fifo_sms.count;
}
