#ifndef __UTILS_ASM_H__
#define __UTILS_ASM_H__

#include <stdint.h>

void _disable_irq(void);
void _enable_irq(void);
uint32_t _start_critical(void);
void _end_critical(uint32_t);
void _wfi(void);
uint32_t _get_psr(void);
void _dsb(void);
unsigned int _get_msp(void);

#endif
