# Makefile
# by kimht

# macros
OBJECTS = base.o tracer.o logger.o disassembler.o
CC = g++
CFLAGS = -Wall -c
OPT = -std=c++11
LIBRARY = -l capstone

main: $(OBJECTS)
	$(CC) $(OPT) -O3 -Wall main.cpp $(OBJECTS) $(LIBRARY) -o main

base.o: ./base/base.cpp ./base/base.h
	$(CC) $(OPT) $(CFLAGS) ./base/base.cpp

tracer.o: ./tracer/tracer.cpp ./tracer/tracer.h
	$(CC) $(OPT) $(CFLAGS) ./tracer/tracer.cpp

logger.o: ./logger/logger.cpp ./logger/logger.h
	$(CC) $(OPT) $(CFLAGS) ./logger/logger.cpp

disassembler.o: ./disassembler/disassembler.cpp ./disassembler/disassembler.h
	$(CC) $(OPT) $(CFLAGS) ./disassembler/disassembler.cpp

clean:
	rm $(OBJECTS) main
