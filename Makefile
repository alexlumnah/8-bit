# Define the compiler
CC = gcc

# Compiler Flags:
CFLAGS = -g -Wall -Wpedantic -Wextra -fsanitize=address,undefined,signed-integer-overflow

SRC = $(wildcard src/*.c)
OBJ = $(SRC:.c=.o)

all: main

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)

main: $(OBJ)
	$(CC) -o microcode $^ $(CFLAGS) $(LDFLAGS)

clean:
	rm microcode $(OBJ)

tidy:
	clang-tidy src/* --

cppcheck:
	cppcheck --enable=portability --check-level=exhaustive --enable=style src/*.c
