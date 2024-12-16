#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "architecture.h"

Arch arch;

Arch* generate_architecture(void) {

    arch.insts = calloc(MAX_OPCODES, sizeof(Inst));
    arch.microcode = calloc(1 << (MICRO_ADDR_WIDTH + 1), MICRO_DATA_WIDTH / 8);
    arch.initialized = true;

    // NOP
    new_ins("nop", ARG_NONE, "No operation");

    // Load data from memory to registers
    new_ins("lda", ARG_BYTE, "Load immediate value to A register");
    add_micro(OE_RAM, IE_A, OE_PC, IE_NO_ADDR, ALU_DEFAULT, CTL_PC_INC);

    new_ins("lda", ARG_PNTR, "Load contents of memory to A register");
    add_micro(OE_RAM, IE_MR_HI, OE_PC, IE_NO_ADDR, ALU_DEFAULT, CTL_PC_INC);
    add_micro(OE_RAM, IE_MR_LO, OE_PC, IE_NO_ADDR, ALU_DEFAULT, CTL_PC_INC);
    add_micro(OE_RAM, IE_A,     OE_MR, IE_NO_ADDR, ALU_DEFAULT, CTL_NONE);

    new_ins("ldx", ARG_BYTE, "Load immediate value to X register");
    add_micro(OE_RAM, IE_X, OE_PC, IE_NO_ADDR, ALU_DEFAULT, CTL_PC_INC);

    new_ins("ldx", ARG_PNTR, "Load contents of memory to X register");
    add_micro(OE_RAM, IE_MR_HI, OE_PC, IE_NO_ADDR, ALU_DEFAULT, CTL_PC_INC);
    add_micro(OE_RAM, IE_MR_LO, OE_PC, IE_NO_ADDR, ALU_DEFAULT, CTL_PC_INC);
    add_micro(OE_RAM, IE_X,     OE_MR, IE_NO_ADDR, ALU_DEFAULT, CTL_NONE);

    new_ins("ldy", ARG_BYTE, "Load immediate value to Y register");
    add_micro(OE_RAM, IE_Y, OE_PC, IE_NO_ADDR, ALU_DEFAULT, CTL_PC_INC);

    new_ins("ldy", ARG_PNTR, "Load contents of memory to Y register");
    add_micro(OE_RAM, IE_MR_HI, OE_PC, IE_NO_ADDR, ALU_DEFAULT, CTL_PC_INC);
    add_micro(OE_RAM, IE_MR_LO, OE_PC, IE_NO_ADDR, ALU_DEFAULT, CTL_PC_INC);
    add_micro(OE_RAM, IE_Y,     OE_MR, IE_NO_ADDR, ALU_DEFAULT, CTL_NONE);

    // Store data from regsiters to memory
    new_ins("sta", ARG_ADDR, "Store A register into memory");
    add_micro(OE_RAM, IE_MR_HI, OE_PC, IE_NO_ADDR, ALU_DEFAULT, CTL_PC_INC);
    add_micro(OE_RAM, IE_MR_LO, OE_PC, IE_NO_ADDR, ALU_DEFAULT, CTL_PC_INC);
    add_micro(OE_A,   IE_RAM,   OE_MR, IE_NO_ADDR, ALU_DEFAULT, CTL_NONE);

    new_ins("stx", ARG_ADDR, "Store X register into memory");
    add_micro(OE_RAM, IE_MR_HI, OE_PC, IE_NO_ADDR, ALU_DEFAULT, CTL_PC_INC);
    add_micro(OE_RAM, IE_MR_LO, OE_PC, IE_NO_ADDR, ALU_DEFAULT, CTL_PC_INC);
    add_micro(OE_X,   IE_RAM,   OE_MR, IE_NO_ADDR, ALU_DEFAULT, CTL_NONE);

    new_ins("sty", ARG_ADDR, "Store Y register into memory");
    add_micro(OE_RAM, IE_MR_HI, OE_PC, IE_NO_ADDR, ALU_DEFAULT, CTL_PC_INC);
    add_micro(OE_RAM, IE_MR_LO, OE_PC, IE_NO_ADDR, ALU_DEFAULT, CTL_PC_INC);
    add_micro(OE_Y,   IE_RAM,   OE_MR, IE_NO_ADDR, ALU_DEFAULT, CTL_NONE);

    // Transfer data between registers
    new_ins("tax", ARG_NONE, "Transfer A register to X register");
    add_micro(OE_A, IE_X, OE_NO_ADDR, IE_NO_ADDR, ALU_DEFAULT, CTL_NONE);

    new_ins("txa", ARG_NONE, "Transfer X register to A register");
    add_micro(OE_X, IE_A, OE_NO_ADDR, IE_NO_ADDR, ALU_DEFAULT, CTL_NONE);

    new_ins("tay", ARG_NONE, "Transfer A register to Y register");
    add_micro(OE_A, IE_Y, OE_NO_ADDR, IE_NO_ADDR, ALU_DEFAULT, CTL_NONE);

    new_ins("tya", ARG_NONE, "Transfer Y register to A register");
    add_micro(OE_Y, IE_A, OE_NO_ADDR, IE_NO_ADDR, ALU_DEFAULT, CTL_NONE);

    new_ins("txy", ARG_NONE, "Transfer X register to Y register");
    add_micro(OE_X, IE_Y, OE_NO_ADDR, IE_NO_ADDR, ALU_DEFAULT, CTL_NONE);

    new_ins("tyx", ARG_NONE, "Transfer Y register to X register");
    add_micro(OE_Y, IE_X, OE_NO_ADDR, IE_NO_ADDR, ALU_DEFAULT, CTL_NONE);

    // ALU Operations
    new_ins("add", ARG_BYTE, "Add immediate value to A register");
    add_micro(OE_RAM, IE_B, OE_PC,      IE_NO_ADDR, ALU_DEFAULT, CTL_PC_INC);
    add_micro(OE_ALU, IE_A, OE_NO_ADDR, IE_NO_ADDR, ALU_ADD,     CTL_SET_STATUS);

    new_ins("add", ARG_PNTR, "Add contents of memory to A register");
    add_micro(OE_RAM, IE_MR_HI, OE_PC,      IE_NO_ADDR, ALU_DEFAULT, CTL_PC_INC);
    add_micro(OE_RAM, IE_MR_LO, OE_PC,      IE_NO_ADDR, ALU_DEFAULT, CTL_PC_INC);
    add_micro(OE_RAM, IE_B,     OE_MR,      IE_NO_ADDR, ALU_DEFAULT, CTL_NONE);
    add_micro(OE_ALU, IE_A,     OE_NO_ADDR, IE_NO_ADDR, ALU_ADD,     CTL_SET_STATUS);

    new_ins("adx", ARG_NONE, "Add X register to A register");
    add_micro(OE_X,   IE_B, OE_NO_ADDR, IE_NO_ADDR, ALU_DEFAULT, CTL_NONE);
    add_micro(OE_ALU, IE_A, OE_NO_ADDR, IE_NO_ADDR, ALU_ADD,     CTL_SET_STATUS);

    new_ins("ady", ARG_NONE, "Add Y register to A register");
    add_micro(OE_Y,   IE_B, OE_NO_ADDR, IE_NO_ADDR, ALU_DEFAULT, CTL_NONE);
    add_micro(OE_ALU, IE_A, OE_NO_ADDR, IE_NO_ADDR, ALU_ADD,     CTL_SET_STATUS);

    new_ins("sub", ARG_BYTE, "Subtract immediate value from A register");
    add_micro(OE_RAM, IE_B, OE_PC,      IE_NO_ADDR, ALU_DEFAULT, CTL_PC_INC);
    add_micro(OE_ALU, IE_A, OE_NO_ADDR, IE_NO_ADDR, ALU_SUB,     CTL_SET_STATUS);

    new_ins("sub", ARG_PNTR, "Subtract contents of memory from A register");
    add_micro(OE_RAM, IE_MR_HI, OE_PC,      IE_NO_ADDR, ALU_DEFAULT, CTL_PC_INC);
    add_micro(OE_RAM, IE_MR_LO, OE_PC,      IE_NO_ADDR, ALU_DEFAULT, CTL_PC_INC);
    add_micro(OE_RAM, IE_B,     OE_MR,      IE_NO_ADDR, ALU_DEFAULT, CTL_NONE);
    add_micro(OE_ALU, IE_A,     OE_NO_ADDR, IE_NO_ADDR, ALU_SUB,     CTL_SET_STATUS);

    new_ins("sbx", ARG_NONE, "Subtract X register from A register");
    add_micro(OE_X,   IE_B, OE_NO_ADDR, IE_NO_ADDR, ALU_DEFAULT, CTL_NONE);
    add_micro(OE_ALU, IE_A, OE_NO_ADDR, IE_NO_ADDR, ALU_SUB,     CTL_SET_STATUS);

    new_ins("sby", ARG_NONE, "Subtract Y register from A register");
    add_micro(OE_Y,   IE_B, OE_NO_ADDR, IE_NO_ADDR, ALU_DEFAULT, CTL_NONE);
    add_micro(OE_ALU, IE_A, OE_NO_ADDR, IE_NO_ADDR, ALU_SUB,     CTL_SET_STATUS);

    // Compare instructions
    new_ins("cmp", ARG_BYTE, "Compare A register to immediate value");
    add_micro(OE_S,   IE_S, OE_NO_ADDR, IE_NO_ADDR, ALU_DEFAULT, CTL_SET_CARRY);
    add_micro(OE_RAM, IE_B, OE_PC,      IE_NO_ADDR, ALU_DEFAULT, CTL_PC_INC);
    add_micro(OE_NO_DATA, IE_NO_DATA, OE_NO_ADDR, IE_NO_ADDR, ALU_SUB, CTL_SET_STATUS);

    new_ins("cmp", ARG_PNTR, "Compare A register to value in memory");
    add_micro(OE_S,       IE_S,       OE_NO_ADDR, IE_NO_ADDR, ALU_DEFAULT, CTL_SET_CARRY);
    add_micro(OE_RAM,     IE_MR_HI,   OE_PC,      IE_NO_ADDR, ALU_DEFAULT, CTL_PC_INC);
    add_micro(OE_RAM,     IE_MR_LO,   OE_PC,      IE_NO_ADDR, ALU_DEFAULT, CTL_PC_INC);
    add_micro(OE_NO_DATA, IE_NO_DATA, OE_NO_ADDR, IE_NO_ADDR, ALU_SUB, CTL_SET_STATUS);

    // Set / Clear Status Flags
    new_ins("scf", ARG_NONE, "Set Carry Flag");
    add_micro(OE_S, IE_S, OE_NO_ADDR, IE_NO_ADDR, ALU_DEFAULT, CTL_SET_CARRY);

    new_ins("ccf", ARG_NONE, "Clear Carry Flag");
    add_micro(OE_S, IE_S, OE_NO_ADDR, IE_NO_ADDR, ALU_DEFAULT, CTL_CLR_CARRY);

    // Stack Operations
    new_ins("lsp", ARG_ADDR, "Set stack pointer to address");
    add_micro(OE_RAM,     IE_MR_HI,   OE_PC, IE_NO_ADDR, ALU_DEFAULT, CTL_PC_INC);
    add_micro(OE_RAM,     IE_MR_LO,   OE_PC, IE_NO_ADDR, ALU_DEFAULT, CTL_PC_INC);
    add_micro(OE_NO_DATA, IE_NO_DATA, OE_MR, IE_SP,      ALU_DEFAULT, CTL_NONE);

    new_ins("psa", ARG_NONE, "Push A register to stack");
    add_micro(OE_NO_DATA, IE_NO_DATA, OE_NO_ADDR, IE_NO_ADDR, ALU_DEFAULT, CTL_SP_DEC);
    add_micro(OE_A,       IE_RAM,     OE_SP,      IE_NO_ADDR, ALU_DEFAULT, CTL_NONE);

    new_ins("ppa", ARG_NONE, "Pop value off stack into A register");
    add_micro(OE_RAM, IE_A, OE_SP, IE_NO_ADDR, ALU_DEFAULT, CTL_SP_INC);

    new_ins("psx", ARG_NONE, "Push X register to stack");
    add_micro(OE_NO_DATA, IE_NO_DATA, OE_NO_ADDR, IE_NO_ADDR, ALU_DEFAULT, CTL_SP_DEC);
    add_micro(OE_X,       IE_RAM,     OE_SP,      IE_NO_ADDR, ALU_DEFAULT, CTL_NONE);

    new_ins("ppx", ARG_NONE, "Pop value off stack into X register");
    add_micro(OE_RAM, IE_X, OE_SP, IE_NO_ADDR, ALU_DEFAULT, CTL_SP_INC);

    new_ins("psy", ARG_NONE, "Push Y register to stack");
    add_micro(OE_NO_DATA, IE_NO_DATA, OE_NO_ADDR, IE_NO_ADDR, ALU_DEFAULT, CTL_SP_DEC);
    add_micro(OE_Y,       IE_RAM,     OE_SP,      IE_NO_ADDR, ALU_DEFAULT, CTL_NONE);

    new_ins("ppy", ARG_NONE, "Pop value off stack into Y register");
    add_micro(OE_RAM, IE_Y, OE_SP, IE_NO_ADDR, ALU_DEFAULT, CTL_SP_INC);

    // Jump Instruction
    new_ins("jmp", ARG_ADDR, "Jump to address");
    add_micro(OE_RAM,     IE_MR_HI,   OE_PC, IE_NO_ADDR, ALU_DEFAULT, CTL_PC_INC);
    add_micro(OE_RAM,     IE_MR_LO,   OE_PC, IE_NO_ADDR, ALU_DEFAULT, CTL_PC_INC);
    add_micro(OE_NO_DATA, IE_NO_DATA, OE_MR, IE_PC,      ALU_DEFAULT, CTL_NONE);

    // Call/Return From Subroutrine
    new_ins("csr", ARG_ADDR, "Call subroutine");
    add_micro(OE_NO_DATA, IE_NO_DATA, OE_PC, IE_MR,      ALU_DEFAULT, CTL_SP_DEC);  // Move PC to MR, decrement SP
    add_micro(OE_MR_HI,   IE_RAM,     OE_SP, IE_NO_ADDR, ALU_DEFAULT, CTL_SP_DEC);  // Store hi PC on stack
    add_micro(OE_MR_LO,   IE_RAM,     OE_SP, IE_NO_ADDR, ALU_DEFAULT, CTL_NONE);    // Store lo PC on stack
    add_micro(OE_RAM,     IE_MR_HI,   OE_PC, IE_NO_ADDR, ALU_DEFAULT, CTL_PC_INC);  // Read Address into MR HI byte
    add_micro(OE_RAM,     IE_MR_LO,   OE_PC, IE_NO_ADDR, ALU_DEFAULT, CTL_PC_INC);  // Read Address into MR LO byte
    add_micro(OE_NO_DATA, IE_NO_DATA, OE_MR, IE_PC,      ALU_DEFAULT, CTL_NONE);    // Set PC to address

    new_ins("ret", ARG_ADDR, "Return from subroutine");
    add_micro(OE_RAM,     IE_MR_HI,   OE_SP, IE_NO_ADDR, ALU_DEFAULT, CTL_SP_INC);  // Pop hi PC off stack
    add_micro(OE_RAM,     IE_MR_LO,   OE_SP, IE_NO_ADDR, ALU_DEFAULT, CTL_SP_INC);  // Pop lo PC off stack
    add_micro(OE_NO_DATA, IE_NO_DATA, OE_MR, IE_PC,      ALU_DEFAULT, CTL_NONE);    // Set PC to address

    // Halt
    new_ins("hlt", ARG_NONE, "Halt processor");
    arch.microcode[arch.opcode * MAX_STEPS] = 0; // Overwrite fetch instruction to halt processor

    // Branch instructions
    new_branch("bcs", FLAG_CARRY, FLAG_SET, "Branch if carry set");
    new_branch("bcc", FLAG_CARRY, FLAG_CLR, "Branch if carry clear");
    new_branch("bzs", FLAG_ZERO,  FLAG_SET, "Branch if zero set");
    new_branch("bzc", FLAG_ZERO,  FLAG_CLR, "Branch if zero clear");

    return &arch;
}

// Return whether mnemonic exists
bool is_mnemonic(char* str, long len) {

    assert(arch.initialized && "ERROR: Must initialize architecture first");
    if (len != 3) return false;

    for (uint8_t i = 0; i < arch.count; i++) {
        if (strncmp(arch.insts[i].mnemonic, str, 3) == 0) return true;
    }

    return false;
}

// Return whether instruction exists
bool ins_exists(char* str, ARG_TYPE type) {

    for (uint8_t i = 0; i < arch.count; i++) {
        Inst ins = arch.insts[i];
        if (strncmp(ins.mnemonic, str, 3) == 0 && ins.arg_type == type)
            return true;
    }

    return false;
}

// Return opcode if it exists
uint8_t get_opcode(char* str, ARG_TYPE type) {

    for (uint8_t i = 0; i < arch.count; i++) {
        Inst ins = arch.insts[i];
        if (strncmp(ins.mnemonic, str, 3) == 0 && ins.arg_type == type)
            return ins.opcode;
    }

    return 0;
}

// Add microcode step to instruction
void add_micro(DATA_OE data_oe, DATA_IE data_ie, ADDR_OE addr_oe, ADDR_IE addr_ie, ALU_FUN alu_fun, uint8_t ctl) {

    // Ensure opcode doesn't use greater than max number of steps
    assert(arch.step < MAX_STEPS && "ERROR: Max steps exceeded");

    // Calculate address, and encode control lines
    uint16_t addr;
    uint32_t word;
    addr = arch.opcode * MAX_STEPS + arch.step;

    word =  (data_oe << 20) + (data_ie << 16); 
    word += (addr_oe << 14) + (addr_ie << 12);
    word += (alu_fun << 8) + ctl;

    assert(word < (1 << 24) && "ERROR: Invalid microcode");

    arch.microcode[addr] = word;

    arch.step++;
}

// Create a new instruction
void new_ins(char* mnemonic, ARG_TYPE arg, char* desc) {

    assert(arch.data_count < MAX_DATA_INS);

    // Add microcode step to clear current step count, and reset to 0
    if (arch.step > 0 && arch.step < MAX_STEPS) {
        add_micro(OE_NO_DATA, IE_NO_DATA, OE_NO_ADDR, IE_NO_ADDR, ALU_DEFAULT, CTL_RESET_STEP);
    }
    arch.step = 0;

    // Add instruction to instruction list
    arch.opcode = arch.data_count;
    arch.insts[arch.count] = (Inst){arch.opcode, mnemonic, arg, desc};
    arch.count++;
    arch.data_count++;

    // Add microcode to fetch instruction
    add_micro(OE_RAM, IE_I, OE_PC, IE_NO_ADDR, ALU_DEFAULT, CTL_PC_INC);
}

// Create a new branch instruction
// 0b1<4 bits for instruction><3 status bits>
void new_branch(char* mnemonic, uint8_t bit, uint8_t set, char* desc) {

    assert(arch.branch_count < MAX_BRANCH_INS);

    // Add microcode step to clear current step count, and reset to 0
    if (arch.step > 0 && arch.step < MAX_STEPS) {
        add_micro(OE_NO_DATA, IE_NO_DATA, OE_NO_ADDR, IE_NO_ADDR, ALU_DEFAULT, CTL_RESET_STEP);
    }
    arch.step = 0;

    // Add instruction to instruction list
    uint8_t opcode = (1 << 7) + (arch.branch_count << 3);
    arch.insts[arch.count] = (Inst){opcode, mnemonic, ARG_ADDR, desc};
    arch.branch_count++;
    arch.count++;

    // For all possible status bits, Add microcode
    for (uint8_t i = 0; i < 8; i++) {
        arch.opcode = opcode + i;
        add_micro(OE_RAM, IE_I, OE_PC, IE_NO_ADDR, ALU_DEFAULT, CTL_PC_INC);
        if (((arch.opcode >> bit) & 1) == set) {
            add_micro(OE_RAM,     IE_MR_HI,   OE_PC, IE_NO_ADDR, ALU_DEFAULT, CTL_PC_INC);
            add_micro(OE_RAM,     IE_MR_LO,   OE_PC, IE_NO_ADDR, ALU_DEFAULT, CTL_PC_INC);
            add_micro(OE_NO_DATA, IE_NO_DATA, OE_MR, IE_PC,      ALU_DEFAULT, CTL_NONE);
        } else {
            add_micro(OE_NO_DATA, IE_NO_DATA, OE_NO_ADDR, IE_NO_ADDR, ALU_DEFAULT, CTL_PC_INC);
            add_micro(OE_NO_DATA, IE_NO_DATA, OE_NO_ADDR, IE_NO_ADDR, ALU_DEFAULT, CTL_PC_INC);

        }
        add_micro(OE_NO_DATA, IE_NO_DATA, OE_NO_ADDR, IE_NO_ADDR, ALU_DEFAULT, CTL_RESET_STEP);
        arch.step = 0;
    }
}

