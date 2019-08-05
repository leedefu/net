CPP = g++
CC = gcc

CFLAGS = -std=c++0x -c -Wall -g
CPPFLAGS = -std=c++0x -c -Wall -g 

LIB =
INCLUDE =

all:daytime

daytime: main.o
	$(CPP) $(LIB) $(INCLUDE) $^ -o $@

.c.o:
	$(CC) $(CFLAGS) $(INCLUDE) -c $<

.cpp.o:
	$(CPP) $(CPPFLAGS) $(INCLUDE) -c $<

clean:
	rm -rf *.o daytime
