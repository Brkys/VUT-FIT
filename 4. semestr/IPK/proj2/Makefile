CC=g++
CFLAGS = -Wall -static-libstdc++ -Wextra -pedantic -std=c++11
LOGIN = xbrhel04
FILES = Makefile ipklib.h ipklib.cpp ipklookup.cpp

all: ipk-lookup ipklib.h

ipk-lookup: ipklookup.cpp
	$(CC) $(CFLAGS) -o $@ ipklookup.cpp ipklib.h ipklib.cpp

zip: clean
	zip $(LOGIN).tar $(FILES)

clean:
	rm -f ipk-lookup *.o
