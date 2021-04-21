		AREA THUMB,CODE,READONLY
		EXPORT SysTick_Handler
		EXTERN Threads
		EXTERN ThreadIndex
		EXTERN TCB_Size
		EXTERN ThreadCount
SysTick_Handler
; Thread switching will happen here
; On entry, R0,R1,R2,R3 and R12 for Thread 'A' (whichever one that may be) are on the thread stack

; Preserve remaining registers on stack of thread that is being suspended (Thread A)     
    cpsid i ; // disable interrupts during thread switch
    MRS R0,PSP ;  // get Thread A stack pointer
    SUBS R0,#32;   // Make room for the other registers : R4-R11 = 8 x 4 = 32 bytes
    STMIA R0! , { R4-R7 } ; // Can only do a multiple store on registers up to R7
    MOV R4,R8 ; // Copy higher registers to lower ones
    MOV R5,R9 ;
    MOV R6,R10 ;
    MOV R7,R11 ;
    STMIA R0! , { R4-R7 } ; // and repeat the multiple register store
; Locate the Thread Control Block (TCB) for Thread A
    LDR R0,=TCB_Size ; // get the size of each TCB
    LDR R0,[R0] ;
    LDR R1,=ThreadIndex ; // Which one is being used right now?
    LDR R1,[R1] ;   
    MULS R1,R0,R1 ; // Calculate offset of Thread A TCB from start of TCB array
    LDR R0,=Threads ; // point to start of TCB array
    ADDS R1,R0,R1 ;  // add offset to get pointer to Thread A TCB
    MRS R0,PSP ;   // get Thread A stack pointer
;	Save Thread A's stack pointer (adjusted for new registers being pushed
    SUBS R0,#32 ;   // Adjust for the other registers : R4-R11 = 8 x 4 = 32 bytes
    STR R0,[R1] ;  // Save Thread A Stack pointer to the TCB (first entry = Saved stack pointer)
    
; Update the ThreadIndex	
	LDR R0,=ThreadIndex
	LDR R1,[R0];
	LDR R2,=ThreadCount;
	LDR R3,[R2] ;
	ADDS R1,R1,#1
	CMP R1,R3 ; reached threadcount?
	BLO systick_skip_zero ; if not then skip next bit
	MOVS R1,#0 ; otherwise zero the thread index
systick_skip_zero
	STR R1,[R0] ; and store it
    
;  Locate the Thread Control Block (TCB) for Thread B
    LDR R0,=TCB_Size ; // get the size of each TCB
    LDR R0,[R0] ;
    LDR R1,=ThreadIndex ; // Which one is being used right now?
    LDR R1,[R1] ;   
    MULS R1,R0,R1 ;  // Calculate offset of Thread A TCB from start of TCB array
    LDR R0,=Threads ; // point to start of TCB array
    ADDS R1,R0,R1 ;  // add offset to get pointer to Thread B TCB
    LDR R0,[R1] ;   // read saved Thread B Stack pointer
    ADDS R0,#16 ;    // Skip past saved low registers for the moment
    LDMIA R0!,{R4-R7} ; // read saved registers
    MOV R8,R4 ; // Copy higher registers to lower ones
    MOV R9,R5 ;
    MOV R10,R6 ;
    MOV R11,R7 ;
    LDR R0,[R1] ;   // read saved Thread B Stack pointer
    LDMIA R0!,{R4-R7} ; // read saved LOW registers
    LDR R0,[R1] ;   // read saved Thread B Stack pointer
    ADDS R0,#32 ;    // re-adjust saved stack pointer
    MSR PSP,R0 ;    // write Thread B stack pointer
    
    ; The following is only really necessary for the first run of the scheduler.
    MOVS R0,#0 ;     // Force LR to 0xffffffd so that
    SUBS R0,#3 ;     // on return from interrupt the CPU
    MOV LR,R0 ;     // will switch to thread stack
    

    cpsie i ; // enable interrupts
    BX LR ;  // return to Thread B
    
    end