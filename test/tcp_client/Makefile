#LIB = -lssl -lcrypto
LIB =
INCLUDE =

tcp_client: main.o
	g++ $^ $(LIB) -o $@ 

main.o: main.cpp
	g++ -c $< --verbose

.PHONY: clean
clean:
	rm -rf client tcp_client *.o
