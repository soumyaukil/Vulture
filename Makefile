# Blackhawk makefile

CC = g++
CFLAGS = -std=c++0x -c -g -Wall -O2

EXEC = Vulture
SOURCES = $(wildcard src/*.C)
#SOURCES += $(wildcard src/kraken/*.cpp)
OBJECTS = $(SOURCES:.C=.o)

all: EXEC

EXEC: $(OBJECTS)
	$(CC) $(OBJECTS) -o $(EXEC) -lcrypto -ljansson -lcurl -g
	rm -rf $(OBJECTS)

%.o: %.C
	$(CC) $(CFLAGS) $< -o $@

clearscreen:
	clear

clean:
	rm -rf $(EXEC) $(OBJECTS)
