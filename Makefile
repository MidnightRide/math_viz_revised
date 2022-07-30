CC = gcc
CFLAGS = -std=c11 -g -Wall -Wextra -Wno-pedantic -Wstrict-aliasing -Wno-visibility -Wno-unused-parameter -Wno-char-subscripts -Wno-format -Wno-sign-compare
CFLAGS += -D_DEBUG -D_CRT_SECURE_NO_WARNINGS
CFLAGS += -I/media/terabyte/lib/freetype/include -I/media/terabyte/lib/cglm/include -I/media/terabyte/lib/glad/include -I/media/terabyte/lib/glfw/include -I/media/terabyte/lib/stb 
LDFLAGS = /media/terabyte/lib/glad/src/glad.o /media/terabyte/lib/freetype/build/libfreetype.a /media/terabyte/lib/cglm/libcglm.a /media/terabyte/lib/glfw/src/libglfw3.a -lbz2 -lm -lpng16 -lz -lharfbuzz -lbrotlidec

SRC  = $(wildcard src/**/*.c) $(wildcard src/*.c) $(wildcard src/**/**/*.c) $(wildcard src/**/**/**/*.c)
OBJ  = $(SRC:.c=.o)
BIN  = bin

.PHONY = all clean

all: dirs game

dirs:
	mkdir -p ./$(BIN)

run: all
	$(BIN)/main

game: $(OBJ)
	$(CC) -o $(BIN)/main $^ $(LDFLAGS)

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)

clean:
	rm -rf $(BIN) $(OBJ)
