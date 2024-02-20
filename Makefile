PROJECT = $(notdir $(CURDIR))
DEBUG = target/debug/$(PROJECT)
RELEASE = target/release/$(PROJECT)
RAYLIB_PATH = raylib/src
RAYGUI_PATH = raygui/src

all: mkdir debug

mkdir:
	mkdir -p ./target/debug
	mkdir -p ./target/release

SRC = $(wildcard src/*.c)
CC = gcc
FLAGS = -Wall -pipe -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
INCLUDE_PATHS = -I$(RAYLIB_PATH) -I$(RAYGUI_PATH)
LDFLAGS = -L$(RAYLIB_PATH)

debug: OUTPUT = $(DEBUG)
debug: FLAGS += -g
debug: executable

release: OUTPUT = $(RELEASE)
release: FLAGS += -O2
release: executable

executable: $(SRC)
	$(CC) $(SRC) -o $(OUTPUT) $(FLAGS) $(INCLUDE_PATHS) $(LDFLAGS) -DPLATFORM_DESKTOP 

.PHONY: run clean install uninstall

# write "make run a="..." for commandline arguments"
run:
	./$(DEBUG) $(a)

clean:
	rm -f $(DEBUG) $(RELEASE)

# installs from release folder only
install:
	ln -s $(CURDIR)/$(RELEASE) ~/.local/bin/

uninstall:
	rm -f ~/.local/bin/$(PROJECT)
