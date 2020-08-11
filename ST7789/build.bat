arm-none-eabi-gcc -std=c++17 -static -mthumb -g -mcpu=cortex-m0plus *.cpp -T linker_script.ld -o main.elf -nostartfiles -fno-rtti  -fno-exceptions
arm-none-eabi-objcopy -g -O binary main.elf main.bin
arm-none-eabi-objcopy -g -O ihex main.elf main.hex

