# Makefile
# by kimht

# macros
OBJECTS = base.o tracer.o
CC = g++
CFLAGS = -Wall -c

main: $(OBJECTS)
	$(CC) -Wall main.cpp $(OBJECTS) -o main

base.o: ./base/base.cpp ./base/base.h
	$(CC) $(CFLAGS) ./base/base.cpp

tracer.o: ./tracer/tracer.cpp ./tracer/tracer.h
	$(CC) $(CFLAGS) ./tracer/tracer.cpp

clean:
	rm $(OBJECTS) main
