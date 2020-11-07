CC=gcc
CFLAGS=-std=gnu99 -Wall -Wextra -Werror -pedantic

all: proj2

proj2: proj2.o
	$(CC) $< $(CFLAGS) -o $@ -pthread

proj2.o: proj2.c
	$(CC) $< $(CFLAGS) -c -o $@

clean:
	rm -f proj2 proj2.o proj2.out
