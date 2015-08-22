CC = gcc
CFLAGS = -Wall -g `pkg-config --cflags --libs gtk+-3.0`
LFLAGS = -lm

FB: FileBrowser.c
	$(CC) FileBrowser.c -o FileBrowser $(CFLAGS) $(LFLAGS)

clean:
	rm -f *.o FileBrowser