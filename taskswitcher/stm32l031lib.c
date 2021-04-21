#include <stdint.h>
#include <stm32l031xx.h>
#include "stm32l031lib.h"

void pinMode(GPIO_TypeDef *Port, uint32_t BitNumber, uint32_t Mode)
{
	// This function writes the given Mode bits to the appropriate location for
	// the given BitNumber in the Port specified.  It leaves other bits unchanged
	// Mode values:
	// 0 : digital input
	// 1 : digital output
	// 2 : Alternative function
	// 3 : Analog input
	uint32_t mode_value = Port->MODER; // read current value of Mode register 
	Mode = Mode << (2 * BitNumber);    // There are two Mode bits per port bit so need to shift
																	   // the mask for Mode up to the proper location
	mode_value = mode_value & ~(3u << (BitNumber * 2)); // Clear out old mode bits
	mode_value = mode_value | Mode; // set new bits
	Port->MODER = mode_value; // write back to port mode register
}
void delay(volatile uint32_t dly)
{
	while(dly--);
}
void enablePullUp(GPIO_TypeDef *Port, uint32_t BitNumber)
{
		Port->PUPDR = Port->PUPDR & ~(3u << BitNumber*2); // clear pull-up bits
		Port->PUPDR = Port->PUPDR | (1u << BitNumber*2); //  set pull-up bit
}
void initClock()
{
	/* This function switches the STM32L031's clock to its 16MHz MSI clock */
	RCC->CR &= (1u << 24);
	RCC->CR |= 1; // Turn on HSI16
	while ( (RCC->CR & (1 <<2)) == 0); // wait for HSI to be ready
	// set HSI16 as the system clock source
	RCC->CFGR |= 1;
}

void initSerial()
{
	/* On the nucleo board, TX is on PA2 while RX is on PA15 */
	initClock();
	RCC->IOPENR |= 1; // enable GPIOA
	RCC->APB1ENR |= (1 << 17); // enable USART2
	pinMode(GPIOA,2,2); // enable alternate function on PA2
	pinMode(GPIOA,15,2); // enable alternate function on PA15
	// AF4 = USART2 TX on PA2
	GPIOA->AFR[0] &= 0xfffff0ff;
	GPIOA->AFR[0] |= (4 << 8);
	// AF4 = USART2 RX on PA2
	GPIOA->AFR[1] &= 0x0fffffff;
	GPIOA->AFR[1] |= (4 << 28);
	USART2->CR1 = 0; // disable before configuration
	USART2->CR3 |= (1 << 12); // disable overrun detection
	USART2->BRR = 16000000/9600; // assuming 16MHz clock and 9600 bps data rate
	USART2->CR1 |= (1 << 2) + (1 << 3); // enable Transmistter and receiver
	USART2->CR1 |= 1; // enable the UART

}
void eputchar(char c)
{
	while( (USART2->ISR & (1 << 6)) == 0); // wait for any ongoing
	// transmission to finish
	USART2->TDR = c;
}
char egetchar()
{
	while( (USART2->ISR & (1 << 5)) == 0); // wait for a character
	return (char)USART2->RDR;
}
void eputs(const char *String)
{
	while(*String) // keep printing until a NULL is found
	{
		eputchar(*String);
		String++;
	}
}
void printDecimal(uint32_t Value)
{
	char DecimalString[11]; // a 32 bit value can go up
												// to about 4billion:
												// That's 10 digits
												// plus a null character
	DecimalString[10] = 0; // terminate the string;
	int index = 9;
	while(index >= 0)
	{
		DecimalString[index]=(Value % 10) + '0';
		Value = Value / 10;
		index--;
	}
	eputs(DecimalString);
}

