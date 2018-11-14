.syntax unified
.cpu cortex-m0plus
.thumb

.global Reset_Handler

.weak NMI_Handler,HardFault_Handler,SVC_Handler,PendSV_Handler,SysTick_Handler,SPI0_IRQHandler,SPI1_IRQHandler,UART0_IRQHandler,UART1_IRQHandler,UART2_IRQHandler,I2C1_IRQHandler,I2C0_IRQHandler,SCT_IRQHandler,MRT_IRQHandler,CMP_IRQHandler,WDT_IRQHandler,BOD_IRQHandler,FLASH_IRQHandler,WKT_IRQHandler,ADC_SEQA_IRQHandler,ADC_SEQB_IRQHandler,ADC_THCMP_IRQHandler,ADC_OVR_IRQHandler,DMA_IRQHandler,I2C2_IRQHandler,I2C3_IRQHandler,PININT0_IRQHandler,PININT1_IRQHandler,PININT2_IRQHandler,PININT3_IRQHandler,PININT4_IRQHandler,PININT5_IRQHandler,PININT6_IRQHandler,PININT7_IRQHandler

.section .intvecs, "a"
.word _stack_end
.word Reset_Handler
.word NMI_Handler
.word HardFault_Handler		@ irq -13, exception 3, offset 0x0c
.word 0
.word 0
.word 0
.word __checksum
.word 0
.word 0
.word 0
.word SVC_Handler
.word 0
.word 0
.word PendSV_Handler
.word SysTick_Handler
@ vendor specific interrupts
.word SPI0_IRQHandler		@ irq 0, exception 16, offset 0x40
.word SPI1_IRQHandler
.word 0
.word UART0_IRQHandler
.word UART1_IRQHandler
.word UART2_IRQHandler
.word 0
.word I2C1_IRQHandler
.word I2C0_IRQHandler
.word SCT_IRQHandler
.word MRT_IRQHandler
.word CMP_IRQHandler
.word WDT_IRQHandler
.word BOD_IRQHandler
.word FLASH_IRQHandler
.word WKT_IRQHandler
.word ADC_SEQA_IRQHandler
.word ADC_SEQB_IRQHandler
.word ADC_THCMP_IRQHandler
.word ADC_OVR_IRQHandler
.word DMA_IRQHandler
.word I2C2_IRQHandler
.word I2C3_IRQHandler
.word 0
.word PININT0_IRQHandler
.word PININT1_IRQHandler
.word PININT2_IRQHandler
.word PININT3_IRQHandler
.word PININT4_IRQHandler
.word PININT5_IRQHandler
.word PININT6_IRQHandler
.word PININT7_IRQHandler

.text
.thumb_func
.type Reset_Handler, %function
Reset_Handler:
bl init
bl main
b .

.thumb_func
NMI_Handler:
.thumb_func
HardFault_Handler:
.thumb_func
SVC_Handler:
.thumb_func
PendSV_Handler:
.thumb_func
SysTick_Handler:
.thumb_func
SPI0_IRQHandler:
.thumb_func
SPI1_IRQHandler:
.thumb_func
UART0_IRQHandler:
.thumb_func
UART1_IRQHandler:
.thumb_func
UART2_IRQHandler:
.thumb_func
I2C1_IRQHandler:
.thumb_func
I2C0_IRQHandler:
.thumb_func
SCT_IRQHandler:
.thumb_func
MRT_IRQHandler:
.thumb_func
CMP_IRQHandler:
.thumb_func
WDT_IRQHandler:
.thumb_func
BOD_IRQHandler:
.thumb_func
FLASH_IRQHandler:
.thumb_func
WKT_IRQHandler:
.thumb_func
ADC_SEQA_IRQHandler:
.thumb_func
ADC_SEQB_IRQHandler:
.thumb_func
ADC_THCMP_IRQHandler:
.thumb_func
ADC_OVR_IRQHandler:
.thumb_func
DMA_IRQHandler:
.thumb_func
I2C2_IRQHandler:
.thumb_func
I2C3_IRQHandler:
.thumb_func
PININT0_IRQHandler:
.thumb_func
PININT1_IRQHandler:
.thumb_func
PININT2_IRQHandler:
.thumb_func
PININT3_IRQHandler:
.thumb_func
PININT4_IRQHandler:
.thumb_func
PININT5_IRQHandler:
.thumb_func
PININT6_IRQHandler:
.thumb_func
PININT7_IRQHandler:
b .

.end
