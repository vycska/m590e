#include "dump.h"
#include "output.h"
#include "utils.h"
#include <ctype.h>

#define DUMP_SIZE 100

struct Dump {
   unsigned char data[DUMP_SIZE];
   int index;
};

static struct Dump dump;

void dump_put(unsigned char c) {
   if(dump.index < DUMP_SIZE)
      dump.data[dump.index] = c;
   dump.index += 1;
}

void dump_print(void) {
   char buf[16];
   int i;
   for(i=0; i<dump.index && i<DUMP_SIZE; i++) {
      mysprintf(buf, "[%d]: %c [%d]", i, isprint(dump.data[i])?dump.data[i]:' ', (int)dump.data[i]);
      output(buf, eOutputSubsystemSystem, eOutputLevelNormal);
   }
   dump.index = 0;
}
