#ifndef ARCHITECTURE_H
#define ARCHITECTURE_H

#include <stdint.h>

#define MAX_OPCODES (256)
#define MAX_DATA_INS (127)
#define MAX_BRANCH_INS (15)
#define MAX_STEPS (8)
#define MICRO_ADDR_WIDTH (11)
#define MICRO_DATA_WIDTH (24)

typedef enum {
    ARG_NONE,
    ARG_BYTE,
    ARG_ADDR,
    ARG_PNTR,
} ARG_TYPE;

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
    uint8_t opcode;         // Current working opcode
    uint8_t count;          // Total instruction count
    uint8_t data_count;     // Total number of data instructions
    uint8_t branch_count;   // Total number of branch instructions
    uint8_t step;           // Current microcode step
} Arch;

typedef enum {
    OE_NO_DATA,
    OE_RAM,
    OE_A,
    OE_X,
    OE_Y,
    OE_S,
    OE_MR_LO,
    OE_MR_HI,
    OE_ALU,
} DATA_OE;

typedef enum {
    IE_NO_DATA,
    IE_RAM,
    IE_A,
    IE_X,
    IE_Y,
    IE_S,
    IE_MR_LO,
    IE_MR_HI,
    IE_B,
    IE_I,
} DATA_IE;

typedef enum {
    OE_NO_ADDR,
    OE_PC,
    OE_SP,
    OE_MR,
} ADDR_OE;

typedef enum {
    IE_NO_ADDR,
    IE_PC,
    IE_SP,
    IE_MR,
} ADDR_IE;

typedef enum {
    ALU_DEFAULT = 0,
    ALU_AND = 1,    // Carry should be set
    ALU_SUB = 6,
    ALU_ADD = 9,
    ALU_OR = 11,
} ALU_FUN;

typedef enum {
    CTL_NONE            = 0,
    CTL_PC_INC          = 1 << 0,
    CTL_SP_INC          = 1 << 1,
    CTL_SP_DEC          = 1 << 2,
    CTL_SET_STATUS      = 1 << 3,
    CTL_SET_CARRY       = 1 << 4,
    CTL_CLR_CARRY       = 1 << 5,
    CTL_RESET_STEP      = 1 << 6,
} CTL_LINES;

// Public functions
Arch* gen_arch(void);

// Private functions
void add_micro(Arch* arch, DATA_OE data_oe, DATA_IE data_ie, ADDR_OE addr_oe, ADDR_IE addr_ie, ALU_FUN alu_fun, uint8_t ctl);
void new_ins(Arch* arch, char* mnemonic, ARG_TYPE arg, char* desc);
void new_branch(Arch* arch, char* mnemonic, uint8_t bit, uint8_t set, char* desc);

#endif // ARCHITECTURE_H

