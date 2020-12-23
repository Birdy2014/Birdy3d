CXX		:= g++
C_FLAGS	:= -Wall -Wextra -g
CXX_FLAGS := -std=c++17 -Wall -Wextra -g

OUTDIR  := build
BIN		:= sandbox
ENGINEDIR	:= ../engine
SOURCES := $(wildcard src/*.cpp) $(wildcard src/*/*.cpp)
SHADERS	:= $(wildcard $(ENGINEDIR)/src/shaders/*)
INCLUDE	:= -I/usr/include -I$(ENGINEDIR)/build/include/src -I$(ENGINEDIR)/build/include/vendor/stb_image -I$(ENGINEDIR)/build/include/vendor/glad/include -I/usr/include/freetype2
LIB		:= -Wl,-rpath,'$$ORIGIN/bin' -L$(OUTDIR)/bin
LIBRARIES	:= -lglfw -lassimp -lBirdy3d -lfreetype
OUTSHADERS	:= $(notdir $(SHADERS))
OUTSHADERS	:= $(addprefix $(OUTDIR)/shaders/,$(OUTSHADERS))

.PHONY: all run clean $(OUTDIR)/bin/libBirdy3d.so

all: $(OUTSHADERS) $(OUTDIR)/ressources $(OUTDIR)/$(BIN)

run: all
	cd build && ./$(BIN)

clean:
	rm -r $(OUTDIR)
	make -C $(ENGINEDIR) -f $(ENGINEDIR)/Makefile clean

$(OUTDIR)/bin/libBirdy3d.so:
	make -C $(ENGINEDIR) -f $(ENGINEDIR)/Makefile
	@mkdir -p $(dir $@)
	@cp $(ENGINEDIR)/build/bin/libBirdy3d.so $@

$(OUTDIR)/$(BIN): $(SOURCES) $(OUTDIR)/bin/libBirdy3d.so
	@mkdir -p $(dir $@)
	$(CXX) $(CXX_FLAGS) $(INCLUDE) $(LIB) $(LIBRARIES) $(SOURCES) -o $@

$(OUTSHADERS): $(SHADERS)
	@echo $^
	@mkdir -p $(dir $@)
	@cp $^ $(dir $@)

$(OUTDIR)/ressources: ressources
	@mkdir -p $(dir $@)
	@cp -r $^ $@
