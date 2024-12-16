#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "architecture.h"
#include "tokenizer.h"

#define MAX_ADDR_VAL ((1 << 16) - 1)
#define MAX_BYTE_VAL ((1 << 8)  - 1)
#define DEFAULT_LABEL_CAPACITY (256)

#define MIN(x, y) ((x) < (y) ? (x) : (y))
#define PRINT_ERR(str) (printf("ERROR Line %d: " str "\n", t.line))

typedef struct {
    const char* str;    // Label string
    uint16_t len;       // Length of string
    uint16_t addr;      // Address of label def, or ref
    uint16_t line;      // Line label was found on (for error reporting)
} Label;

typedef struct {
    Token* tokens;
    int count;
    int curr;

    uint8_t code[MAX_ADDR_VAL + 1];
    uint16_t i;

    // Label definitions
    Label* label_defs;
    int def_count;
    int def_capacity;

    // Label references
    Label* label_refs;
    int ref_count;
    int ref_capacity;
} Assembler;

Assembler a;

// Initialize assembler struct
void init_assembler(void) {

    a.def_count = 0;
    a.def_capacity = DEFAULT_LABEL_CAPACITY;
    a.label_defs = calloc(a.def_capacity, sizeof(Label));

    a.ref_count = 0;
    a.ref_capacity = DEFAULT_LABEL_CAPACITY;
    a.label_refs = calloc(a.def_capacity, sizeof(Label));
}

// Get current token, increment iterator
Token get_token(void) {
    return a.tokens[a.curr++];
}

// Skip current token, return next token and increment
Token skip_token(void) {
    a.curr++;
    return a.tokens[a.curr++];
}

// Peek at current token, don't increment iterator
Token peek_token(void) {
    return a.tokens[a.curr];
}

// Capture label reference for later filling in
void capture_ref(Token t) {

    if (a.ref_count == a.ref_capacity) {
        a.ref_capacity *= 2;
        a.label_refs = realloc(a.label_refs,a.ref_capacity * sizeof(Label));
    }

    // Capture reference
    a.label_refs[a.ref_count] = (Label){t.str, t.len, a.i, t.line};
    a.ref_count++;
}

// Define label
void define_label(Token t) {

    if (a.def_count == a.def_capacity) {
        a.def_capacity *= 2;
        a.label_defs = realloc(a.label_defs,a.def_capacity * sizeof(Label));
    }

    // Capture definition
    a.label_defs[a.def_count] = (Label){t.str, t.len, a.i, t.line};
    a.def_count++;

}

// Write byte to compiled code
void write_byte(uint8_t byte) {
    a.code[a.i++] = byte;
}

// Parse mnemonic
void parse_mnemonic(void) {

    Token t = get_token();
    Token n = peek_token();
    uint8_t opcode;

    // Pointer argument
    if (n.type == TOKEN_STAR && ins_exists(t.str, ARG_PNTR)) {
        opcode = get_opcode(t.str, ARG_PNTR); 
        n = skip_token();
        if (n.type == TOKEN_LABEL) {
            write_byte(opcode);
            capture_ref(n);
            write_byte(0);
            write_byte(0);
            return;
        }
        if (n.val > MAX_ADDR_VAL)
            PRINT_ERR("Address larger than 16bit address space");
        write_byte(opcode);
        write_byte(n.val >> 8);
        write_byte(n.val);
    } else if (n.type == TOKEN_LABEL && ins_exists(t.str, ARG_ADDR)) {
        opcode = get_opcode(t.str, ARG_ADDR); 
        write_byte(opcode);
        // Get label, and capture a reference to it
        n = get_token();
        capture_ref(n);
        write_byte(0);
        write_byte(0);
    } else if (n.type == TOKEN_NUMBER && ins_exists(t.str, ARG_ADDR)) {
        opcode = get_opcode(t.str, ARG_ADDR); 
        n = get_token();
        if (n.val > MAX_ADDR_VAL)
            PRINT_ERR("Address larger than 16 bits");
        write_byte(opcode);
        write_byte(n.val >> 8);
        write_byte(n.val);
    } else if (n.type == TOKEN_NUMBER && ins_exists(t.str, ARG_BYTE)) {
        opcode = get_opcode(t.str, ARG_BYTE); 
        n = get_token();
        if (n.val > MAX_BYTE_VAL)
            PRINT_ERR("Argument larger than 8 bits");
        write_byte(opcode);
        write_byte(n.val);
    } else if (ins_exists(t.str, ARG_NONE)) {
        opcode = get_opcode(t.str, ARG_NONE);
        write_byte(opcode);
    } else {
        PRINT_ERR("Mnemonic missing argument");
    }
}

void parse_label(void) {

    Token t = get_token();
    Token n = get_token();
    if (n.type != TOKEN_COLON) {
        PRINT_ERR("Expected label definition");
    }
    define_label(t);
}

void parse_number(void) {

    Token t = get_token();
    if (t.val > MAX_BYTE_VAL) {
        PRINT_ERR("Number larger than a single byte");
    }
    write_byte(t.val);
}

void parse_string(void) {

    Token t = get_token();
    for (uint16_t i = 1; i < t.len - 1; i++) write_byte(t.str[i]);
    write_byte(0);  // Null-terminate the string
}

// Lookup label definition, place address in addr pointer if it exists
bool lookup_label_def(Label ref, uint16_t* addr) {

    for (uint16_t i = 0; i < a.def_count; i++) {
        Label def = a.label_defs[i];
        if (strncmp(ref.str, def.str, MIN(ref.len, def.len)) == 0) {
            *addr = def.addr;
            return true;
        }
    }
    return false;
}

void resolve_labels(void) {

    uint16_t def_addr;
    for (uint16_t i = 0; i < a.ref_count; i++) {
        Label ref = a.label_refs[i];
        if (lookup_label_def(ref, &def_addr)) {
            a.code[ref.addr] = (uint8_t)(def_addr >> 8);
            a.code[ref.addr + 1] = (uint8_t)def_addr;
        } else {
            Token t = peek_token(); // Used for error reporting
            PRINT_ERR("Label not defined");
        }
    }
}

void assemble(Token* tokens, int count) {

    a.tokens = tokens;
    a.count = count;

    // TODO: Only allow random bytes and strings in a data section of file
    Token t = peek_token();
    while (t.type != TOKEN_END) {
        switch (t.type) {
        case TOKEN_MNEMONIC:
            parse_mnemonic();
            break;
        case TOKEN_LABEL:
            parse_label();
            break;
        case TOKEN_NUMBER:
            parse_number();
            break;
        case TOKEN_STRING:
            parse_string();
            break;
        default:
            PRINT_ERR("Unexpected token while parsing");
        }
        t = peek_token();
    }

    resolve_labels();
}

int main(int argc, const char** argv) {

    const char* filename = "example.asm";
    if (argc == 2) {
        filename = argv[1];
    }

    // Initialize architecture
    generate_architecture();

    // Tokenize
    Token* tokens;
    int count;
    count = tokenize(filename, &tokens);

    if (count == 0) return 1;

    init_assembler();
    assemble(tokens, count);

    // Dump code
    for (int i = 0; i < a.i; i++) {

        printf("%02x ", a.code[i]);
        if (i % 16 == 15) printf("\n");
    }

}
