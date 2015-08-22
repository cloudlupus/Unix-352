CC = gcc
CFLAGS = -g -c -Wall

all: best worst quick 

quick: quickheap352.o
	$(CC) -Wall quickheap352.o HeapTestEngine.o -o quickheap352 

best: bestheap352.o
	$(CC) -Wall bestheap352.o HeapTestEngine.o -o bestheap352

worst: worstheap352.o
	$(CC) -Wall worstheap352.o HeapTestEngine.o -o worstheap352

heap352: heap352.o 
	$(CC) -Wall heap352.o HeapTestEngine.o -o $@

%.o: %.c
	$(CC) $(CFLAGS) $<

clean:
	rm -f heap352.o heap352