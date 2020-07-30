#LIB = -lssl -lcrypto
LIB =
INCLUDE =

tcp_servere: main.o
	g++ $^ $(LIB) -o $@ 

main.o: main.cpp
	g++ -c $< --verbose

.PHONY: clean
clean:
	rm -rf tcp_servere client *.o
