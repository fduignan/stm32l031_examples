#include <stdint.h>
typedef struct {
    uint32_t *ThreadStack;
    void (*ThreadFn )(void);
    uint32_t Attributes;   
} ThreadControlBlock;

#define MAX_THREADS 10
extern uint32_t ThreadIndex;
extern uint32_t ThreadCount;
extern ThreadControlBlock Threads[MAX_THREADS];
extern uint32_t TCB_Size;

void createThread(void (*ThreadFn )(void), uint32_t *ThreadStack, uint32_t StackSize);
__attribute__((noreturn))  void startSwitcher(void);
