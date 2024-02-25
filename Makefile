PROJECT = $(notdir $(CURDIR))
DEBUG = target/debug/$(PROJECT)
RELEASE = target/release/$(PROJECT)
RAYLIB_PATH = raylib/src
RAYGUI_PATH = raygui/src

ifeq ($(OS),Windows_NT)
    PLATFORM_OS=WINDOWS
else
    UNAMEOS=$(shell uname)
    ifeq ($(UNAMEOS),Linux)
        PLATFORM_OS=LINUX
    endif
endif

MAKE = mingw32-make

ifeq ($(PLATFORM_OS),LINUX)
    MAKE = make
endif

all: mkdir debug

mkdir:
	mkdir -p ./target/debug
	mkdir -p ./target/release

SRC = $(wildcard src/*.c)
CC = gcc

FLAGS = -Wall -pipe -lraylib

ifeq ($(PLATFORM_OS),WINDOWS)
    # Libraries for Windows desktop compilation
    # NOTE: WinMM library required to set high-res timer resolution
    FLAGS += -lraylib -lopengl32 -lgdi32 -lwinmm
    # # Required for physac examples
    # FLAGS += -static -lpthread
endif

ifeq ($(PLATFORM_OS),LINUX)
	FLAGS += -lGL -lm -lpthread -ldl -lrt -lX11
endif

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
