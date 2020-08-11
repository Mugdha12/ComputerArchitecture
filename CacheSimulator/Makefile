CC=gcc
CFLAGS=-pipe -ggdb -Wall

all: cacheSimulator.o
	$(CC) $(CFLAGS) cacheSimulator.o -o cacheSimulator
simulator.o:  cacheSimulator.c
	$(CC) $(CFLAGS) -c cacheSimulator.c
clean:
	rm cacheSimulator *.o *.*~ *~
