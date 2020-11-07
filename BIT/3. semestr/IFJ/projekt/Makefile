CC=gcc
CFLAGS=-std=c99 -Wall -Wextra -pedantic -g

all: ifj2017 main

ifj2017: main
	cp main ifj2017

pack:
	zip xborko02 *.c *.h Makefile dokumentace.pdf rozdeleni

main: main.o ast.o generator.o list.o precedence.o recursive_descent.o scanner.o semantic.o stack.o symtable.o garbage.o
	$(CC) $^ -o $@ $(CFLAGS)

main.o: main.c
	$(CC) -c $^ -o $@ $(CFLAGS)

ast.o: ast.c
	$(CC) -c $^ -o $@ $(CFLAGS)

generator.o: generator.c
	$(CC) -c $^ -o $@ $(CFLAGS)

list.o: list.c
	$(CC) -c $^ -o $@ $(CFLAGS)

precedence.o: precedence.c
	$(CC) -c $^ -o $@ $(CFLAGS)

recursive_descent.o: recursive_descent.c
	$(CC) -c $^ -o $@ $(CFLAGS)

scanner.o: scanner.c
	$(CC) -c $^ -o $@ $(CFLAGS)

semantic.o: semantic.c
	$(CC) -c $^ -o $@ $(CFLAGS)

stack.o: stack.c
	$(CC) -c $^ -o $@ $(CFLAGS)

symtable.o: symtable.c
	$(CC) -c $^ -o $@ $(CFLAGS)

garbage.o: garbage.c
	$(CC) -c $^ -o $@ $(CFLAGS)

clean:
	rm -f ifj2017 main *.o xborko02.zip
