arm-none-eabi-gcc -std=c11 -static -mthumb -g -mcpu=cortex-m0plus *.c -T linker_script.ld -o main.elf -nostartfiles 
arm-none-eabi-objcopy -g -O binary main.elf main.bin
arm-none-eabi-objcopy -g -O ihex main.elf main.hex
