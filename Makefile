CC		:= g++
C_FLAGS := -std=c++17 -Wall -Wextra -g

BIN		:= bin
SOURCES = $(wildcard src/*.cpp) $(wildcard src/*.c) $(wildcard src/*/*.cpp)
INCLUDE	:= include -I/usr/include
LIB		:= lib

LIBRARIES	:= -lglfw -lGL -lX11 -lpthread -lXrandr -lXi -ldl -lassimp

ifeq ($(OS),Windows_NT)
EXECUTABLE	:= main.exe
else
EXECUTABLE	:= main
endif

all: $(BIN)/$(EXECUTABLE)

clean:
	$(RM) $(BIN)/$(EXECUTABLE)

run: all
	./$(BIN)/$(EXECUTABLE)

$(BIN)/$(EXECUTABLE): $(SOURCES)
	$(CC) $(C_FLAGS) -I$(INCLUDE) -L$(LIB) $^ -o $@ $(LIBRARIES)