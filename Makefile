# Globales
CC := g++
CFLAGS := -Wall
MODULE_NAME := ./build/main.out
SOURCE_FILES := $(shell find . -type f -iname "*.cpp" -exec basename {} \;)
OBJECT_FILES := $(SOURCE_FILES:%.cpp=./build/%.o)
LINKS := -lsfml-graphics -lsfml-window -lsfml-system

### Reglas ###

build: makefolder $(MODULE_NAME)

debug: CFLAGS := $(CFLAGS) -g
debug: makeshared_debug build

rebuild: clean build

run: build
	$(MODULE_NAME)

$(MODULE_NAME): $(OBJECT_FILES)
	$(CC) $(CFLAGS) -o $@ $^ $(LINKS)

./build/%.o: ./%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

makefolder:
	if [ ! -d ./build ]; then mkdir build; fi

# Clean

clean:
	rm -rf build
