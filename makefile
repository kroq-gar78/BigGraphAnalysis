SOURCES := graph.o

CC=gcc
CFLAGS=-fopenmp -D PARALLEL

all: $(SOURCES)
	$(CC) $(SOURCES) -fopenmp -o ReadGraph

clean:
	rm *.o ReadGraph
