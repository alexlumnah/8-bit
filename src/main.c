#include <stdio.h>
#include <assert.h>

#include "architecture.h"

int main(void) {

    Arch* arch = generate_architecture();

    // Dump out microcode
    char* arg_strings[] = {"      ", "<BYTE>", "<ADDR>", "<PNTR>"};
    for (int i = 0; i < arch->count; i++) {
        Inst inst = arch->insts[i];
        printf("%02x: %s %s - %s\n", inst.opcode, inst.mnemonic, arg_strings[inst.arg_type], inst.desc);
    }

    // Dump microcode out to a file
    FILE *f = fopen("outputs/microcode.bin", "wb");
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

    // Dump opcodes out to file for assembler
    f = fopen("outputs/instructions.txt", "w");
    assert(f != NULL);
    char* arg_types[] = {"NONE", "BYTE", "ADDR", "PNTR"};
    for (int i = 0; i < arch->count; i++) {
        Inst inst = arch->insts[i];
        fprintf(f, "%02x, %s, %s, %s\n", inst.opcode, inst.mnemonic, arg_types[inst.arg_type], inst.desc);
    }
    fclose(f);

}
