#include "STM32L0x3.h"
#include "display.h"
timer T;
display Display;
void delay(volatile uint32_t dly)
{
    // simple software delay
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
void setup()
{
    initClockHSI16();
    
    RCC->IOPENR |= BIT2; // Turn on port C
    GPIOC->MODER |= BIT28; // Make bit 14 an output
    GPIOC->MODER &= ~BIT29;
    T.begin();
    Display.begin(&T);
}
int main()
{
    uint16_t Count=0;
    setup();
    Display.drawRectangle(0, 0 , 239, 134, RGBToWord(0xff,0xff,0xff));    
    Display.print(DISP_STR("Hello World"),10,10,RGBToWord(0xff,0xff,0xff),0);    
    Display.drawRectangle(50,50,10,10,RGBToWord(0xff,0,0));
    Display.fillRectangle(70,50,10,10,RGBToWord(0xf,0xf,0xff));
    Display.drawCircle(100, 70, 10 , RGBToWord(0xff,0xff,0));
    Display.fillCircle(130, 70, 10 , RGBToWord(0,0xff,0));
    while(1)
    {       
        Display.print(Count++,10,25,RGBToWord(0,0xff,0xff),0);
    }
    return 0;
}
