#ifndef __LPC824_H__
#define __LPC824_H__

#include <stdint.h>

//MRT
#define INTVAL0                        (*((volatile uint32_t*)0x40004000)) //MRT0 time interval value (loaded into the TIMER0 register)
#define TIMER0                         (*((volatile uint32_t*)0x40004004)) //MRT0 timer register (reads the value of the down counter)
#define CTRL0                          (*((volatile uint32_t*)0x40004008)) //MRT0 control register (controls the MRT0 modes)
#define STAT0                          (*((volatile uint32_t*)0x4000400c)) //MRT0 status register
#define INTVAL1                        (*((volatile uint32_t*)0x40004010)) //MRT1 timer interval value (loaded into the TIMER1 register)
#define TIMER1                         (*((volatile uint32_t*)0x40004014)) //MRT1 timer register (reads the value of the down counter)
#define CTRL1                          (*((volatile uint32_t*)0x40004018)) //MRT1 control register (controls the MRT1 modes)
#define STAT1                          (*((volatile uint32_t*)0x4000401c)) //MRT1 status register
#define INTVAL2                        (*((volatile uint32_t*)0x40004020)) //MRT2 timer interval value (loaded into the TIMER2 register)
#define TIMER2                         (*((volatile uint32_t*)0x40004024)) //MRT2 timer register (reads the value of the down counter)
#define CTRL2                          (*((volatile uint32_t*)0x40004028)) //MRT2 control register (controls the MRT2 modes)
#define STAT2                          (*((volatile uint32_t*)0x4000402c)) //MRT2 status register
#define INTVAL3                        (*((volatile uint32_t*)0x40004030)) //MRT3 timer interval value (loaded into the TIMER3 register)
#define TIMER3                         (*((volatile uint32_t*)0x40004034)) //MRT3 timer register (reads the value of the down counter)
#define CTRL3                          (*((volatile uint32_t*)0x40004038)) //MRT3 control register (controls the MRT3 modes)
#define STAT3                          (*((volatile uint32_t*)0x4000403c)) //MRT3 status register
#define IDLE_CH                        (*((volatile uint32_t*)0x400040f4)) //idle channel register (returns the number of the first idle)
#define IRQ_FLAG                       (*((volatile uint32_t*)0x400040f8)) //global interrupt flag register

//ADC
#define ADCCTRL                        (*((volatile uint32_t*)0x4001c000)) //A/D control register
#define ADCSEQA_CTRL                   (*((volatile uint32_t*)0x4001c008)) //A/D conversion sequence-A control register
#define ADCSEQB_CTRL                   (*((volatile uint32_t*)0x4001c00c)) //A/D conversion sequence-B control register
#define ADCSEQA_GDAT                   (*((volatile uint32_t*)0x4001c010)) //A/D sequence-A global data register
#define ADCSEQB_GDAT                   (*((volatile uint32_t*)0x4001c014)) //A/D sequence-B global data register
#define ADCDAT0                        (*((volatile uint32_t*)0x4001c020)) //A/D channel 0 data register
#define ADCDAT1                        (*((volatile uint32_t*)0x4001c024)) //A/D channel 1 data register
#define ADCDAT2                        (*((volatile uint32_t*)0x4001c028)) //A/D channel 2 data register
#define ADCDAT3                        (*((volatile uint32_t*)0x4001c02c)) //A/D channel 3 data register
#define ADCDAT4                        (*((volatile uint32_t*)0x4001c030)) //A/D channel 4 data register
#define ADCDAT5                        (*((volatile uint32_t*)0x4001c034)) //A/D channel 5 data register
#define ADCDAT6                        (*((volatile uint32_t*)0x4001c038)) //A/D channel 6 data register
#define ADCDAT7                        (*((volatile uint32_t*)0x4001c03c)) //A/D channel 7 data register
#define ADCDAT8                        (*((volatile uint32_t*)0x4001c040)) //A/D channel 8 data register
#define ADCDAT9                        (*((volatile uint32_t*)0x4001c044)) //A/D channel 9 data register
#define ADCDAT10                       (*((volatile uint32_t*)0x4001c048)) //A/D channel 10 data register
#define ADCDAT11                       (*((volatile uint32_t*)0x4001c04c)) //A/D channel 11 data register
#define ADCTHR0_LOW                    (*((volatile uint32_t*)0x4001c050)) //A/D low compare threshold register 0
#define ADCTHR1_LOW                    (*((volatile uint32_t*)0x4001c054)) //A/D low compare threshold register 1
#define ADCTHR0_HIGH                   (*((volatile uint32_t*)0x4001c058)) //A/D high compare threshold register 0
#define ADCTHR1_HIGH                   (*((volatile uint32_t*)0x4001c05c)) //A/D high compare threshold register 1
#define ADCCHAN_THRSEL                 (*((volatile uint32_t*)0x4001c060)) //A/D channel-threshold select register
#define ADCINTEN                       (*((volatile uint32_t*)0x4001c064)) //A/D interrupt enable register
#define ADCFLAGS                       (*((volatile uint32_t*)0x4001c068)) //A/D flags register
#define ADCTRM                         (*((volatile uint32_t*)0x4001c06c)) //ADC trim register

//I/O configuration
#define PIO0_17                        (*((volatile uint32_t*)0x40044000)) //I/O configuration for pin PIO0_17/ADC_9
#define PIO0_13                        (*((volatile uint32_t*)0x40044004)) //I/O configuration for pin PIO0_13/ADC_10
#define PIO0_12                        (*((volatile uint32_t*)0x40044008)) //I/O configuration for pin PIO0_12
#define PIO0_5                         (*((volatile uint32_t*)0x4004400c)) //I/O configuration for pin PIO0_5/RESET
#define PIO0_4                         (*((volatile uint32_t*)0x40044010)) //I/O configuration for pin PIO0_4/ADC_11/TRSTN/WAKEUP
#define PIO0_3                         (*((volatile uint32_t*)0x40044014)) //I/O configuration for pin PIO0_3/SWCLK
#define PIO0_2                         (*((volatile uint32_t*)0x40044018)) //I/O configuration for pin PIO0_2/SWDIO
#define PIO0_11                        (*((volatile uint32_t*)0x4004401c)) //I/O configuration for pin PIO0_11 (for the true OD pin)
#define PIO0_10                        (*((volatile uint32_t*)0x40044020)) //I/O configuration for pin PIO0_10 (for the true OD pin)
#define PIO0_16                        (*((volatile uint32_t*)0x40044024)) //I/O configuration for pin PIO0_16
#define PIO0_15                        (*((volatile uint32_t*)0x40044028)) //I/O configuration for pin PIO0_15
#define PIO0_1                         (*((volatile uint32_t*)0x4004402c)) //I/O configuration for pin PIO0_1/ACMP_I1/CLKIN
#define PIO0_9                         (*((volatile uint32_t*)0x40044034)) //I/O configuration for pin PIO0_9/XTALOUT
#define PIO0_8                         (*((volatile uint32_t*)0x40044038)) //I/O configuration for pin PIO0_8/XTALIN
#define PIO0_7                         (*((volatile uint32_t*)0x4004403c)) //I/O configuration for pin PIO0_7/ADC_0
#define PIO0_6                         (*((volatile uint32_t*)0x40044040)) //I/O configuration for pin PIO0_6/ADC_1/VDDCMP
#define PIO0_0                         (*((volatile uint32_t*)0x40044044)) //I/O configuration for pin PIO0_0/ACMP_I0
#define PIO0_14                        (*((volatile uint32_t*)0x40044048)) //I/O configuration for pin PIO0_14/ACMP_I3/ADC_2
#define PIO0_28                        (*((volatile uint32_t*)0x40044050)) //I/O configuration for pin PIO0_28
#define PIO0_27                        (*((volatile uint32_t*)0x40044054)) //I/O configuration for pin PIO0_27
#define PIO0_26                        (*((volatile uint32_t*)0x40044058)) //I/O configuration for pin PIO0_26
#define PIO0_25                        (*((volatile uint32_t*)0x4004405c)) //I/O configuration for pin PIO0_25
#define PIO0_24                        (*((volatile uint32_t*)0x40044060)) //I/O configuration for pin PIO0_24
#define PIO0_23                        (*((volatile uint32_t*)0x40044064)) //I/O configuration for pin PIO0_23/ADC_3/ACMP_I4
#define PIO0_22                        (*((volatile uint32_t*)0x40044068)) //I/O configuration for pin PIO0_22/ADC_4
#define PIO0_21                        (*((volatile uint32_t*)0x4004406c)) //I/O configuration for pin PIO0_21/ADC_5
#define PIO0_20                        (*((volatile uint32_t*)0x40044070)) //I/O configuration for pin PIO0_20/ADC_6
#define PIO0_19                        (*((volatile uint32_t*)0x40044074)) //I/O configuration for pin PIO0_19/ADC_7
#define PIO0_18                        (*((volatile uint32_t*)0x40044078)) //I/O configuration for pin PIO0_18/ADC_8

//system configuration
#define SYSMEMREMAP                    (*((volatile uint32_t*)0x40048000)) //system memory remap
#define PRESETCTRL                     (*((volatile uint32_t*)0x40048004)) //peripheral reset control
#define SYSPLLCTRL                     (*((volatile uint32_t*)0x40048008)) //system pll control
#define SYSPLLSTAT                     (*((volatile uint32_t*)0x4004800c)) //system pll status
#define SYSOSCCTRL                     (*((volatile uint32_t*)0x40048020)) //system oscillator control
#define WDTOSCCTRL                     (*((volatile uint32_t*)0x40048024)) //watchdog oscillator control
#define IRCCTRL                        (*((volatile uint32_t*)0x40048028)) //irc control
#define SYSRSTSTAT                     (*((volatile uint32_t*)0x40048030)) //system reset status
#define SYSPLLCLKSEL                   (*((volatile uint32_t*)0x40048040)) //system pll clock source select
#define SYSPLLCLKUEN                   (*((volatile uint32_t*)0x40048044)) //system pll clock source update enable
#define MAINCLKSEL                     (*((volatile uint32_t*)0x40048070)) //main clock source select
#define MAINCLKUEN                     (*((volatile uint32_t*)0x40048074)) //main clock source update enable
#define SYSAHBCLKDIV                   (*((volatile uint32_t*)0x40048078)) //system clock divider
#define SYSAHBCLKCTRL                  (*((volatile uint32_t*)0x40048080)) //system clock control
#define UARTCLKDIV                     (*((volatile uint32_t*)0x40048094)) //USART clock divider
#define CLKOUTSEL                      (*((volatile uint32_t*)0x400480e0)) //CLKOUT clock source select
#define CLKOUTUEN                      (*((volatile uint32_t*)0x400480e4)) //CLKOUT clock source update enable
#define CLKOUTDIV                      (*((volatile uint32_t*)0x400480e8)) //CLKOUT clock divider
#define UARTFRGDIV                     (*((volatile uint32_t*)0x400480f0)) //USART1 to USART4 common fractional generator divider value
#define UARTFRGMULT                    (*((volatile uint32_t*)0x400480f4)) //USART1 to USART4 common fractional generator multiplier value
#define EXTTRACECMD                    (*((volatile uint32_t*)0x400480fc)) //external trace buffer command
#define PIOPORCAP0                     (*((volatile uint32_t*)0x40048100)) //POR captured PIO status 0
#define IOCONCLKDIV6                   (*((volatile uint32_t*)0x40048134)) //peripheral clock 6 to the IOCON block for programmable glitch
#define IOCONCLKDIV5                   (*((volatile uint32_t*)0x40048138)) //peripheral clock 5 to the IOCON block for programmable glitch
#define IOCONCLKDIV4                   (*((volatile uint32_t*)0x4004813c)) //peripheral clock 4 to the IOCON block for programmable glitch
#define IOCONCLKDIV3                   (*((volatile uint32_t*)0x40048140)) //peripheral clock 3 to the IOCON block for programmable glitch
#define IOCONCLKDIV2                   (*((volatile uint32_t*)0x40048144)) //peripheral clock 2 to the IOCON block for programmable glitch
#define IOCONCLKDIV1                   (*((volatile uint32_t*)0x40048148)) //peripheral clock 1 to the IOCON block for programmable glitch
#define IOCONCLKDIV0                   (*((volatile uint32_t*)0x4004814c)) //peripheral clock 0 to the IOCON block for programmable glitch
#define BODCTRL                        (*((volatile uint32_t*)0x40048150)) //brown-out detect
#define SYSTCKCAL                      (*((volatile uint32_t*)0x40048154)) //system tick counter calibration
#define IRQLATENCY                     (*((volatile uint32_t*)0x40048170)) //IRQ delay; allows trade-off between interrupt latency and
#define NMISRC                         (*((volatile uint32_t*)0x40048174)) //NMI source control
#define PINTSEL0                       (*((volatile uint32_t*)0x40048178)) //GPIO pin interrupt select register 0
#define PINTSEL1                       (*((volatile uint32_t*)0x4004817c)) //GPIO pin interrupt select register 1
#define PINTSEL2                       (*((volatile uint32_t*)0x40048180)) //GPIO pin interrupt select register 2
#define PINTSEL3                       (*((volatile uint32_t*)0x40048184)) //GPIO pin interrupt select register 3
#define PINTSEL4                       (*((volatile uint32_t*)0x40048188)) //GPIO pin interrupt select register 4
#define PINTSEL5                       (*((volatile uint32_t*)0x4004818c)) //GPIO pin interrupt select register 5
#define PINTSEL6                       (*((volatile uint32_t*)0x40048190)) //GPIO pin interrupt select register 6
#define PINTSEL7                       (*((volatile uint32_t*)0x40048194)) //GPIO pin interrupt select register 7
#define STARTERP0                      (*((volatile uint32_t*)0x40048204)) //start logic 0 pin wake-up enable
#define STARTERP1                      (*((volatile uint32_t*)0x40048214)) //start logic 1 interrupt wake-up enable
#define PDSLEEPCFG                     (*((volatile uint32_t*)0x40048230)) //power-down states in deep-sleep mode
#define PDAWAKECFG                     (*((volatile uint32_t*)0x40048234)) //power-down states for wake-up from deep-sleep
#define PDRUNCFG                       (*((volatile uint32_t*)0x40048238)) //power configuration register
#define DEVICE_ID                      (*((volatile uint32_t*)0x400483f8)) //device id

//switch matrix
#define PINASSIGN0                     (*((volatile uint32_t*)0x4000c000)) //pin assign register 0
#define PINASSIGN1                     (*((volatile uint32_t*)0x4000c004)) //pin assign register 1
#define PINASSIGN2                     (*((volatile uint32_t*)0x4000c008)) //pin assign register 2
#define PINASSIGN3                     (*((volatile uint32_t*)0x4000c00c)) //pin assign register 3
#define PINASSIGN4                     (*((volatile uint32_t*)0x4000c010)) //pin assign register 4
#define PINASSIGN5                     (*((volatile uint32_t*)0x4000c014)) //pin assign register 5
#define PINASSIGN6                     (*((volatile uint32_t*)0x4000c018)) //pin assign register 6
#define PINASSIGN7                     (*((volatile uint32_t*)0x4000c01c)) //pin assign register 7
#define PINASSIGN8                     (*((volatile uint32_t*)0x4000c020)) //pin assign register 8
#define PINASSIGN9                     (*((volatile uint32_t*)0x4000c024)) //pin assign register 9
#define PINASSIGN10                    (*((volatile uint32_t*)0x4000c028)) //pin assign register 10
#define PINASSIGN11                    (*((volatile uint32_t*)0x4000c02c)) //pin assign register 11
#define PINENABLE0                     (*((volatile uint32_t*)0x4000c1c0)) //pin enable register

//PMU
#define PCON                           (*((volatile uint32_t*)0x40020000)) //power control register
#define GPREG0                         (*((volatile uint32_t*)0x40020004)) //general purpose register 0
#define GPREG1                         (*((volatile uint32_t*)0x40020008)) //general purpose register 1
#define GPREG2                         (*((volatile uint32_t*)0x4002000c)) //general purpose register 2
#define GPREG3                         (*((volatile uint32_t*)0x40020010)) //general purpose register 3
#define DPDCTRL                        (*((volatile uint32_t*)0x40020014)) //deep power-down control register

//I2C
struct I2C_Registers {
   volatile uint32_t CFG,              //configuration for shared functions
                     STAT,             //status register for master, slave, and monitor functions
                     INTENSET,         //interrupt enable set and read register
                     INTENCLR,         //interrupt enable clear register
                     TIMEOUT,          //time-out value register
                     CLKDIV,           //clock pre-divider for the entire I2C block
                     INTSTAT,          //interrupt status register for master, slave, and monitor functions
                     reserved_1,
                     MSTCTL,           //master control register
                     MSTTIME,          //master timing configuration
                     MSTDAT,           //combined master receiver and transmitter data register
                     reserved_2[5],
                     SLVCTL,           //slave control register
                     SLVDAT,           //combined slave receiver and transmitter data register
                     SLVADR0,          //slave address 0
                     SLVADR1,          //slave address 1
                     SLVADR2,          //slave address 2
                     SLVADR3,          //slave address 3
                     SLVQUAL0,         //slave qualification for address 0
                     reserved_3[9],
                     MONRXDAT;         //monitor receiver data register
};

#define I2C0 ((struct I2C_Registers*)(0x40050000))
#define I2C1 ((struct I2C_Registers*)(0x40054000))
#define I2C2 ((struct I2C_Registers*)(0x40070000))
#define I2C3 ((struct I2C_Registers*)(0x40074000))

//SPI
struct SPI_Registers {
   volatile uint32_t CFG,              //configuration register
                     DLY,              //delay register
                     STAT,             //status; some status flags can be cleared by writing a 1 to that bit position
                     INTENSET,         //interrupt enable read and set
                     INTENCLR,         //interrupt enable clear
                     RXDAT,            //receive data
                     TXDATCTL,         //transmit data with control
                     TXDAT,            //transmit data
                     TXCTL,            //transmit control
                     DIV,              //clock divider
                     INSTSTAT;         //interrupt status
};

#define SPI0 ((struct SPI_Registers*)(0x40058000))
#define SPI1 ((struct SPI_Registers*)(0x4005c000))

//USART0
#define USART0CFG                      (*((volatile uint32_t*)0x40064000)) //USART configuration register
#define USART0CTL                      (*((volatile uint32_t*)0x40064004)) //USART control register
#define USART0STAT                     (*((volatile uint32_t*)0x40064008)) //USART status register
#define USART0INTENSET                 (*((volatile uint32_t*)0x4006400c)) //interrupt enable read and set register
#define USART0INTENCLR                 (*((volatile uint32_t*)0x40064010)) //interrupt enable clear register
#define USART0RXDAT                    (*((volatile uint32_t*)0x40064014)) //receiver data register
#define USART0RXDATSTAT                (*((volatile uint32_t*)0x40064018)) //receiver data with status register
#define USART0TXDAT                    (*((volatile uint32_t*)0x4006401c)) //transmit data register
#define USART0BRG                      (*((volatile uint32_t*)0x40064020)) //baud rate generator register
#define USART0INTSTAT                  (*((volatile uint32_t*)0x40064024)) //interrupt status register
#define USART0OSR                      (*((volatile uint32_t*)0x40064028)) //oversample selection register for asynchronous communication
#define USART0ADDR                     (*((volatile uint32_t*)0x4006402c)) //address register for automatic address matching

//USART1
#define USART1CFG                      (*((volatile uint32_t*)0x40068000)) //USART configuration register
#define USART1CTL                      (*((volatile uint32_t*)0x40068004)) //USART control register
#define USART1STAT                     (*((volatile uint32_t*)0x40068008)) //USART status register
#define USART1INTENSET                 (*((volatile uint32_t*)0x4006800c)) //interrupt enable read and set register
#define USART1INTENCLR                 (*((volatile uint32_t*)0x40068010)) //interrupt enable clear register
#define USART1RXDAT                    (*((volatile uint32_t*)0x40068014)) //receiver data register
#define USART1RXDATSTAT                (*((volatile uint32_t*)0x40068018)) //receiver data with status register
#define USART1TXDAT                    (*((volatile uint32_t*)0x4006801c)) //transmit data register
#define USART1BRG                      (*((volatile uint32_t*)0x40068020)) //baud rate generator register
#define USART1INTSTAT                  (*((volatile uint32_t*)0x40068024)) //interrupt status register
#define USART1OSR                      (*((volatile uint32_t*)0x40068028)) //oversample selection register for asynchronous communication
#define USART1ADDR                     (*((volatile uint32_t*)0x4006802c)) //address register for automatic address matching

//USART2
#define USART2CFG                      (*((volatile uint32_t*)0x4006c000)) //USART configuration register
#define USART2CTL                      (*((volatile uint32_t*)0x4006c004)) //USART control register
#define USART2STAT                     (*((volatile uint32_t*)0x4006c008)) //USART status register
#define USART2INTENSET                 (*((volatile uint32_t*)0x4006c00c)) //interrupt enable read and set register
#define USART2INTENCLR                 (*((volatile uint32_t*)0x4006c010)) //interrupt enable clear register
#define USART2RXDAT                    (*((volatile uint32_t*)0x4006c014)) //receiver data register
#define USART2RXDATSTAT                (*((volatile uint32_t*)0x4006c018)) //receiver data with status register
#define USART2TXDAT                    (*((volatile uint32_t*)0x4006c01c)) //transmit data register
#define USART2BRG                      (*((volatile uint32_t*)0x4006c020)) //baud rate generator register
#define USART2INTSTAT                  (*((volatile uint32_t*)0x4006c024)) //interrupt status register
#define USART2OSR                      (*((volatile uint32_t*)0x4006c028)) //oversample selection register for asynchronous communication
#define USART2ADDR                     (*((volatile uint32_t*)0x4006c02c)) //address register for automatic address matching

//GPIO port
#define B0                             (*((volatile uint8_t*)0xa0000000)) //byte pin registers port 0; pins PIO0_0 to PIO0_28
#define B1                             (*((volatile uint8_t*)0xa0000001))
#define B2                             (*((volatile uint8_t*)0xa0000002))
#define B3                             (*((volatile uint8_t*)0xa0000003))
#define B4                             (*((volatile uint8_t*)0xa0000004))
#define B5                             (*((volatile uint8_t*)0xa0000005))
#define B6                             (*((volatile uint8_t*)0xa0000006))
#define B7                             (*((volatile uint8_t*)0xa0000007))
#define B8                             (*((volatile uint8_t*)0xa0000008))
#define B9                             (*((volatile uint8_t*)0xa0000009))
#define B10                            (*((volatile uint8_t*)0xa000000a))
#define B11                            (*((volatile uint8_t*)0xa000000b))
#define B12                            (*((volatile uint8_t*)0xa000000c))
#define B13                            (*((volatile uint8_t*)0xa000000d))
#define B14                            (*((volatile uint8_t*)0xa000000e))
#define B15                            (*((volatile uint8_t*)0xa000000f))
#define B16                            (*((volatile uint8_t*)0xa0000010))
#define B17                            (*((volatile uint8_t*)0xa0000011))
#define B18                            (*((volatile uint8_t*)0xa0000012))
#define B19                            (*((volatile uint8_t*)0xa0000013))
#define B20                            (*((volatile uint8_t*)0xa0000014))
#define B21                            (*((volatile uint8_t*)0xa0000015))
#define B22                            (*((volatile uint8_t*)0xa0000016))
#define B23                            (*((volatile uint8_t*)0xa0000017))
#define B24                            (*((volatile uint8_t*)0xa0000018))
#define B25                            (*((volatile uint8_t*)0xa0000019))
#define B26                            (*((volatile uint8_t*)0xa000001a))
#define B27                            (*((volatile uint8_t*)0xa000001b))
#define B28                            (*((volatile uint8_t*)0xa000001c))
#define W0                             (*((volatile uint32_t*)0xa0001000)) //word pin registers port 0
#define W1                             (*((volatile uint32_t*)0xa0001004))
#define W2                             (*((volatile uint32_t*)0xa0001008))
#define W3                             (*((volatile uint32_t*)0xa000100c))
#define W4                             (*((volatile uint32_t*)0xa0001010))
#define W5                             (*((volatile uint32_t*)0xa0001014))
#define W6                             (*((volatile uint32_t*)0xa0001018))
#define W7                             (*((volatile uint32_t*)0xa000101c))
#define W8                             (*((volatile uint32_t*)0xa0001020))
#define W9                             (*((volatile uint32_t*)0xa0001024))
#define W10                            (*((volatile uint32_t*)0xa0001028))
#define W11                            (*((volatile uint32_t*)0xa000102c))
#define W12                            (*((volatile uint32_t*)0xa0001030))
#define W13                            (*((volatile uint32_t*)0xa0001034))
#define W14                            (*((volatile uint32_t*)0xa0001038))
#define W15                            (*((volatile uint32_t*)0xa000103c))
#define W16                            (*((volatile uint32_t*)0xa0001040))
#define W17                            (*((volatile uint32_t*)0xa0001044))
#define W18                            (*((volatile uint32_t*)0xa0001048))
#define W19                            (*((volatile uint32_t*)0xa000104c))
#define W20                            (*((volatile uint32_t*)0xa0001050))
#define W21                            (*((volatile uint32_t*)0xa0001054))
#define W22                            (*((volatile uint32_t*)0xa0001058))
#define W23                            (*((volatile uint32_t*)0xa000105c))
#define W24                            (*((volatile uint32_t*)0xa0001060))
#define W25                            (*((volatile uint32_t*)0xa0001064))
#define W26                            (*((volatile uint32_t*)0xa0001068))
#define W27                            (*((volatile uint32_t*)0xa000106c))
#define W28                            (*((volatile uint32_t*)0xa0001070))
#define DIR0                           (*((volatile uint32_t*)0xa0002000)) //direction registers port 0
#define MASK0                          (*((volatile uint32_t*)0xa0002080)) //mask register port 0
#define PIN0                           (*((volatile uint32_t*)0xa0002100)) //port pin register port 0
#define MPIN0                          (*((volatile uint32_t*)0xa0002180)) //masked port register port 0
#define SET0                           (*((volatile uint32_t*)0xa0002200)) //write: set register for port 0; read: output bits for port 0
#define CLR0                           (*((volatile uint32_t*)0xa0002280)) //clear port 0
#define NOT0                           (*((volatile uint32_t*)0xa0002300)) //toggle port 0
#define DIRSET0                        (*((volatile uint32_t*)0xa0002380)) //set pin direction bits for port 0
#define DIRCLR0                        (*((volatile uint32_t*)0xa0002400)) //clear pin direction bits for port 0
#define DIRNOT0                        (*((volatile uint32_t*)0xa0002480)) //toggle pin direction bits for port 0

//pin interrupts and pattern match engine
#define ISEL                           (*((volatile uint32_t*)0xa0004000)) //pin interrupt mode register
#define IENR                           (*((volatile uint32_t*)0xa0004004)) //pin interrupt level or rising edge interrupt enable register
#define SIENR                          (*((volatile uint32_t*)0xa0004008)) //pin interrupt level or rising edge interrupt set register
#define CIENR                          (*((volatile uint32_t*)0xa000400c)) //pin interrupt level (rising edge interrupt) clear register
#define IENF                           (*((volatile uint32_t*)0xa0004010)) //pin interrupt active level or falling edge interrupt enable register
#define SIENF                          (*((volatile uint32_t*)0xa0004014)) //pin interrupt active level or falling edge interrupt set register
#define CIENF                          (*((volatile uint32_t*)0xa0004018)) //pin interrupt active level or falling edge interrupt clear register
#define RISE                           (*((volatile uint32_t*)0xa000401C)) //pin interrupt rising edge register
#define FALL                           (*((volatile uint32_t*)0xa0004020)) //pin interrupt falling edge register
#define IST                            (*((volatile uint32_t*)0xa0004024)) //pin interrupt status register
#define PMCTRL                         (*((volatile uint32_t*)0xa0004028)) //pattern match interrupt control register
#define PMSRC                          (*((volatile uint32_t*)0xa000402C)) //pattern match interrupt bit-slice source register
#define PMCFG                          (*((volatile uint32_t*)0xa0004030)) //pattern match interrupt bit slice configuration register

//SysTick timer
#define SYST_CSR                       (*((volatile uint32_t*)0xe000e010)) //system timer control and status register
#define SYST_RVR                       (*((volatile uint32_t*)0xe000e014)) //system timer reload value register
#define SYST_CVR                       (*((volatile uint32_t*)0xe000e018)) //system timer current value register
#define SYST_CALIB                     (*((volatile uint32_t*)0xe000e01c)) //system timer calibration value register

//NVIC
#define ISER0                          (*((volatile uint32_t*)0xe000e100)) //interrupt set enable
#define ICER0                          (*((volatile uint32_t*)0xe000e180)) //interrupt clear enable
#define ISPR0                          (*((volatile uint32_t*)0xe000e200)) //interrupt set pending
#define ICPR0                          (*((volatile uint32_t*)0xe000e280)) //interrupt clear pending
#define IABR0                          (*((volatile uint32_t*)0xe000e300)) //interrupt active bit
#define IPR0                           (*((volatile uint32_t*)0xe000e400)) //interrupt priority registers 0
#define IPR1                           (*((volatile uint32_t*)0xe000e404)) //interrupt priority registers 1
#define IPR2                           (*((volatile uint32_t*)0xe000e408)) //interrupt priority registers 2
#define IPR3                           (*((volatile uint32_t*)0xe000e40c)) //interrupt priority registers 3
#define IPR4                           (*((volatile uint32_t*)0xe000e410)) //interrupt priority registers 4
#define IPR5                           (*((volatile uint32_t*)0xe000e414)) //interrupt priority registers 5
#define IPR6                           (*((volatile uint32_t*)0xe000e418)) //interrupt priority registers 6
#define IPR7                           (*((volatile uint32_t*)0xe000e41c)) //interrupt priority registers 7

//SCB (system control block)
#define CPUID                          (*((volatile uint32_t*)0xe000ed00)) //CPUID register
#define ICSR                           (*((volatile uint32_t*)0xe000ed04)) //interrupt control and state register
#define VTOR                           (*((volatile uint32_t*)0xe000ed08)) //vector table offset register
#define AIRCR                          (*((volatile uint32_t*)0xe000ed0c)) //application interrupt and reset control register
#define SCR                            (*((volatile uint32_t*)0xe000ed10)) //system control register
#define CCR                            (*((volatile uint32_t*)0xe000ed14)) //configuration and control register
#define SHPR2                          (*((volatile uint32_t*)0xe000ed1c)) //system handler priority register 2
#define SHPR3                          (*((volatile uint32_t*)0xe000ed20)) //system handler priority register 3

#endif
