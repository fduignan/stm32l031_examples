#include <stdint.h>
#include "cortexm0plus.h"
#include "STM32L0x1.h"
void delay(volatile uint32_t dly)
{
    while(dly--);
}

void initClockHSI16()
{
    // Use the HSI16 clock as the system clock - allows operation down to 1.5V
        RCC->CR &= ~BIT24;
        RCC->CR |= BIT0; // turn on HSI16 (16MHz clock)
        while ((RCC->CR & BIT2)==0); // wait for HSI to be ready
        // set HSI16 as system clock source 
        RCC->CFGR |= BIT0;
}
void configPins()
{
    RCC->IOPENR |= BIT2; // enable port C
    GPIOC->MODER &= ~BIT29; // make PC14 an output
    GPIOC->MODER |= BIT28;
}
void initSPI()
{
    
    RCC->IOPENR |= BIT0; // enable port A as SPI1 uses it
    RCC->APB2ENR |= BIT12; // enable SPI1
    // Pin configuration
    // Will use MOSI on pin 13 PA7 and SCK on pin 11 PA5
    GPIOA->MODER &= ~(BIT10 | BIT14); // select alternative function for PA5 and PA7
    GPIOA->MODER |= (BIT11 | BIT15);
    // SPI is alternative mode 0 for these pins
    GPIOA->AFRL &= ~(BIT23 | BIT22 | BIT21 | BIT20 | BIT31 | BIT30 | BIT29 | BIT28);
    // SPI device configuration
    // Baud rate control:
    
    SPI1->CR1 = 0; 
    // software SS management, SSI high, MSB first, divide by 8, 
    // Master mode, CPHA=0 when idle
    SPI1->CR1 |= (BIT9 + BIT8 + BIT4 + BIT2 + BIT0);
    // enable SPI1
    SPI1->CR1 |= BIT6;
    
}
uint8_t transferSPI(uint8_t Data)
{
    SPI1->DR=   Data; // write the data out
    while( (SPI1->SR & BIT1) == 0); // wait for data to go
    if (SPI1->SR & BIT0) // if data was received        
    {
        return SPI1->DR; // return it
    }
    else                // otherwise
    {
        return 0;       // return 0 (no data)
    }
        
}
void initDisplay()
{
    
}
int main()
{
    initClockHSI16();
    configPins();
    initSPI();
    while(1)
    {
        transferSPI(0x2a);
        GPIOC->ODR |= BIT14; // LED on
        delay(100000);
        GPIOC->ODR &= ~BIT14; // LED off
        delay(100000);
       
    }
}
