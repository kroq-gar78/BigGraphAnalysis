SOURCES := graph.o

CC=gcc
CFLAGS=-fopenmp -D PARALLEL -lm

all: $(SOURCES)
	$(CC) $(SOURCES) -fopenmp -lm -o ReadGraph

clean:
	rm *.o ReadGraph
