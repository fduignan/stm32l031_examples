
// Test program for tls2561 light sensor - also uses the SSD1306 display for debug purposes.
// The program outputs the channel data to the display and also blinks an LED 

#include <stdint.h>
#include "stm32l031.h"
#include "i2c.h"
#include "oled.h"
#include "tls2561.h"
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

void initClockHSI16()
{
    // Use the HSI16 clock as the system clock - allows operation down to 1.5V
    RCC_CR &= ~BIT24;
    RCC_CR |= BIT0; // turn on HSI16 (16MHz clock)
    while ((RCC_CR & BIT2)==0); // wait for HSI to be ready
    // set HSI16 as system clock source 
    RCC_CFGR |= BIT0;
}
int main()
{
    
    int i;
    char msg[20];
    initClockHSI16();
    configPins();
    initI2C();
    enable_interrupts();        
    resetOLED();    
    clearDisplay(); 
    writeOLEDText(5,1,"Light data ");
    TLS2561_writeReg(0x80,0x03); // Turn on the light sensor 
    
    // The output message is a bit messy as the printing routines are line
    // oriented.  Before printing to a particular line on the screen it is 
    // first cleared.  This creates a flicker that is overcome by constructing
    // the output messages manually (didn't want to use library string functions)
    // as shown below.  
    msg[0]='C';
    msg[1]='h';
    msg[2]=' ';
    msg[3]=':';
    while(1)
    {
        GPIOC_ODR |= BIT14; // LED on
        delay(10000);
        GPIOC_ODR &= ~BIT14; // LED off
        delay(10000);          
        int2Hex(&msg[4], TLS2561_readData(0));        
        msg[2]='0';
        writeOLEDText(1,3,msg);
        int2Hex(&msg[4], TLS2561_readData(1));        
        msg[2]='1';
        writeOLEDText(1,4,msg);
    }
    
}
