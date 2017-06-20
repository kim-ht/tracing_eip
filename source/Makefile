# Makefile
# by kimht

# macros
OBJECTS = base.o tracer.o logger.o
CC = g++
CFLAGS = -Wall -c
OPT = -std=c++11

main: $(OBJECTS)
	$(CC) $(OPT) -Wall main.cpp $(OBJECTS) -o main

base.o: ./base/base.cpp ./base/base.h
	$(CC) $(OPT) $(CFLAGS) ./base/base.cpp

tracer.o: ./tracer/tracer.cpp ./tracer/tracer.h
	$(CC) $(OPT) $(CFLAGS) ./tracer/tracer.cpp

logger.o: ./logger/logger.cpp ./logger/logger.h
	$(CC) $(OPT) $(CFLAGS) ./logger/logger.cpp

clean:
	rm $(OBJECTS) main
