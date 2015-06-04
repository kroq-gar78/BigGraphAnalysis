SOURCES := graph.o

CC=gcc
CFLAGS=-fopenmp

all: $(SOURCES)
	$(CC) $(SOURCES) -fopenmp -o ReadGraph

clean:
	rm *.o ReadGraph
