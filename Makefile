#
# Copyright (c) 2021 jdeokkim
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#

.PHONY: all clean

_COLOR_BEGIN := $(shell tput setaf 10)
_COLOR_END := $(shell tput sgr0)

FEROX_PATH ?= $(PROJECT_PATH)/lib/ferox
RAYLIB_PATH ?= $(PROJECT_PATH)/lib/raylib
RAYGUI_PATH ?= $(PROJECT_PATH)/lib/raygui

# TODO: Change the values of `PROJECT_NAME` and `PROJECT_FULL_NAME`
PROJECT_NAME := rbedit
PROJECT_FULL_NAME := c-krit/rbedit

PROJECT_PATH := $(PROJECT_NAME)
PROJECT_PREFIX := $(_COLOR_BEGIN)$(PROJECT_FULL_NAME):$(_COLOR_END)

BINARY_PATH := $(PROJECT_PATH)/bin

INCLUDE_PATH := \
	$(PROJECT_PATH)/include \
	$(RAYLIB_PATH)/src      \
	$(RAYGUI_PATH)/src

LIBRARY_PATH := \
	$(PROJECT_PATH)/lib \
	$(RAYLIB_PATH)/src

RESOURCE_PATH := $(PROJECT_PATH)/res
SOURCE_PATH := $(PROJECT_PATH)/src

INCLUDE_PATH += $(SOURCE_PATH)/external

SOURCES := \
	$(SOURCE_PATH)/loader.c  \
	$(SOURCE_PATH)/physics.c \
	$(SOURCE_PATH)/screen.c  \
	$(SOURCE_PATH)/utils.c   \
	$(SOURCE_PATH)/main.c

OBJECTS := $(SOURCES:.c=.o)

# TODO: Edit the line below if you want another name for your executable
TARGETS := $(BINARY_PATH)/$(PROJECT_PATH)

HOST_PLATFORM := LINUX

ifeq ($(OS),Windows_NT)
	PROJECT_PREFIX := $(PROJECT_NAME):
	HOST_PLATFORM := WINDOWS
else
	UNAME = $(shell uname)

	ifeq ($(UNAME),Linux)
		HOST_PLATFORM = LINUX
	endif
endif

CC := gcc
CFLAGS := -D_DEFAULT_SOURCE -g $(INCLUDE_PATH:%=-I%) -O2 -std=gnu99 -Wno-unused-result
LDFLAGS := $(LIBRARY_PATH:%=-L%)
LDLIBS := -lferox -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

CFLAGS += -I$(FEROX_PATH)/ferox/include
LDFLAGS += -L$(FEROX_PATH)/ferox/lib

PLATFORM := $(HOST_PLATFORM)

ifeq ($(PLATFORM),WINDOWS)
	TARGETS := $(BINARY_PATH)/$(PROJECT_PATH).exe

	ifneq ($(HOST_PLATFORM),WINDOWS)
		CC := x86_64-w64-mingw32-gcc
	endif

	LDLIBS := -lferox -lraylib -lopengl32 -lgdi32 -lwinmm -lpthread
else ifeq ($(PLATFORM),WEB)
	TARGETS := $(BINARY_PATH)/$(PROJECT_PATH).html

	CC := emcc
	CFLAGS += -Wno-unused-command-line-argument

# https://github.com/emscripten-core/emscripten/blob/main/src/settings.js
	WEBFLAGS := -s ASYNCIFY -s FORCE_FILESYSTEM -s INITIAL_MEMORY=67108864 -s USE_GLFW=3
	WEBFLAGS += --preload-file $(RESOURCE_PATH) --shell-file $(RESOURCE_PATH)/html/shell.html
endif

all: pre-build build post-build

pre-build:
	@echo "$(PROJECT_PREFIX) Using: '$(CC)' to build this project."
    
build: $(TARGETS)

$(SOURCE_PATH)/%.o: $(SOURCE_PATH)/%.c
	@echo "$(PROJECT_PREFIX) Compiling: $@ (from $<)"
	@$(CC) -c $< -o $@ $(CFLAGS) $(LDFLAGS) $(LDLIBS)
    
$(TARGETS): $(OBJECTS)
	@mkdir -p $(BINARY_PATH)
	@echo "$(PROJECT_PREFIX) Linking: $(TARGETS)"
	@$(CC) $(OBJECTS) -o $(TARGETS) $(CFLAGS) $(LDFLAGS) $(LDLIBS) $(WEBFLAGS)
    
post-build:
	@echo "$(PROJECT_PREFIX) Build complete."

clean:
	@echo "$(PROJECT_PREFIX) Cleaning up."
	@rm -rf $(BINARY_PATH)/*
	@rm -rf $(SOURCE_PATH)/*.o