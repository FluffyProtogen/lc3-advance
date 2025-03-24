#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    uint16_t memory[0xFFFF];
    uint16_t r0;
    uint16_t r1;
    uint16_t r2;
    uint16_t r3;
    uint16_t r4;
    uint16_t r5;
    uint16_t r6;
    uint16_t r7;
    uint16_t pc;
    enum {
        CC_POSITIVE = 1 << 0,
        CC_ZERO = 1 << 1,
        CC_NEGATIVE = 1 << 2,
    } cc;
    bool waiting_for_char;
    char char_input;
} VirtualMachine;

void vm_load(VirtualMachine *vm);

bool vm_exec_next_instruction(VirtualMachine *vm);
