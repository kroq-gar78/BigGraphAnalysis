SOURCES := main.o graph.o simulation.o

CC=gcc
CFLAGS=-fopenmp -D PARALLEL -lm -g

all: $(SOURCES)
	$(CC) $(SOURCES) -fopenmp -lm -g -o ReadGraph

serial: graph_serial.o simulation.o
	$(CC) graph_serial.o simulation.o -lm -g -o SerialReadGraph

tests: graph.o simulation.o graph_check.o
	$(CC) graph.o simulation.o graph_check.o $(CFLAGS) -lcheck -o tests
	./tests
	rm tests

clean:
	rm -f *.o ReadGraph SerialReadGraph tests
