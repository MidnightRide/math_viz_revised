CC = clang
CFLAGS = -std=c11 -MD -g -O0 -Wall -Wextra -Wpedantic -Wstrict-aliasing -Wno-visibility -Wno-pedantic -Wno-write-strings
CFLAGS += -Wno-pointer-arith -Wno-newline-eof -Wno-unused-parameter -Wno-gnu-statement-expression -Wno-static-in-inline
CFLAGS += -Wno-gnu-compound-literal-initializer -Wno-gnu-zero-variadic-macro-arguments -Wno-missing-braces 
CFLAGS += -Wno-deprecated-declarations -Wno-zero-length-array -Wno-language-extension-token -Wno-char-subscripts	
CFLAGS += -D_DEBUG -D_CRT_SECURE_NO_WARNINGS 
CFLAGS += -ID:/lib/cglm/include -ID:/lib/glad/include -ID:/lib/glfw/include -ID:/lib/stb -ID:\lib\freetype\include
LDFLAGS = D:/lib/glad/src/glad.o D:/lib/cglm/build/Release/cglm.lib D:/lib/glfw/lib-vc2022/glfw3dll.lib 
LDFLAGS += D:\lib\freetype\objs\x64\Release\freetype.lib

SRC  = $(wildcard D:/development/math_visualiser_revised/src/**/*.c) 
SRC += $(wildcard D:/development/math_visualiser_revised/src/*.c) 
SRC += $(wildcard D:/development/math_visualiser_revised/src/**/**/*.c) 
SRC += $(wildcard D:/development/math_visualiser_revised/src/**/**/**/*.c)
OBJ = $(SRC:.c=.o)
BIN = bin

.PHONY: all clean

all: dirs game clean

dirs:
	@echo Creating directories...
	@IF exist "$(BIN)/" (@echo Directory "$(BIN)" Already Exists!) else (mkdir $(BIN))
	@echo Done.

run: all
	@echo $(OBJ)
	@echo Running game...
	@$(BIN)/main.exe

game: $(OBJ)
	@echo Linking game...
	$(CC) -o $(BIN)/main.exe -g $^ $(LDFLAGS)
	@echo Done.

%.o: %.c
	@echo $<...
	@$(CC) -o $@ -c $< $(CFLAGS)

clean:
	@echo Deleting object files...
	@del /s src\*.o
	@del /s src\*.d
	@echo Done.
