CC=g++
CFLAGS = -pedantic -std=c++11 -static-libstdc++
LOGIN = xbrhel04
FILES = Makefile ipk-client.cpp parameters_c.cpp parameters_c.h client_side.cpp client_side.h ipk-server.cpp parameters_s.cpp parameters_s.h server_side.cpp server_side.h

all: ipk-client ipk-server

ipk-client: ipk-client.cpp parameters_c.cpp parameters_c.h client_side.cpp client_side.h
	g++ $(CFLAGS) -o $@ ipk-client.cpp parameters_c.cpp parameters_c.h client_side.cpp client_side.h

ipk-server: ipk-server.cpp parameters_s.cpp parameters_s.h server_side.cpp server_side.h
	g++ $(CFLAGS) -o $@ ipk-server.cpp parameters_s.cpp parameters_s.h server_side.cpp server_side.h

zip: clean
	zip $(LOGIN).tar $(FILES)

clean:
	rm -f ipk-client ipk-server *.o
