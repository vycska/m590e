#include "utils.h"
#include "lpc824.h"
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int gcd(int a, int b) {
   return a == 0 ? b : gcd(b % a, a);
}

unsigned short crc16(unsigned char *p, unsigned char l) {
   unsigned char i, j;
   unsigned short crc = 0xffff;

   for(i = 0; i < l; i++) {
      crc ^= p[i];
      for(j = 0; j < 8; j++) {
         if(crc & 0x01) {
            crc >>= 1;
            crc ^= 0xA001;
         }
         else
            crc >>= 1;
      }
   }
   return crc;
}

void streverse(char *s, int l) {
   char t;
   int i;

   for(i = 0; i < l / 2; i++) {
      t = s[i];
      s[i] = s[l - 1 - i];
      s[l - 1 - i] = t;
   }
}

int dec2hex2str(unsigned long long int n, char *s) {
   int i, j;

   for(i = 0; s[i] = n % 16, n /= 16; i++);
   for(j = 0; j <= i; j++)
      s[j] = s[j] < 10 ? s[j] + '0' : s[j] - 10 + 'a';
   streverse(s, i + 1);
   return i + 1;
}

int dec2str(long long int n, char *s) {
   int i, j;

   for(j = i = (n < 0 ? (n = -n, s[0] = '-', 1) : 0); s[i] = n % 10 + '0', n /= 10; i++);
   streverse(s + j, i - j + 1);
   return i + 1;
}

int double2str(double d, int p, char *s) {
   int i, j, n;

   for(j = i = ((d < 0.0) ? (d = -d, s[0] = '-', 1) : 0), n = (int)d; n > 0 || j == i; (s[j++] = '0' + n % 10), n /= 10);
   for(streverse(s + i, j - i), (s[j++] = '.'), i = 0; i < p; (d -= (int)d), (d *= 10), (s[j++] = '0' + (int)d), i++);
   return j;
}

int mysprintf(char *buf, const char *format, ...) {
   char *s;
   int i, j, k, d;
   unsigned int u;
   long long int l;
   double f;
   va_list va;

   va_start(va, format);
   for(j = i = 0; format[i] != 0; j += k, i++) {
      if(format[i] != '%') {
         k = 1;
         buf[j] = format[i];
      }
      else {
         switch (format[++i]) {
            case '%':
               buf[j] = '%';
               k = 1;
               break;
            case 'd':
               d = va_arg(va, int);
               k = dec2str(d, &buf[j]);
               break;
            case 'l':
               l = *((long long int *)va_arg(va, char *));
               k = dec2str(l, &buf[j]);
               break;
            case 'u':
               u = va_arg(va, unsigned int);
               k = dec2str(u, &buf[j]);
               break;
            case 'x':
               u = va_arg(va, unsigned int);
               k = dec2hex2str(u, &buf[j]);
               break;
            case 'f':
               f = *((double*)va_arg(va,char*));
               k = double2str(f, format[++i] - '0', &buf[j]);
               break;
            case 'c':
               buf[j] = va_arg(va, int);
               k = 1;
               break;
            case 's':
               s = va_arg(va, char *);
               strcpy(&buf[j], s);
               k = strlen(s);
               break;
            case 'n':
               k = va_arg(va, int);
               for(d=j;d<k+j;buf[d++]=' ');
               break;
            default:
               buf[j] = '?';
               k = 1;
               break;
         }
      }
   }
   buf[j] = 0;
   va_end(va);
   return j;
}

long long int power(int a, int k) {
   long long int d = a, s = 1;

   while(k >= 1) {
      if(k % 2 == 0)
         d *= d, k /= 2;
      else
         s *= d, k -= 1;
   }
   return s;
}

unsigned char reflect_byte(unsigned char b) {
   b = ((b & 0xf0) >> 4) | ((b & 0xf) << 4);
   b = ((b & 0xcc) >> 2) | ((b & 0x33) << 2);
   b = ((b & 0xaa) >> 1) | ((b & 0x55) << 1);
   return b;
}

int ndigits(int n) {
   int k=0;
   do {
      n/=10;
      k+=1;
   } while(n>0);
   return k;
}

int str2unixtime(char *s) {
   static int mdays[12] = {0,31,59,90,120,151,181,212,243,273,304,334};
   int stage, t=0;
   struct tm dt = {0};
   stage = 1;
   while(stage != 0) {
      switch(stage) {
         case 1:
            if((s=strchr(s, '\"')) != NULL) {
               dt.tm_year = atoi(s+1) + 2000 - 1900;
               stage = 2;
            }
            else stage = 100;
            break;
         case 2:
            if((s=strchr(s+1, '/')) != NULL) {
               dt.tm_mon = atoi(s+1) - 1;
               stage = 3;
            }
            else stage = 100;
            break;
         case 3:
            if((s=strchr(s+1, '/')) != NULL) {
               dt.tm_mday = atoi(s+1);
               stage = 4;
            }
            else stage = 100;
            break;
         case 4:
            if((s=strchr(s+1, ',')) != NULL) {
               dt.tm_hour = atoi(s+1);
               stage = 5;
            }
            else stage = 100;
            break;
         case 5:
            if((s=strchr(s+1, ':')) != NULL) {
               dt.tm_min = atoi(s+1);
               stage = 6;
            }
            else stage = 100;
            break;
         case 6:
            if((s=strchr(s+1, ':')) != NULL) {
               dt.tm_sec = atoi(s+1);
               stage = 7;
            }
            else stage = 100;
            break;
         case 7: //all ok
            t = ((dt.tm_year+1900-1970)*365 + ((dt.tm_year+1900-1-1968)>>2) + mdays[dt.tm_mon+1-1] + (dt.tm_mday-1))*86400 + dt.tm_hour*3600 + dt.tm_min*60 + dt.tm_sec;
            stage = 0;
            break;
         case 100: //something wrong
            t = 0;
            stage = 0;
            break;
      }
   }
   return t;
}
