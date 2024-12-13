#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "architecture.h"

Arch* gen_arch(void) {

    Arch* arch = calloc(1, sizeof(Arch));
    arch->insts = calloc(MAX_OPCODES, sizeof(Inst));
    arch->microcode = calloc(1 << (MICRO_ADDR_WIDTH + 1), MICRO_DATA_WIDTH / 8);

    // NOP
    new_ins(arch, "nop", ARG_NONE, "No operation");

    // Load data from memory to registers
    new_ins(arch, "lda", ARG_BYTE, "Load immediate value to A register");
    add_micro(arch, OE_RAM, IE_A, OE_PC, IE_NO_ADDR, ALU_DEFAULT, CTL_PC_INC);

    new_ins(arch, "lda", ARG_PNTR, "Load contents of memory to A register");
    add_micro(arch, OE_RAM, IE_MR_HI, OE_PC, IE_NO_ADDR, ALU_DEFAULT, CTL_PC_INC);
    add_micro(arch, OE_RAM, IE_MR_LO, OE_PC, IE_NO_ADDR, ALU_DEFAULT, CTL_PC_INC);
    add_micro(arch, OE_RAM, IE_A,     OE_MR, IE_NO_ADDR, ALU_DEFAULT, CTL_NONE);

    new_ins(arch, "ldx", ARG_BYTE, "Load immediate value to X register");
    add_micro(arch, OE_RAM, IE_X, OE_PC, IE_NO_ADDR, ALU_DEFAULT, CTL_PC_INC);

    new_ins(arch, "ldx", ARG_PNTR, "Load contents of memory to X register");
    add_micro(arch, OE_RAM, IE_MR_HI, OE_PC, IE_NO_ADDR, ALU_DEFAULT, CTL_PC_INC);
    add_micro(arch, OE_RAM, IE_MR_LO, OE_PC, IE_NO_ADDR, ALU_DEFAULT, CTL_PC_INC);
    add_micro(arch, OE_RAM, IE_X,     OE_MR, IE_NO_ADDR, ALU_DEFAULT, CTL_NONE);

    new_ins(arch, "ldy", ARG_BYTE, "Load immediate value to Y register");
    add_micro(arch, OE_RAM, IE_Y, OE_PC, IE_NO_ADDR, ALU_DEFAULT, CTL_PC_INC);

    new_ins(arch, "ldy", ARG_PNTR, "Load contents of memory to Y register");
    add_micro(arch, OE_RAM, IE_MR_HI, OE_PC, IE_NO_ADDR, ALU_DEFAULT, CTL_PC_INC);
    add_micro(arch, OE_RAM, IE_MR_LO, OE_PC, IE_NO_ADDR, ALU_DEFAULT, CTL_PC_INC);
    add_micro(arch, OE_RAM, IE_Y,     OE_MR, IE_NO_ADDR, ALU_DEFAULT, CTL_NONE);

    // Store data from regsiters to memory
    new_ins(arch, "sta", ARG_ADDR, "Store A register into memory");
    add_micro(arch, OE_RAM, IE_MR_HI, OE_PC, IE_NO_ADDR, ALU_DEFAULT, CTL_PC_INC);
    add_micro(arch, OE_RAM, IE_MR_LO, OE_PC, IE_NO_ADDR, ALU_DEFAULT, CTL_PC_INC);
    add_micro(arch, OE_A,   IE_RAM,   OE_MR, IE_NO_ADDR, ALU_DEFAULT, CTL_NONE);

    new_ins(arch, "stx", ARG_ADDR, "Store X register into memory");
    add_micro(arch, OE_RAM, IE_MR_HI, OE_PC, IE_NO_ADDR, ALU_DEFAULT, CTL_PC_INC);
    add_micro(arch, OE_RAM, IE_MR_LO, OE_PC, IE_NO_ADDR, ALU_DEFAULT, CTL_PC_INC);
    add_micro(arch, OE_X,   IE_RAM,   OE_MR, IE_NO_ADDR, ALU_DEFAULT, CTL_NONE);

    new_ins(arch, "sty", ARG_ADDR, "Store Y register into memory");
    add_micro(arch, OE_RAM, IE_MR_HI, OE_PC, IE_NO_ADDR, ALU_DEFAULT, CTL_PC_INC);
    add_micro(arch, OE_RAM, IE_MR_LO, OE_PC, IE_NO_ADDR, ALU_DEFAULT, CTL_PC_INC);
    add_micro(arch, OE_Y,   IE_RAM,   OE_MR, IE_NO_ADDR, ALU_DEFAULT, CTL_NONE);

    // Transfer data between registers
    new_ins(arch, "tax", ARG_NONE, "Transfer A register to X register");
    add_micro(arch, OE_A, IE_X, OE_NO_ADDR, IE_NO_ADDR, ALU_DEFAULT, CTL_NONE);

    new_ins(arch, "txa", ARG_NONE, "Transfer X register to A register");
    add_micro(arch, OE_X, IE_A, OE_NO_ADDR, IE_NO_ADDR, ALU_DEFAULT, CTL_NONE);

    new_ins(arch, "tay", ARG_NONE, "Transfer A register to Y register");
    add_micro(arch, OE_A, IE_Y, OE_NO_ADDR, IE_NO_ADDR, ALU_DEFAULT, CTL_NONE);

    new_ins(arch, "tya", ARG_NONE, "Transfer Y register to A register");
    add_micro(arch, OE_Y, IE_A, OE_NO_ADDR, IE_NO_ADDR, ALU_DEFAULT, CTL_NONE);

    new_ins(arch, "txy", ARG_NONE, "Transfer X register to Y register");
    add_micro(arch, OE_X, IE_Y, OE_NO_ADDR, IE_NO_ADDR, ALU_DEFAULT, CTL_NONE);

    new_ins(arch, "tyx", ARG_NONE, "Transfer Y register to X register");
    add_micro(arch, OE_Y, IE_X, OE_NO_ADDR, IE_NO_ADDR, ALU_DEFAULT, CTL_NONE);

    // ALU Operations
    new_ins(arch, "add", ARG_BYTE, "Add immediate value A register");
    add_micro(arch, OE_RAM, IE_B, OE_PC,      IE_NO_ADDR, ALU_DEFAULT, CTL_PC_INC);
    add_micro(arch, OE_ALU, IE_A, OE_NO_ADDR, IE_NO_ADDR, ALU_ADD,     CTL_SET_STATUS);

    new_ins(arch, "add", ARG_PNTR, "Add contents of memory to A register");
    add_micro(arch, OE_RAM, IE_MR_HI, OE_PC,      IE_NO_ADDR, ALU_DEFAULT, CTL_PC_INC);
    add_micro(arch, OE_RAM, IE_MR_LO, OE_PC,      IE_NO_ADDR, ALU_DEFAULT, CTL_PC_INC);
    add_micro(arch, OE_RAM, IE_B,     OE_MR,      IE_NO_ADDR, ALU_DEFAULT, CTL_NONE);
    add_micro(arch, OE_ALU, IE_A,     OE_NO_ADDR, IE_NO_ADDR, ALU_ADD,     CTL_SET_STATUS);

    new_ins(arch, "sub", ARG_BYTE, "Subtract immediate value from A register");
    add_micro(arch, OE_RAM, IE_B, OE_PC,      IE_NO_ADDR, ALU_DEFAULT, CTL_PC_INC);
    add_micro(arch, OE_ALU, IE_A, OE_NO_ADDR, IE_NO_ADDR, ALU_SUB,     CTL_SET_STATUS);

    new_ins(arch, "sub", ARG_PNTR, "Subtract contents of memory from A register");
    add_micro(arch, OE_RAM, IE_MR_HI, OE_PC,      IE_NO_ADDR, ALU_DEFAULT, CTL_PC_INC);
    add_micro(arch, OE_RAM, IE_MR_LO, OE_PC,      IE_NO_ADDR, ALU_DEFAULT, CTL_PC_INC);
    add_micro(arch, OE_RAM, IE_B,     OE_MR,      IE_NO_ADDR, ALU_DEFAULT, CTL_NONE);
    add_micro(arch, OE_ALU, IE_A,     OE_NO_ADDR, IE_NO_ADDR, ALU_SUB,     CTL_SET_STATUS);

    // Set / Clear Status Flags
    new_ins(arch, "scf", ARG_NONE, "Set Carry Flag");
    add_micro(arch, OE_S, IE_S, OE_NO_ADDR, IE_NO_ADDR, ALU_DEFAULT, CTL_SET_CARRY);

    new_ins(arch, "ccf", ARG_NONE, "Clear Carry Flag");
    add_micro(arch, OE_S, IE_S, OE_NO_ADDR, IE_NO_ADDR, ALU_DEFAULT, CTL_CLR_CARRY);

    // Stack Operations
    new_ins(arch, "lsp", ARG_ADDR, "Set stack pointer to address");
    add_micro(arch, OE_RAM,     IE_MR_HI,   OE_PC, IE_NO_ADDR, ALU_DEFAULT, CTL_PC_INC);
    add_micro(arch, OE_RAM,     IE_MR_LO,   OE_PC, IE_NO_ADDR, ALU_DEFAULT, CTL_PC_INC);
    add_micro(arch, OE_NO_DATA, IE_NO_DATA, OE_MR, IE_SP,      ALU_DEFAULT, CTL_NONE);

    new_ins(arch, "psa", ARG_NONE, "Push A register to stack");
    add_micro(arch, OE_NO_DATA, IE_NO_DATA, OE_NO_ADDR, IE_NO_ADDR, ALU_DEFAULT, CTL_SP_DEC);
    add_micro(arch, OE_A,       IE_RAM,     OE_SP,      IE_NO_ADDR, ALU_DEFAULT, CTL_NONE);

    new_ins(arch, "ppa", ARG_NONE, "Pop value off stack into A register");
    add_micro(arch, OE_RAM, IE_A, OE_SP, IE_NO_ADDR, ALU_DEFAULT, CTL_SP_INC);

    new_ins(arch, "psx", ARG_NONE, "Push X register to stack");
    add_micro(arch, OE_NO_DATA, IE_NO_DATA, OE_NO_ADDR, IE_NO_ADDR, ALU_DEFAULT, CTL_SP_DEC);
    add_micro(arch, OE_X,       IE_RAM,     OE_SP,      IE_NO_ADDR, ALU_DEFAULT, CTL_NONE);

    new_ins(arch, "ppx", ARG_NONE, "Pop value off stack into X register");
    add_micro(arch, OE_RAM, IE_X, OE_SP, IE_NO_ADDR, ALU_DEFAULT, CTL_SP_INC);

    new_ins(arch, "psy", ARG_NONE, "Push Y register to stack");
    add_micro(arch, OE_NO_DATA, IE_NO_DATA, OE_NO_ADDR, IE_NO_ADDR, ALU_DEFAULT, CTL_SP_DEC);
    add_micro(arch, OE_Y,       IE_RAM,     OE_SP,      IE_NO_ADDR, ALU_DEFAULT, CTL_NONE);

    new_ins(arch, "ppy", ARG_NONE, "Pop value off stack into Y register");
    add_micro(arch, OE_RAM, IE_Y, OE_SP, IE_NO_ADDR, ALU_DEFAULT, CTL_SP_INC);

    // Jump Instruction
    new_ins(arch, "jmp", ARG_ADDR, "Jump to address");
    add_micro(arch, OE_RAM,     IE_MR_HI,   OE_PC, IE_NO_ADDR, ALU_DEFAULT, CTL_PC_INC);
    add_micro(arch, OE_RAM,     IE_MR_LO,   OE_PC, IE_NO_ADDR, ALU_DEFAULT, CTL_PC_INC);
    add_micro(arch, OE_NO_DATA, IE_NO_DATA, OE_MR, IE_PC,      ALU_DEFAULT, CTL_PC_INC);

    // Call/Return From Subroutrine
    new_ins(arch, "csr", ARG_ADDR, "Call subroutine");
    add_micro(arch, OE_NO_DATA, IE_NO_DATA, OE_PC, IE_MR,      ALU_DEFAULT, CTL_SP_DEC);  // Move PC to MR, decrement SP
    add_micro(arch, OE_MR_HI,   IE_RAM,     OE_SP, IE_NO_ADDR, ALU_DEFAULT, CTL_SP_DEC);  // Store hi PC on stack
    add_micro(arch, OE_MR_LO,   IE_RAM,     OE_SP, IE_NO_ADDR, ALU_DEFAULT, CTL_NONE);    // Store lo PC on stack
    add_micro(arch, OE_RAM,     IE_MR_HI,   OE_PC, IE_NO_ADDR, ALU_DEFAULT, CTL_PC_INC);  // Read Address into MR HI byte
    add_micro(arch, OE_RAM,     IE_MR_LO,   OE_PC, IE_NO_ADDR, ALU_DEFAULT, CTL_PC_INC);  // Read Address into MR LO byte
    add_micro(arch, OE_NO_DATA, IE_NO_DATA, OE_MR, IE_PC,      ALU_DEFAULT, CTL_NONE);    // Set PC to address

    new_ins(arch, "ret", ARG_ADDR, "Return from subroutine");
    add_micro(arch, OE_RAM,     IE_MR_HI,   OE_SP, IE_NO_ADDR, ALU_DEFAULT, CTL_SP_INC);  // Pop hi PC off stack
    add_micro(arch, OE_RAM,     IE_MR_LO,   OE_SP, IE_NO_ADDR, ALU_DEFAULT, CTL_SP_INC);  // Pop lo PC off stack
    add_micro(arch, OE_NO_DATA, IE_NO_DATA, OE_MR, IE_PC,      ALU_DEFAULT, CTL_NONE);    // Set PC to address

    // Branch instructions
    new_branch(arch, "bcs", 0, 1, "Branch if carry set");
    new_branch(arch, "bcc", 0, 0, "Branch if carry clear");
    new_branch(arch, "bzs", 1, 1, "Branch if zero set");
    new_branch(arch, "bzc", 1, 0, "Branch if zero clear");

    return arch;
}

// Add microcode step to instruction
void add_micro(Arch* arch, DATA_OE data_oe, DATA_IE data_ie, ADDR_OE addr_oe, ADDR_IE addr_ie, ALU_FUN alu_fun, uint8_t ctl) {

    // Ensure opcode doesn't use greater than max number of steps
    assert(arch->step < MAX_STEPS && "ERROR: Max steps exceeded");

    // Calculate address, and encode control lines
    uint16_t addr;
    uint32_t word;
    addr = arch->opcode * MAX_STEPS + arch->step;

    word =  (data_oe << 20) + (data_ie << 16); 
    word += (addr_oe << 14) + (addr_ie << 12);
    word += (alu_fun << 8) + ctl;

    assert(word < (1 << 24) && "ERROR: Invalid microcode");

    arch->microcode[addr] = word;

    arch->step++;
}

// Create a new instruction
void new_ins(Arch* arch, char* mnemonic, ARG_TYPE arg, char* desc) {

    assert(arch->data_count < MAX_DATA_INS);

    // Add microcode step to clear current step count, and reset to 0
    if (arch->step > 0 && arch->step < MAX_STEPS) {
        add_micro(arch, OE_NO_DATA, IE_NO_DATA, OE_NO_ADDR, IE_NO_ADDR, ALU_DEFAULT, CTL_RESET_STEP);
    }
    arch->step = 0;

    // Add instruction to instruction list
    arch->opcode = arch->data_count;
    arch->insts[arch->count] = (Inst){arch->opcode, mnemonic, arg, desc};
    arch->count++;
    arch->data_count++;

    // Add microcode to fetch instruction
    add_micro(arch, OE_RAM, IE_I, OE_PC, IE_NO_ADDR, ALU_DEFAULT, CTL_PC_INC);
}

// Create a new branch instruction
// 0b1<4 bits for instruction><3 status bits>
void new_branch(Arch* arch, char* mnemonic, uint8_t bit, uint8_t set, char* desc) {

    assert(arch->branch_count < MAX_BRANCH_INS);

    // Add microcode step to clear current step count, and reset to 0
    if (arch->step > 0 && arch->step < MAX_STEPS) {
        add_micro(arch, OE_NO_DATA, IE_NO_DATA, OE_NO_ADDR, IE_NO_ADDR, ALU_DEFAULT, CTL_RESET_STEP);
    }
    arch->step = 0;

    // Add instruction to instruction list
    uint8_t opcode = (1 << 7) + (arch->branch_count << 3);
    arch->insts[arch->count] = (Inst){opcode, mnemonic, ARG_ADDR, desc};
    arch->branch_count++;
    arch->count++;

    // For all possible status bits, Add microcode
    for (uint8_t i = 0; i < 8; i++) {
        arch->opcode = opcode + i;
        add_micro(arch, OE_RAM, IE_I, OE_PC, IE_NO_ADDR, ALU_DEFAULT, CTL_PC_INC);
        if ((arch->opcode & (1 << bit)) == set) {
            add_micro(arch, OE_RAM,     IE_MR_HI,   OE_PC, IE_NO_ADDR, ALU_DEFAULT, CTL_PC_INC);
            add_micro(arch, OE_RAM,     IE_MR_LO,   OE_PC, IE_NO_ADDR, ALU_DEFAULT, CTL_PC_INC);
            add_micro(arch, OE_NO_DATA, IE_NO_DATA, OE_MR, IE_PC,      ALU_DEFAULT, CTL_PC_INC);
        }
        add_micro(arch, OE_NO_DATA, IE_NO_DATA, OE_NO_ADDR, IE_NO_ADDR, ALU_DEFAULT, CTL_RESET_STEP);
        arch->step = 0;
    }
}

