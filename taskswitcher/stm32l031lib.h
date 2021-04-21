#ifndef __stm32l031lib_h
#define __stm32l031lib_h
#include <stdint.h>
#include <stm32l031xx.h>
#define enable_interrupts() __asm(" cpsie i ")
#define disable_interrupts() __asm(" cpsie d ")
void pinMode(GPIO_TypeDef *Port, uint32_t BitNumber, uint32_t Mode);
void delay(volatile uint32_t dly);
void enablePullUp(GPIO_TypeDef *Port, uint32_t BitNumber);
void initClock(void);
void initSerial(void);
void eputchar(char c);
char egetchar(void);
void eputs(const char *String);
void printDecimal(uint32_t Value);
#endif
