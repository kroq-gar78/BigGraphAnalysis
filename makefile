SOURCES := graph.o simulation.o

CC=gcc
CFLAGS=-fopenmp -D PARALLEL -lm -g

all: $(SOURCES)
	$(CC) $(SOURCES) -fopenmp -lm -g -o ReadGraph

clean:
	rm -f *.o ReadGraph
