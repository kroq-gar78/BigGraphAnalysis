SOURCES := graph.o simulation.o

CC=gcc
CFLAGS=-fopenmp -D PARALLEL -lm -g

all: $(SOURCES)
	$(CC) $(SOURCES) -fopenmp -lm -g -o ReadGraph

serial: graph_serial.o simulation.o
	$(CC) graph_serial.o simulation.o -lm -g -o ReadGraph_serial

clean:
	rm -f *.o ReadGraph SerialReadGraph
