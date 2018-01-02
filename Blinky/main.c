#include <stdint.h>
#include "stm32l031.h"
void delay(volatile uint32_t dly)
{
    while(dly--);
}
void configPins()
{
    RCC_IOPENR |= BIT2; // enable port C
    GPIOC_MODER &= ~BIT29; // make PC14 an output
    GPIOC_MODER |= BIT28;
}
int main()
{
    configPins();
    while(1)
    {
        GPIOC_ODR |= BIT14; // LED on
        delay(100000);
        GPIOC_ODR &= ~BIT14; // LED off
        delay(100000);
        
    }
}
