CPP = g++
CC = gcc

CFLAGS = -std=c++0x -c -Wall -g
CPPFLAGS = -std=c++0x -c -Wall -g 

LIB =
INCLUDE =

all:udp_server

udp_server: main.o
	$(CPP) $(LIB) $(INCLUDE) $^ -o $@

.c.o:
	$(CC) $(CFLAGS) $(INCLUDE) -c $<

.cpp.o:
	$(CPP) $(CPPFLAGS) $(INCLUDE) -c $<

clean:
	rm -rf *.o cmds/*.o server udp_server

