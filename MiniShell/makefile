CC = gcc
CFLAGS = -g -c -Wall

all: calc listf cli

calc: calc.o
	$(CC) -Wall calc.o -o calc

listf: listf.o
	$(CC) -Wall -g listf.o -o listf

cli: cli.o
	$(CC) -Wall cli.o -o cli

%.o: %.c
	$(CC) $(CFLAGS) $<

clean:
	rm -f *.o listf cli calc