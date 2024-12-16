#ifndef TOKENIZER_H
#define TOKENIZER_H

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

int tokenize(const char* file, Token** tokens);

#endif // TOKENIZER_H

