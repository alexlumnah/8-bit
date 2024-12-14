# Define the compiler
CC = gcc

# Compiler Flags:
CFLAGS = -g -Wall -Wpedantic -Wextra -fsanitize=address,undefined,signed-integer-overflow

SRC = $(wildcard src/*.c)
OBJ = $(SRC:.c=.o)

all: main assembler

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)

main: src/main.o src/architecture.o
	$(CC) -o architecture $^ $(CFLAGS) $(LDFLAGS)

assembler: src/assembler.o src/architecture.o
	$(CC) -o assembler $^ $(CFLAGS) $(LDFLAGS)

clean:
	rm architecture assembler $(OBJ)

tidy:
	clang-tidy src/* --

cppcheck:
	cppcheck --enable=portability --check-level=exhaustive --enable=style src/*.c
