#include "architecture.h"

void assemble(void) {

    Token t = tz.tokens[0];
    while (t.type != TOKEN_END) {
        switch (t.type) {
            case TOKEN_MNEMONIC:

        }
    }
}

int main(void) {

    // Initialize architecture
    gen_arch();

    // Read source code
    read_source("example.asm");

    // Tokenize
    tokenize();
}
