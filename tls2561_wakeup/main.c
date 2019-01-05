// 
// Test program for tls2561 light sensor - also uses the SSD1306 display for debug purposes.
// The MCU configures the tls2561 to wake it when the light level is above a certain value (0x100)
// The tls2561 interrupt pin is connected to PC15 (mapped to EXTI15).  This pin goes low when the
// tls2561 wishes to interrupt the MCU.

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
void pc15_handler()
{
    EXTI_PR |= 0x8000; // clear EXTI interrupt  

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
uint16_t TLS2561_readDataWord(uint8_t WordNum)
{
    uint16_t result;
    I2CTransaction Transaction;            
    Transaction.Mode = 'r';
    Transaction.SlaveAddress = 0x39;
    Transaction.TXCount = 1;
    Transaction.RXCount = 2;
    Transaction.TXData[0] = 0x80 | WordNum;  
    I2CDoTransaction(&Transaction);  
    result=Transaction.RXData[1];
    result = result << 8;
    result += Transaction.RXData[0];
    return result;
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
    TLS2561_writeReg(0x84,0x00); // Set upper trigger light level
    TLS2561_writeReg(0x85,0x01); // to 0x0100
    TLS2561_writeReg(0x82,0x00); // Set lower trigger light level
    TLS2561_writeReg(0x83,0x00); // to 0x00
    
    TLS2561_writeReg(0x86,0x11); // Turn on light level interrupt system, check levels after each conversion

    
    GPIOC_MODER &= ~(BIT31 + BIT30); // Make PC15 an input
    RCC_APB2ENR |= BIT0; // turn on clock for SYSCFG system
    
    SYSCFG_EXTICR4 &= ~(BIT15+BIT14+BIT13+BIT12); // route PC15 to EXTI system
    SYSCFG_EXTICR4 |= BIT13;
    
    EXTI_FTSR |= BIT15; // enable falling edge trigger for PC15/EXTI15         
    EXTI_IMR |= BIT15; // enable interrupts from EXTI15    
    ISER |= BIT7; // enable interrupts from EXTI15 in the NVIC (see vector table in init.c)
    TLS2561_writeReg(0xc0,0x03); // Clear pending interrupts of the light sensor
    
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
        
        asm("wfi"); // put CPU to sleep by waiting for an interrupt        

        // Having been woken from sleep .... 
        
        GPIOC_ODR |= BIT14; // LED on
        delay(10000);
        GPIOC_ODR &= ~BIT14; // LED off
        delay(10000);          
        // read and display light data values
        int2Hex(&msg[4], TLS2561_readData(0));        
        msg[2]='0';
        writeOLEDText(1,3,msg);
        int2Hex(&msg[4], TLS2561_readData(1));                        
        msg[2]='1';
        writeOLEDText(1,4,msg);                
        TLS2561_writeReg(0xc0,0x03); // Clear pending interrupts of the light sensor
        TLS2561_writeReg(0x86,0x11); // Turn on light level interrupt system, check levels after each conversion

    }
    
}
