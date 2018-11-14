.syntax unified
.cpu cortex-m0plus
.thumb

.text

.global _disable_irq
.global _enable_irq
.global _start_critical
.global _end_critical
.global _wfi
.global _get_psr
.global _dsb
.global _get_msp

.thumb_func
_disable_irq:
cpsid i
bx lr

.thumb_func
_enable_irq:
cpsie i
bx lr

.thumb_func
_start_critical:
mrs r0,primask
cpsid i
bx lr

.thumb_func
_end_critical:
msr primask,r0
bx lr

.thumb_func
_wfi:
wfi
bx lr

.thumb_func
_get_psr:
mrs r0,psr
bx lr

.thumb_func
_dsb:
dsb sy
bx lr

.thumb_func
_get_msp:
mrs r0,msp
bx lr

.end
