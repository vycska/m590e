#ifndef __ONEWIRE_H__
#define __ONEWIRE_H__

void onewire_init(void);
void onewire_drivelinelow(void);
void onewire_releaseline(void);
unsigned char onewire_getlinevalue(void);
void onewire_delay(int);
int onewire_reset(void);
unsigned char onewire_readbit(void);
void onewire_writebit(unsigned char);
unsigned char onewire_receivebyte(void);
void onewire_sendbyte(unsigned char);
unsigned char onewire_crc8(unsigned char *, int);

#endif
