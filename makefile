SOURCES := graph.o simulation.o csparse.o

CC=gcc
CFLAGS=-fopenmp -D PARALLEL -lm -g

all: $(SOURCES)
	$(CC) $(SOURCES) $(CFLAGS) -o ReadGraph

serial: graph_serial.o simulation.o
	$(CC) graph_serial.o simulation.o -lm -g -o SerialReadGraph

clean:
	rm -f *.o ReadGraph SerialReadGraph
