#include <stdint.h>
#include "thread.h"
#include "stm32l031lib.h"

uint32_t ThreadIndex = 0;
uint32_t ThreadCount = 0;
ThreadControlBlock Threads[MAX_THREADS];
uint32_t TCB_Size = sizeof(ThreadControlBlock);

void createThread(void (*ThreadFn )(void), uint32_t *ThreadStack, uint32_t StackSize)
{
    uint32_t Index;
    if (ThreadCount < MAX_THREADS)
    {
        __asm(" cpsid i "); // disable interrupts during thread creation
        Threads[ThreadCount].ThreadFn = ThreadFn;
        
        // The thread stack needs to be set up so that it can be context switched back to later
        // First zero the stack (assuming 32 bit accesses.  Stacksize expressed in 32 bit words
        for (Index=0;Index < StackSize; Index++)
            ThreadStack[Index]=0;
        ThreadStack[StackSize-1] = (1 << 24); // Set the arithmetic flags to zero and ensure Thumb bit is set in PSR
        ThreadStack[StackSize-2] = (uint32_t) ThreadFn; // The "Stacked" PC goes here.
        // Fill in some test values to help debugging
        ThreadStack[StackSize-3] = 1; // LR
        ThreadStack[StackSize-4] = 2; // R12
        ThreadStack[StackSize-5] = 3; // R3
        ThreadStack[StackSize-6] = 4; // R2
        ThreadStack[StackSize-7] = 5; // R1
        ThreadStack[StackSize-8] = 6; // R0
        ThreadStack[StackSize-9] = 7; // R11
        ThreadStack[StackSize-10] = 8;// R10
        ThreadStack[StackSize-11] = 9;// R9
        ThreadStack[StackSize-12] = 10;// R8
        ThreadStack[StackSize-13] = 11;// R7
        ThreadStack[StackSize-14] = 12;// R6
        ThreadStack[StackSize-15] = 13;// R5
        ThreadStack[StackSize-16] = 14;// R4        
        Threads[ThreadCount].ThreadStack = &ThreadStack[StackSize-16];               
        Threads[ThreadCount].Attributes = 1; // lets say 1 means "schedulable"
        ThreadCount++;
        __asm(" cpsie i "); // enable interrupts
    }
}

__attribute__((noreturn)) void startSwitcher()
{

    // At the end of this function, the routine enters a never-ending while loop.
    // At the next interrupt, the contents of xPSR, PC, LR, R12 and R0-R3 are dumped
    // on the thread stack.  This consumes 32 bytes of stack on this stack.
    // If we do not take this into account, the stack pointer for the first thread will
    // be off by this amount so we need to adjust it in advanace 
		SysTick->LOAD = 15999;   // Configure System Tick interrupt rate of 16MHz / 16000 = 1kHz 
		SysTick->CTRL = 7;       // enable systick counting and interrupts, use core clock
		SysTick->VAL = 100; 		 // set SysTick to a low value so we don't have to wait long for switching to start
	
    __asm("LDR R0,=Threads "); // point to start of TCB array    
    __asm("LDR R0,[R0] ");   // read first Thread Stack pointer
    
    __asm("ADDS R0,#32 ");    // Adjust first thread stack
    __asm("MSR PSP,R0 ");    // write first stack pointer to process stack pointer        
    enable_interrupts();                
    while(1);    
}




