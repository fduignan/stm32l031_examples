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
    RCC->IOPENR |= BIT1; // enable port B
    GPIOB->MODER &= ~BIT7; // make PC14 an output
    GPIOB->MODER |= BIT6;
}
int X;
void initSPI()
{
    
    RCC->IOPENR |= BIT1; // enable port B as SPI1 uses it (necessary?)
    RCC->APB2ENR |= BIT12; // enable SPI1
    // Pin configuration
    // on the STM32L031 Nucleo board MOSI is on PB5, 
    // MISO is on PB4 and SCK is on PB3.  The PL9823 only needs MOSI
    GPIOB->MODER &= ~(BIT10); // select alternative function for PB5
    GPIOB->MODER |= (BIT11);
    // SPI is alternative mode 0 for this pin
    GPIOB->AFRL &= ~(BIT23 + BIT22 + BIT21 + BIT20);
    // SPI device configuration
    // Baud rate control:
    /* The PL9823 data frame format : 
     * Logic 0 : High pulse for 0.35us, low pulse for 1.36us
     * Logic 1 : High pulse for 1.36us, low pulse for 0.35us
     * Reset : low pulse for 50us
     * All timings +/- 150ns
     * Range of timings:
     * Short pulse : 0.2 to 0.5 us
     * Long pulse  : 1.21 to 1.51us
     * If the SPI interface is run at 2MHz then 1 short
     * pulse should (just about) map to 1 clock cycle (0.5us)
     * One long pulse should map to 3 clock cycles (1.5us)
     * This is barely within spec but there are is limited set 
     * of clocks available in the STM32L031.  So, if 16MHz 
     * is used APB2 clock, the divisor for the SPI interface should
     * be 8.
     */
    SPI1->CR1 = 0; 
    // software SS management, SSI high, MSB first, divide by 8, 
    // Master mode, CPHA=0 when idle
    SPI1->CR1 |= (BIT9 + BIT8 + BIT4 + BIT2 + BIT0);
    X = SPI1->CR1;
    // enable SPI1
    SPI1->CR1 |= BIT6;
    X = SPI1->CR1;
    
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
void writePL9823(uint32_t Colour)
{
    // each colour bit should map to 4 SPI bits.
    // Format of Colour (bytes) 00RRGGBB
    uint8_t SPI_Output[12];
    int SrcIndex = 0;
    int DestIndex = 0;
    for (DestIndex = 0; DestIndex < 12; DestIndex++)
    {
        if (Colour & (1 << 23))
        {
            SPI_Output[DestIndex] = 0xe0;
        }
        else
        {
            SPI_Output[DestIndex] = 0x80;
        }
        Colour = Colour << 1;
        if (Colour & (1 << 23))
        {
            SPI_Output[DestIndex] |= 0xe;
        }
        else
        {
            SPI_Output[DestIndex] |= 0x8;
        }
        Colour = Colour << 1;
    }
    for (int i=0;i<12;i++)
    {
        transferSPI(SPI_Output[i]);
    }
}
unsigned long getRainbow()
{   // Cycle through the colours of the rainbow (non-uniform brightness however)
    // Inspired by : http://academe.co.uk/2012/04/arduino-cycling-through-colours-of-the-rainbow/
    static unsigned Red = 0;
    static unsigned Green = 0;
    static unsigned Blue = 0;
    static int State = 0;
    switch (State)
    {
        case 0:{
            Green++;
            if (Green == 255)
                State = 1;
            break;
        }
        case 1:{
            Red++;
            if (Red == 255)
                State = 2;
            break;
        }
        case 2:{
            Blue++;
            if (Blue == 255)
                State = 3;          
            break;
        }
        case 3:{
            Green--;
            if (Green == 0)
                State = 4;
            break;
        }
        case 4:{
            Red--;
            if (Red == 0)
                State = 5;
            break;
        }
        case 5:{
            Blue --;
            if (Blue == 0)
                State = 0;
            break;
        }       
    }
    return (Green << 16) + (Red << 8) + Blue;
}
int main()
{
    initClockHSI16();
    configPins();
    initSPI();
    while(1)
    {
        writePL9823(getRainbow());  
        GPIOB->ODR |= BIT3; // LED on
        delay(100000);
        GPIOB->ODR &= ~BIT3; // LED off
        delay(100000);
       
    }
}
