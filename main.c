#include "graph.h"
#include "simulation.h"

int main(int argc, char const *argv[]) {

	if (argc != 2) {
		fprintf(stderr, "Usage: %s <graphFile>\n", argv[0]);
		fprintf(stderr, "<graphFile>: Name of graph to test\n");
		exit(1);
	}

	clock_t t1, t2;
	t1 = clock();

	readGraph(argv[1]);

	t1 = clock() - t1; // Done reading
	t2 = clock();

	char *temp = (char *)malloc(strlen(argv[1])+1);
	strcpy(temp, argv[1]);

	char *tok = strtok(temp, "/");

	if ((tok = strtok(NULL, "/")) != NULL) {
		tok = strtok(tok, ".");
	} else {
		tok = strtok((char *)argv[1], ".");
	}

	char choice = 'z';
	printf("[a]nalyze graph or [r]un simulation: ");
	scanf("%c", &choice);

	if (choice == 'a')
		graphStats(tok);
	else
		runSimulation(tok);

	free(temp);
	free(graph);

	t2 = clock() - t2; // Done processing

	printf("\nGraph Read Runtime: %lu clicks (%.3f seconds)\n", t1, (((float)t1)/CLOCKS_PER_SEC));
	printf("Graph Process Runtime: %lu clicks (%.3f seconds)\n", t2, (((float)t2)/CLOCKS_PER_SEC));
	printf("Total Runtime: %lu clicks (%.3f seconds)\n", t1+t2, (((float)t1+t2)/CLOCKS_PER_SEC));

	return 0;
}
