#include <stdint.h>

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

typedef enum {
    ARG_NONE,
    ARG_BYTE,
    ARG_ADDR,
} ARG_TYPE;
