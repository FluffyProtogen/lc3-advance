#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include "assembler/object.h"
#include "vm.h"
#include "vm_viewer.h"

void parse_hex(char hex[4], uint16_t *result) {
    if (hex[0] == 0) {
        *result = 0;
        return;
    }
    if (hex[0] == '?')
        return;
    *result = 0;
    for (int i = 3, shift = 0; i >= 0; i--, shift += 4)
        *result += (hex[i] - ((hex[i] > '9') ? ('A' - 10) : '0')) << shift;
}

uint16_t parse_uint(char dec[4]) {
    int o = 0;
    while (dec[o])
        o++;
    uint16_t r = 0;
    for (int i = 0; i < o; i++)
        r = (r << 3) + (r << 1) + dec[i] - '0';
    return r;
}

void vm_load(VirtualMachine *vm) {
    for (size_t i = 0; i < (sizeof(vm->memory) / sizeof(*vm->memory)) / sizeof(uint32_t); i++)
        ((uint32_t *)vm->memory)[i] = rand();

    for (size_t i = 0; i < sizeof(VirtualMachine) - sizeof(vm->memory); i++)
        ((uint8_t *)&vm->r0)[i] = rand();
    vm->waiting_for_char = false;

    uint16_t line = 0;
    uint16_t cur_addr = 0;
    parse_hex(obj_buf[line++], &cur_addr);
    uint16_t left_to_read = parse_uint(obj_buf[line++]);
    vm->pc = cur_addr;
    for (;;) {
        for (int i = 0; i < left_to_read; i++)
            parse_hex(obj_buf[line++], &vm->memory[cur_addr++]);
        parse_hex(obj_buf[line++], &cur_addr);
        if (cur_addr == 0)
            return;
        left_to_read = parse_uint(obj_buf[line++]);
        if (left_to_read == 0)
            return;
    }
}

uint16_t sext(uint16_t num, uint16_t bit_count) {
    if (((num >> (bit_count - 1)) & 1) == 1)
        return num | (0xFFFF << bit_count);
    else
        return num;
}

uint16_t read_reg(const VirtualMachine *vm, uint16_t reg) {
    return *(const uint16_t *[]){&vm->r0, &vm->r1, &vm->r2, &vm->r3, &vm->r4, &vm->r5, &vm->r6, &vm->r7}[reg];
}

void write_reg(VirtualMachine *vm, uint16_t reg, uint16_t value) {
    *(uint16_t *[]){&vm->r0, &vm->r1, &vm->r2, &vm->r3, &vm->r4, &vm->r5, &vm->r6, &vm->r7}[reg] = value;
    if (value == 0)
        vm->cc = CC_ZERO;
    else if (value >> 15)
        vm->cc = CC_NEGATIVE;
    else
        vm->cc = CC_POSITIVE;
}

bool vm_exec_next_instruction(VirtualMachine *vm) {
    if (vm->waiting_for_char) {
        if (vm->char_input != 0) {
            vm->r0 = vm->char_input;
            vm->waiting_for_char = false;
        } else
            return true;
    }

    uint16_t instr = vm->memory[vm->pc++];
    uint16_t op_code = instr >> 12;
    uint16_t dr, sr, sr1, sr2, br, imm5, offset, value, addr;

    switch (op_code) {
        case 0x0:;  // BR
            uint16_t flags = (instr >> 9) & 0x7;
            if (flags & vm->cc)
                vm->pc += sext(instr & 0x1FF, 9);
            break;
        case 0x1:  // ADD
            dr = (instr >> 9) & 0x7;
            sr1 = (instr >> 6) & 0x7;
            if ((instr >> 5) & 0x1) {
                imm5 = sext(instr & 0x1F, 5);
                uint16_t result = read_reg(vm, sr1) + imm5;
                write_reg(vm, dr, result);
            } else {
                sr2 = instr & 0x7;
                uint16_t result = read_reg(vm, sr1) + read_reg(vm, sr2);
                write_reg(vm, dr, result);
            }
            break;
        case 0x2:  // LD
            dr = (instr >> 9) & 0x7;
            offset = sext(instr & 0x1FF, 9);
            addr = vm->pc + offset;
            value = vm->memory[addr];
            write_reg(vm, dr, value);
            break;
        case 0x3:  // ST
            sr = (instr >> 9) & 0x7;
            offset = sext(instr & 0x1FF, 9);
            addr = vm->pc + offset;
            vm->memory[addr] = read_reg(vm, sr);
            break;
        case 0x4:  // JSR
            write_reg(vm, 7, vm->pc);
            if ((instr >> 11) & 0x1)
                vm->pc += sext(instr & 0x7FF, 11);
            else
                vm->pc += read_reg(vm, (instr >> 6) & 0x7);
            break;
        case 0x5:  // AND
            dr = (instr >> 9) & 0x7;
            sr1 = (instr >> 6) & 0x7;
            if ((instr >> 5) & 0x1) {
                imm5 = sext(instr & 0x1F, 5);
                uint16_t result = read_reg(vm, sr1) & imm5;
                write_reg(vm, dr, result);
            } else {
                sr2 = instr & 0x7;
                uint16_t result = read_reg(vm, sr1) & read_reg(vm, sr2);
                write_reg(vm, dr, result);
            }
            break;
        case 0x6:  // LDR
            dr = (instr >> 9) & 0x7;
            br = (instr >> 6) & 0x7;
            offset = sext(instr & 0x3F, 6);
            addr = read_reg(vm, br) + offset;
            value = vm->memory[addr];
            write_reg(vm, dr, value);
            break;
        case 0x7:  // STR
            sr = (instr >> 9) & 0x7;
            br = (instr >> 6) & 0x7;
            offset = sext(instr & 0x3F, 6);
            addr = read_reg(vm, br) + offset;
            vm->memory[addr] = read_reg(vm, sr);
            break;
        case 0x8:  // RTI
            break;
        case 0x9:  // NOT
            dr = (instr >> 9) & 0x7;
            sr = (instr >> 6) & 0x7;
            write_reg(vm, dr, ~read_reg(vm, sr));
            break;
        case 0xA:  // LDI
            dr = (instr >> 9) & 0x7;
            offset = sext(instr & 0x1FF, 9);
            addr = vm->pc + offset;
            value = vm->memory[addr];
            write_reg(vm, dr, vm->memory[value]);
            break;
        case 0xB:  // STI
            sr = (instr >> 9) & 0x7;
            offset = sext(instr & 0x1FF, 9);
            addr = vm->pc + offset;
            value = vm->memory[addr];
            vm->memory[value] = read_reg(vm, sr);
            break;
        case 0xC:  // JMP
            vm->pc = read_reg(vm, (instr >> 6) & 0x7);
            break;
        case 0xD:  // reserved
            break;
        case 0xE:  // LEA
            dr = (instr >> 9) & 0x7;
            offset = sext(instr & 0x1FF, 9);
            *(uint16_t *[]){
                &vm->r0, &vm->r1, &vm->r2, &vm->r3, &vm->r4, &vm->r5, &vm->r6, &vm->r7,
            }[dr] = vm->pc + offset;
            break;
        case 0xF:;  // Trap
            int trap = instr & 0xFF;
            switch (trap) {
                case 0x20:  // GETC
                    vm->waiting_for_char = true;
                    vm->char_input = 0;
                    break;
                case 0x21:  // OUT
                    vm_put_char(vm->r0);
                    break;
                case 0x22:;  // PUTS
                    uint16_t i = vm->r0;
                    for (;;) {
                        char c = vm->memory[i++];
                        if (!c)
                            break;
                        vm_put_char(c);
                    }
                    break;
                case 0x25:  // HALT
                    return false;
            }
    }
    return true;
}
