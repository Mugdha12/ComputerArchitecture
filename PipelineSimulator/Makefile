CC=gcc
CFLAGS=-pipe -ggdb -Wall

all: pipelineSimulator.o
	$(CC) $(CFLAGS) pipelineSimulator.o -o pipelineSimulator
simulator.o:  pipelineSimulator.c
	$(CC) $(CFLAGS) -c pipelineSimulator.c
clean:
	rm pipelineSimulator *.o *.*~ *~
