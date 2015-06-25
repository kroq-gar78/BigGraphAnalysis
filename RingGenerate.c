#include <stdio.h>
#include <stdlib.h>

int main(int argc, char const *argv[]) {
	if (argc < 2 || argc > 3) {
		fprintf(stderr, "Usage: %s <numNodes> [outputfile]\n", argv[0]);
		fprintf(stderr, "<numNodes>: The number of nodes to generate.\n");
		fprintf(stderr, "[outputfile]: Where to save the graph. If not present, print to stdout.\n");
		return 1;
	}
	//if (argc == 2) bool use_stdout = false;
	int use_stdout = (argc == 2);

	int numNodes = atoi(argv[1]);

	if (numNodes < 2) {
		fprintf(stderr, "Enter a graph size greater than 1\n");
		return 1;
	}

	FILE *f = stdout;
	if(!use_stdout) { f = fopen(argv[2], "w"); }
	if (!f) {
		fprintf(stderr, "Could not open file %s\n", argv[2]);
		return 1;
	}

	int i;
	for (i = 1; i < numNodes; i++) {
		fprintf(f, "%d %d 1\n", i, i+1);
	}
	fprintf(f, "%d 1 1\n", numNodes);
	
	if (!use_stdout) {
		fclose(f);
		printf("Graph written to \"%s\"\n", argv[2]);
	}

	return 0;
}
