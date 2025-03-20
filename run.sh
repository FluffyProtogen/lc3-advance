#!/bin/bash
arm-none-eabi-gcc -O3 -Wall -Wextra -Wpedantic crt0.s *.c assets/*.c assembler/*.c -mcpu=arm7tdmi --specs=nosys.specs -nostartfiles -T linkscript -o App.gba && mgba-qt App.gba
