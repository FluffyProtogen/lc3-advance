#!/bin/bash
arm-none-eabi-gcc -O3 -Wall -Wextra -Wpedantic crt0.s *.c assets/*.c -mcpu=arm7tdmi -nostartfiles -T linkscript -o App.gba && mgba-qt App.gba
