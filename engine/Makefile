CC		:= gcc
CXX		:= g++
C_FLAGS	:= -Wall -Wextra -g -fPIC
CXX_FLAGS := -std=c++17 -Wall -Wextra -g -fPIC

OUTDIR  := build
OBJDIR	:= $(OUTDIR)/obj
BINDIR	:= $(OUTDIR)/bin
INCLDIR	:= $(OUTDIR)/include
BIN		:= libBirdy3d.so
SOURCES := $(wildcard src/*.cpp) $(wildcard src/*/*.cpp) vendor/glad/src/glad.c
HEADERS	:= $(wildcard src/*.hpp) $(wildcard src/*/*.hpp) $(wildcard vendor/**/*.h) $(wildcard vendor/**/include/**/*.h)
INCLUDE	:= -I/usr/include -Ivendor/glad/include -Ivendor/stb_image -Isrc -I/usr/include/freetype2
LIBRARIES	:= -lglfw -lGL -lX11 -lpthread -lXrandr -lXi -ldl -lassimp

OBJECTS := $(patsubst %.cpp,$(OBJDIR)/%.o,$(SOURCES))
OBJECTS := $(patsubst %.c,$(OBJDIR)/%.o,$(OBJECTS))
OUTHEADERS := $(patsubst %,$(INCLDIR)/%,$(HEADERS))

all: $(OUTHEADERS) $(BINDIR)/$(BIN)

clean:
	rm -r $(OUTDIR)

$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXX_FLAGS) $(INCLUDE) $(LIBRARIES) -c $^ -o $@

$(OBJDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(C_FLAGS) $(INCLUDE) $(LIBRARIES) -c $^ -o $@

$(BINDIR)/$(BIN): $(OBJECTS)
	@mkdir -p $(dir $@)
	$(CXX) -shared $^ -o $@

$(INCLDIR)/%.hpp: %.hpp
	@mkdir -p $(dir $@)
	@cp $^ $@

$(INCLDIR)/%.h: %.h
	@mkdir -p $(dir $@)
	@cp $^ $@
