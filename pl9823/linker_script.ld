/* Linker script for stm32l031F4P6 */
    FLASH_SIZE = 16384;
    FLASH_START = 0x08000000;
    RAM_SIZE = 8192;
    RAM_START = 0x20000000;
    STACK_TOP = RAM_START + RAM_SIZE;

MEMORY
{
    flash : org = FLASH_START, len = FLASH_SIZE
    ram : org = RAM_START, len = RAM_SIZE
}
  
SECTIONS
{
        
	. = ORIGIN(flash);
        .text : {
		  *(.vectors); /* The interrupt vectors */
		  *(.text);
        } >flash
	. = ORIGIN(ram);
        .data : {
	  INIT_DATA_VALUES = LOADADDR(.data);
	  INIT_DATA_START = .;
	    *(.data);
	  INIT_DATA_END = .;
        } >ram AT>flash
	BSS_START = .;
	.bss : {
	  
	    *(.bss);
	  
	} > ram
	BSS_END = .;
}
