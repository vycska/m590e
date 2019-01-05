#ifndef __BOOZER_H__
#define __BOOZER_H__

struct Boozer_Data {
   volatile int active, duration;
};

void Boozer_Init(void);
void Boozer_On(int);
void Boozer_Off(void);

#endif
