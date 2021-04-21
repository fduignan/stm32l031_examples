#include "stm32l031lib.h"
#include "thread.h"


#define STACK_SIZE 128
__attribute__((noreturn))  void threadA(void);
__attribute__((noreturn)) void threadB(void);
__attribute__((noreturn)) void threadC(void);
static uint32_t StackA[STACK_SIZE];
static uint32_t StackB[STACK_SIZE];
static uint32_t StackC[STACK_SIZE];

void threadA()
{
	while (1)
	{
		static int State = 0;
		if (State == 0)
		{
			GPIOA->ODR &=~(1u << 0);
			State = 1;
		}
		else
		{
			GPIOA->ODR |= (1 << 0);
			State = 0;
		}
		delay(100000);
	}	
}
void threadB()
{
	while(1)
	{
		static int State = 0;
		if (State == 0)
		{
			GPIOA->ODR &=~(1u << 1);
			State = 1;
		}
		else
		{
			GPIOA->ODR |= (1 << 1);
			State = 0;
		}
		delay(200000);
	}
}
void threadC()
{
	while(1)
	{
		static int State = 0;
		if (State == 0)
		{
			GPIOA->ODR &=~(1u << 2);
			State = 1;
		}
		else
		{
			GPIOA->ODR |= (1 << 2);
			State = 0;
		}
		delay(300000);
	}
}
int main()
{
	// Initialize I/O ports
	RCC->IOPENR = 1; // Enable GPIOA
	pinMode(GPIOA,0,1); // Make GPIOA Bit 0 an output
	pinMode(GPIOA,1,1); // Make GPIOA Bit 1 an output
	pinMode(GPIOA,2,1); // Make GPIOA Bit 2 an output
	initClock();		// Set the MCU running at 16MHz
	createThread(threadA,StackA, STACK_SIZE);
	createThread(threadB,StackB, STACK_SIZE);
	createThread(threadC,StackC, STACK_SIZE);
	startSwitcher();
}

