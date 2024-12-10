#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "microcode.h"

#define MAX_OPCODES (32)
#define MAX_STEPS (8)
#define MICRO_ADDR_WIDTH (11)
#define MICRO_DATA_WIDTH (24)

// Microcode format
// Input -> 0b[8-bit OPCODE][3-bit STEP]
// Output -> 0b[4-bit OE Select][4-bit IE Select][8-bit Control Lines]

typedef struct {
    uint8_t opcode;         // Opcode of instruction
    char* mnemonic;         // Mnemonic of instruction
    ARG_TYPE arg_type;      // Argument type, if it exists
    char* desc;             // Description of opcode
} Inst;

typedef struct {
    Inst* insts;            // Instructions
    uint32_t* microcode;    // Microcode

    // Metadata
    uint8_t count;          // Total instruction count
    uint8_t step;           // Current microcode step
} Arch;


// Initialize architecture struct
void init_arch(Arch* arch) {

    arch->insts = calloc(MAX_OPCODES, sizeof(Inst));
    arch->microcode = calloc(1 << MICRO_ADDR_WIDTH, MICRO_DATA_WIDTH / 8);
    
    arch->count = 0;
    arch->step = 0;
}

// Add microcode step to instruction
void add_micro(Arch* arch, DATA_OE data_oe, DATA_IE data_ie, 
                           ADDR_OE addr_oe, ADDR_IE addr_ie,
                           ALU_FUN alu_fun, uint8_t ctl) {

    // Ensure opcode doesn't use greater than max number of steps
    assert(arch->step < MAX_STEPS && "ERROR: Max steps exceeded");

    // Calculate address, and encode control lines
    uint16_t addr;
    uint32_t word;
    addr = (arch->count - 1) * MAX_STEPS + arch->step;

    word =  (data_oe << 20) + (data_ie << 16); 
    word += (addr_oe << 14) + (addr_ie << 12);
    word += (alu_fun << 8) + ctl;

    assert(word < (1 << 24) && "ERROR: Invalid microcode");

    arch->microcode[addr] = word;

    arch->step++;
}

// Create a new instruction
void new_ins(Arch* arch, char* mnemonic, ARG_TYPE arg, char* desc) {

    //TODO - check that opcode isn't already in use

    // Add microcode step to clear current step count, and reset to 0
    if (arch->step > 0 && arch->step < MAX_STEPS) {
        add_micro(arch, OE_NO_DATA, IE_NO_DATA,
                        OE_NO_ADDR, IE_NO_ADDR,
                        ALU_DEFAULT, CTL_RESET_STEP);
    }
    arch->step = 0;

    // Add instruction to instruction list
    arch->insts[arch->count] = (Inst){arch->count, mnemonic, arg, desc};
    arch->count++;

    // Add microcode to fetch instruction
    add_micro(arch, OE_RAM, IE_I, OE_PC, IE_NO_ADDR, ALU_DEFAULT, CTL_PC_INC);

}

int main(void) {

    Arch* arch = calloc(1, sizeof(Arch));
    init_arch(arch);

    // NOP
    new_ins(arch, "NOP", ARG_NONE, "No operation");

    // Load data from memory to registers
    new_ins(arch, "LDA", ARG_BYTE, "Load immediate value to A register");
    add_micro(arch, OE_RAM, IE_A, OE_PC, IE_NO_ADDR, ALU_DEFAULT, CTL_PC_INC | CTL_SET_STATUS);

    new_ins(arch, "LDA", ARG_ADDR, "Load contents of memory to A register");
    add_micro(arch, OE_RAM, IE_MR_HI, OE_PC, IE_NO_ADDR, ALU_DEFAULT, CTL_PC_INC);
    add_micro(arch, OE_RAM, IE_MR_LO, OE_PC, IE_NO_ADDR, ALU_DEFAULT, CTL_PC_INC);
    add_micro(arch, OE_RAM, IE_A,     OE_MR, IE_NO_ADDR, ALU_DEFAULT, CTL_SET_STATUS);

    new_ins(arch, "LDX", ARG_BYTE, "Load immediate value to X register");
    add_micro(arch, OE_RAM, IE_X, OE_PC, IE_NO_ADDR, ALU_DEFAULT, CTL_PC_INC | CTL_SET_STATUS);

    new_ins(arch, "LDX", ARG_ADDR, "Load contents of memory to X register");
    add_micro(arch, OE_RAM, IE_MR_HI, OE_PC, IE_NO_ADDR, ALU_DEFAULT, CTL_PC_INC);
    add_micro(arch, OE_RAM, IE_MR_LO, OE_PC, IE_NO_ADDR, ALU_DEFAULT, CTL_PC_INC);
    add_micro(arch, OE_RAM, IE_X,     OE_MR, IE_NO_ADDR, ALU_DEFAULT, CTL_SET_STATUS);

    new_ins(arch, "LDY", ARG_BYTE, "Load immediate value to Y register");
    add_micro(arch, OE_RAM, IE_Y, OE_PC, IE_NO_ADDR, ALU_DEFAULT, CTL_PC_INC | CTL_SET_STATUS);

    new_ins(arch, "LDY", ARG_ADDR, "Load contents of memory to Y register");
    add_micro(arch, OE_RAM, IE_MR_HI, OE_PC, IE_NO_ADDR, ALU_DEFAULT, CTL_PC_INC);
    add_micro(arch, OE_RAM, IE_MR_LO, OE_PC, IE_NO_ADDR, ALU_DEFAULT, CTL_PC_INC);
    add_micro(arch, OE_RAM, IE_Y,     OE_MR, IE_NO_ADDR, ALU_DEFAULT, CTL_SET_STATUS);

    // Store data from regsiters to memory
    new_ins(arch, "STA", ARG_ADDR, "Store A register into memory");
    add_micro(arch, OE_RAM, IE_MR_HI, OE_PC, IE_NO_ADDR, ALU_DEFAULT, CTL_PC_INC);
    add_micro(arch, OE_RAM, IE_MR_LO, OE_PC, IE_NO_ADDR, ALU_DEFAULT, CTL_PC_INC);
    add_micro(arch, OE_A,   IE_RAM,   OE_MR, IE_NO_ADDR, ALU_DEFAULT, CTL_NONE);

    new_ins(arch, "STX", ARG_ADDR, "Store X register into memory");
    add_micro(arch, OE_RAM, IE_MR_HI, OE_PC, IE_NO_ADDR, ALU_DEFAULT, CTL_PC_INC);
    add_micro(arch, OE_RAM, IE_MR_LO, OE_PC, IE_NO_ADDR, ALU_DEFAULT, CTL_PC_INC);
    add_micro(arch, OE_X,   IE_RAM,   OE_MR, IE_NO_ADDR, ALU_DEFAULT, CTL_NONE);

    new_ins(arch, "STY", ARG_ADDR, "Store Y register into memory");
    add_micro(arch, OE_RAM, IE_MR_HI, OE_PC, IE_NO_ADDR, ALU_DEFAULT, CTL_PC_INC);
    add_micro(arch, OE_RAM, IE_MR_LO, OE_PC, IE_NO_ADDR, ALU_DEFAULT, CTL_PC_INC);
    add_micro(arch, OE_Y,   IE_RAM,   OE_MR, IE_NO_ADDR, ALU_DEFAULT, CTL_NONE);

    // Transfer data between registers
    new_ins(arch, "TAX", ARG_NONE, "Transfer A register to X register");
    add_micro(arch, OE_A, IE_X, OE_NO_ADDR, IE_NO_ADDR, ALU_DEFAULT, CTL_NONE);

    new_ins(arch, "TXA", ARG_NONE, "Transfer X register to A register");
    add_micro(arch, OE_X, IE_A, OE_NO_ADDR, IE_NO_ADDR, ALU_DEFAULT, CTL_NONE);

    new_ins(arch, "TAY", ARG_NONE, "Transfer A register to Y register");
    add_micro(arch, OE_A, IE_Y, OE_NO_ADDR, IE_NO_ADDR, ALU_DEFAULT, CTL_NONE);

    new_ins(arch, "TYA", ARG_NONE, "Transfer Y register to A register");
    add_micro(arch, OE_Y, IE_A, OE_NO_ADDR, IE_NO_ADDR, ALU_DEFAULT, CTL_NONE);

    new_ins(arch, "TXY", ARG_NONE, "Transfer X register to Y register");
    add_micro(arch, OE_X, IE_Y, OE_NO_ADDR, IE_NO_ADDR, ALU_DEFAULT, CTL_NONE);

    new_ins(arch, "TYX", ARG_NONE, "Transfer Y register to X register");
    add_micro(arch, OE_Y, IE_X, OE_NO_ADDR, IE_NO_ADDR, ALU_DEFAULT, CTL_NONE);

    // ALU Operations
    new_ins(arch, "ADD", ARG_BYTE, "Add immediate value A register");
    add_micro(arch, OE_RAM, IE_B, OE_PC,      IE_NO_ADDR, ALU_DEFAULT, CTL_PC_INC);
    add_micro(arch, OE_ALU, IE_A, OE_NO_ADDR, IE_NO_ADDR, ALU_ADD,     CTL_SET_STATUS);

    new_ins(arch, "ADD", ARG_ADDR, "Add contents of memory to A register");
    add_micro(arch, OE_RAM, IE_MR_HI, OE_PC,      IE_NO_ADDR, ALU_DEFAULT, CTL_PC_INC);
    add_micro(arch, OE_RAM, IE_MR_LO, OE_PC,      IE_NO_ADDR, ALU_DEFAULT, CTL_PC_INC);
    add_micro(arch, OE_RAM, IE_B,     OE_MR,      IE_NO_ADDR, ALU_DEFAULT, CTL_NONE);
    add_micro(arch, OE_ALU, IE_A,     OE_NO_ADDR, IE_NO_ADDR, ALU_ADD,     CTL_SET_STATUS);

    new_ins(arch, "SUB", ARG_BYTE, "Subtract immediate value from A register");
    add_micro(arch, OE_RAM, IE_B, OE_PC,      IE_NO_ADDR, ALU_DEFAULT, CTL_PC_INC);
    add_micro(arch, OE_ALU, IE_A, OE_NO_ADDR, IE_NO_ADDR, ALU_SUB,     CTL_SET_STATUS);

    new_ins(arch, "SUB", ARG_ADDR, "Subtract contents of memory from A register");
    add_micro(arch, OE_RAM, IE_MR_HI, OE_PC,      IE_NO_ADDR, ALU_DEFAULT, CTL_PC_INC);
    add_micro(arch, OE_RAM, IE_MR_LO, OE_PC,      IE_NO_ADDR, ALU_DEFAULT, CTL_PC_INC);
    add_micro(arch, OE_RAM, IE_B,     OE_MR,      IE_NO_ADDR, ALU_DEFAULT, CTL_NONE);
    add_micro(arch, OE_ALU, IE_A,     OE_NO_ADDR, IE_NO_ADDR, ALU_SUB,     CTL_SET_STATUS);

    // Set / Clear Status Flags
    new_ins(arch, "SCF", ARG_NONE, "Set Carry Flag");
    add_micro(arch, OE_S, IE_S, OE_NO_ADDR, IE_NO_ADDR, ALU_DEFAULT, CTL_SET_CARRY);

    new_ins(arch, "CCF", ARG_NONE, "Clear Carry Flag");
    add_micro(arch, OE_S, IE_S, OE_NO_ADDR, IE_NO_ADDR, ALU_DEFAULT, CTL_CLR_CARRY);

    // Stack Operations
    new_ins(arch, "LSP", ARG_ADDR, "Set stack pointer to address");
    add_micro(arch, OE_RAM,     IE_MR_HI,   OE_PC, IE_NO_ADDR, ALU_DEFAULT, CTL_PC_INC);
    add_micro(arch, OE_RAM,     IE_MR_LO,   OE_PC, IE_NO_ADDR, ALU_DEFAULT, CTL_PC_INC);
    add_micro(arch, OE_NO_DATA, IE_NO_DATA, OE_MR, IE_SP,      ALU_DEFAULT, CTL_NONE);

    new_ins(arch, "PSA", ARG_NONE, "Push A register to stack");
    add_micro(arch, OE_NO_DATA, IE_NO_DATA, OE_NO_ADDR, IE_NO_ADDR, ALU_DEFAULT, CTL_SP_DEC);
    add_micro(arch, OE_A,       IE_RAM,     OE_SP,      IE_NO_ADDR, ALU_DEFAULT, CTL_NONE);

    new_ins(arch, "PPA", ARG_NONE, "Pop value off stack into A register");
    add_micro(arch, OE_RAM, IE_A, OE_SP, IE_NO_ADDR, ALU_DEFAULT, CTL_SP_INC);

    new_ins(arch, "PSX", ARG_NONE, "Push X register to stack");
    add_micro(arch, OE_NO_DATA, IE_NO_DATA, OE_NO_ADDR, IE_NO_ADDR, ALU_DEFAULT, CTL_SP_DEC);
    add_micro(arch, OE_X,       IE_RAM,     OE_SP,      IE_NO_ADDR, ALU_DEFAULT, CTL_NONE);

    new_ins(arch, "PPX", ARG_NONE, "Pop value off stack into X register");
    add_micro(arch, OE_RAM, IE_X, OE_SP, IE_NO_ADDR, ALU_DEFAULT, CTL_SP_INC);

    new_ins(arch, "PSY", ARG_NONE, "Push Y register to stack");
    add_micro(arch, OE_NO_DATA, IE_NO_DATA, OE_NO_ADDR, IE_NO_ADDR, ALU_DEFAULT, CTL_SP_DEC);
    add_micro(arch, OE_Y,       IE_RAM,     OE_SP,      IE_NO_ADDR, ALU_DEFAULT, CTL_NONE);

    new_ins(arch, "PPY", ARG_NONE, "Pop value off stack into Y register");
    add_micro(arch, OE_RAM, IE_Y, OE_SP, IE_NO_ADDR, ALU_DEFAULT, CTL_SP_INC);

    // TODO - Jump Instructions
    // TODO - Call/Return from Subroutine

    // Add microcode step to clear current step count, and reset to 0
    if (arch->step > 0 && arch->step < MAX_STEPS) {
        add_micro(arch, OE_NO_DATA, IE_NO_DATA, OE_NO_ADDR, IE_NO_ADDR, ALU_DEFAULT, CTL_RESET_STEP);
    }
    arch->step = 0;

    // Dump out microcode
    for (int i = 0; i < arch->count; i++) {
        Inst inst = arch->insts[i];
        printf("%02x - %s\n", inst.opcode, inst.mnemonic);
        for (int j = 0; j < MAX_STEPS; j++) {
            printf("[%4d] %06x\n", i * MAX_STEPS + j,
                                   arch->microcode[i * MAX_STEPS + j]);

        }
    }

    // Dump out to file
    FILE *f = fopen("microcode.bin", "wb");
    assert(f != NULL);

    for (int i = 0; i < MAX_OPCODES; i++) {
        for (int j = 0; j < MAX_STEPS; j++) {
            // Data is stored in 32bit int, but we only need 24 bits
            uint32_t word = arch->microcode[i * MAX_STEPS + j];
            uint8_t bytes[3];
            bytes[2] = word & 0xff;
            bytes[1] = (word >> 8) & 0xff;
            bytes[0] = (word >> 16) & 0xff;
            fwrite(bytes, sizeof(uint8_t), 3, f);
        }
    }
    fclose(f);
    


}
