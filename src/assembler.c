#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "architecture.h"

#define IS_NUMBER(c) ((c) >= '0' && (c) <= '9')
#define IS_LETTER(c) ((c) >= 'A' && (c) <= 'z')
#define IS_ALPHAN(c) (IS_NUMBER(c) || IS_LETTER(c) || (c) == '_')
#define IS_HEXVAL(c) ((c) >= '0' && (c) <= '9') || ((c) >= 'a' && (c) <= 'f')
#define IS_WSPACE(c) ((c) == ' ' || (c) == '\t')
#define IS_CSTART(c) ((c) == ';')
#define IS_NWLINE(c) ((c) == '\n')
#define IS_EOFILE(c) ((c) == 0)
#define IS_LABDEF(c) ((c) == ':')
#define IS_VALIDC(c) (IS_ALPHAN(c) || IS_WSPACE(c) || IS_CSTART(c) || IS_NWLINE(c) || IS_LABDEF(c))

#define HEX2DEC(c) (IS_NUMBER(c) ? (c) - '0' : (c) - 'a' + 10);
#define PRINT_ERR(str) (printf("ERROR Line %ld: " str "\n", tz.line))

#define DEFAULT_TOKEN_CAPACITY (1 << 8)

typedef enum {
    TOKEN_END,
    TOKEN_MNEMONIC,
    TOKEN_LABEL,
    TOKEN_NUMBER,
    TOKEN_STRING,
    TOKEN_STAR,
    TOKEN_COLON,
} TokenType;

typedef struct {
    TokenType type;
    uint32_t val;       // Value of number token
    char* str;          // Pointer to string in source code
    uint32_t len;       // Length of string

    uint32_t line;      // Line in source code
} Token;

typedef struct {
    char* src;      // Source code
    long len;       // Length of source code

    char* curs;     // Current position in source code
    long line;      // Current line in source code

    Token* tokens;  // Array of Tokens
    int capacity;   // Token Array Capacity
    int count;      // Count of Tokens in Array
} Tokenizer;

Tokenizer tz;

void read_source(const char* file) {
    
    // First read entire file into a buffer
    FILE* f;
    f = fopen(file, "r");

    fseek(f, 0, SEEK_END);
    tz.len = ftell(f);
    rewind(f);

    tz.src = calloc(1, tz.len + 1);   // File ends with NUL char
    fread(tz.src, 1, tz.len, f);

    tz.capacity = DEFAULT_TOKEN_CAPACITY;
    tz.tokens = calloc(tz.capacity, sizeof(Token));
}

void print_token(Token t) {

    switch (t.type) {
    case TOKEN_MNEMONIC:
        printf("[MNEMONIC] - %.3s\n", t.str);
        break;
    case TOKEN_LABEL:
        printf("[LABEL] - %.*s\n", t.len, t.str);
        break;
    case TOKEN_NUMBER:
        printf("[NUMBER] - %d\n", t.val);
        break;
    case TOKEN_STRING:
        printf("[STRING] - \"%.*s\"\n", t.len, t.str);
        break;
    case TOKEN_COLON:
        printf("[COLON]\n");
        break;
    case TOKEN_STAR:
        printf("[STAR]\n");
        break;
    case TOKEN_END:
        printf("[END]\n");
        break;
    }
}

void store_token(Token t) {

    // Grow token array if necessary
    if (tz.count == tz.capacity) {
        tz.capacity *= 2;
        tz.tokens = realloc(tz.tokens, tz.capacity * sizeof(Token));
    }

    // Add token to array
    tz.tokens[tz.count] = t;
    tz.count++;

    print_token(t);
}

// Parse up to a 32bit hex value
uint32_t parse_hex(char* str, long len) {

    uint32_t value = 0;
    for (int i = 2; i < len; i++) {
        char c = *(str + i);
        value = value * 16 + HEX2DEC(c);
    }
    return value;
}

// Parse up to a 32bit decimal value
uint32_t parse_dec(char* str, long len) {

    uint32_t value = 0;
    for (int i = 2; i < len; i++) {
        char c = *(str + i);
        value = value * 10 + (c - '0');
    }
    return value;
}

// Fast forward through white space and comments
char* skip_white_space(char* c) {

    // Repeat until no more white space
    while (IS_WSPACE(*c) || IS_CSTART(*c) || IS_NWLINE(*c)) {
        // Skip white space
        while (IS_WSPACE(*c)) c++;
        // Skip comments
        if (IS_CSTART(*c)) while (!IS_NWLINE(*c) && !IS_EOFILE(*c)) c++;
        // Skip newline
        if (IS_NWLINE(*c)) {
            c++;
            tz.line += 1;
            continue;
        }
    }
    return c;
}

// If cursor matches value, increment cursor
bool match(char val) {

    if (*tz.curs != val) return false;

    tz.curs++;
    return true;
}

int tokenize(void) {

    char* c = tz.src;
    while (*c != 0) {

        // Fastforward through white space and comments
        c = skip_white_space(c);

        Token t;
        t.str = c;  // Start of token string

        // Mnemonic or Label
        if (IS_LETTER(*c)) {

            // Seek to end of string
            while (IS_ALPHAN(*c)) c++;
            t.len = c - t.str;

            // Mnemonic
            if (is_mnemonic(t.str, t.len)) {
                t.type = TOKEN_MNEMONIC;
            // Label
            } else {
                t.type = TOKEN_LABEL;
            }
            store_token(t);
            continue;

        // Hex Number
        } else if (strncmp(c, "0x", 2) == 0){

            // Seek to end of hex value
            c += 2;
            while (IS_HEXVAL(*c)) c++;

            t.len = c - t.str;
            if (t.len == 2) {
                PRINT_ERR("Expect value after '0x'");
                return 1;
            }

            // Now parse the hex
            t.type = TOKEN_NUMBER;
            t.val = parse_hex(t.str, t.len);

            store_token(t);
            continue;
        // Decimal Number
        } else if (IS_NUMBER(*c)) {

            // Seek to end of number
            while (IS_NUMBER(*c)) c++;

            t.len = c - t.str;
            t.type = TOKEN_NUMBER;
            t.val = parse_dec(t.str, t.len);

            store_token(t);
            continue;
        } 

        // Single character matching
        switch (*c) {
        // Character
        case '\'':
            // Confirm there is a closing quote
            if (*(c + 2) != '\'') {
                PRINT_ERR("Expect closing ' after char");
                return 1;
            }

            t.len = c - t.str;
            t.type = TOKEN_NUMBER;
            t.val = *(c + 1);
            c += 3;
            break;
        // String
        case '"':
            // Seek to end of string
            c++;
            while (*c != '"' && *c != 0) {
                if (*c == '\n') tz.line++;
                c++;
            }

            if (*c == 0) {
                PRINT_ERR("Reached end of file while parsing string");
                return 1;
            }
            c++;
            t.len = c - t.str;
            t.type = TOKEN_STRING;
            break;
        // Star
        case '*':
            c++;
            t.len = 1;
            t.type = TOKEN_STAR;
            break;
        // Colon
        case ':':
            c++;
            t.len = 1;
            t.type = TOKEN_COLON;
            break;
        // End of File
        case 0:
            t.len = 1;
            t.type = TOKEN_END;
            break;
        default:
            PRINT_ERR("Unexpected character while parsing");
            printf("'%d'\n", (int)*c);
            return 1;
        }
        store_token(t);
    }

    return 0;
}

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
