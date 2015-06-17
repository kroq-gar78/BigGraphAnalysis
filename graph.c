#include "graph.h"
#include "simulation.h"

#define MAX_DATA_POINTS 100

int rank;

void connectNode(int src, int dest) {
	if (graph[src] == NULL) {
		Node *head = (Node *)malloc(sizeof(Node));
		head->vertexNum = src;
		head->isInfected = false;
		graph[src] = head;
	}

	Node *temp = graph[src];
	while (temp->next != NULL)
		temp = temp->next;

	Node *newNode = (Node *)malloc(sizeof(Node));
	newNode->vertexNum = dest;
	newNode->isInfected = false;
	temp->next = newNode;

	if (graph[dest] == NULL) {
		Node *head = (Node *)malloc(sizeof(Node));
		head->vertexNum = dest;
		head->isInfected = false;
		graph[dest] = head;
	}

	temp = graph[dest];
	while (temp->next != NULL)
		temp = temp->next;

	newNode = (Node *)malloc(sizeof(Node));
	newNode->vertexNum = src;
	newNode->isInfected = false;
	temp->next = newNode;

	edgeCount++;
}

int countDegree(Node *node) {
	int count = 0;
	Node *temp = node;
	if (temp != NULL) {
		while (temp->next != NULL) {
			count++;
			temp = temp->next;
		}
	}

	return count;
}

void writeDegreeDistribution(int higestDegNum, int lowestDegNum, 
	int avgDegree, char *filename) {
	int numDataPoints = highestNode;
	int step = 1;

	FILE *f = fopen("web/data.js", "w");

	if (!f) {
		fprintf(stderr, "Unable to open output file\n");
		exit(-1);
	}

	printf("Writing degree distribution data...\n");

	int *distribuion = (int *)malloc(sizeof(int)*higestDegNum+1);
	int chunk = higestDegNum / 10;
	int i;

#pragma omp parallel shared(higestDegNum, distribuion, chunk), private(i)
{
	#pragma omp for schedule(dynamic,chunk) nowait
	for (i = 0; i <= higestDegNum; i++)
		distribuion[i] = 0;

	#pragma omp for schedule(dynamic, chunk)
	for (i = 0; i < highestNode; i++) {
		int degree = countDegree(graph[i]);

		if (degree > 0) {
			#pragma omp atomic
			distribuion[degree] += 1;
		}
	}
}

	if (higestDegNum > MAX_DATA_POINTS) {
		numDataPoints = highestNode;
		step = higestDegNum / MAX_DATA_POINTS;
	}

	long variance = 0;

	// standard deviation
#pragma omp parallel shared(variance, avgDegree), private(i)
{
	#pragma omp for reduction(+: variance)
	for (i = 0; i < highestNode; i++) {
		int degree = countDegree(graph[i]);
		int val = degree - avgDegree;
		val *= val;
		variance += val;
	}
}
	
	variance /= highestNode;
	double standardDev = sqrt(variance);
	printf("Standard deviation is: %lf\n", standardDev);

	fprintf(f, "var data = {\n");
	fprintf(f, "\t\"name\": \"%s\",\n", filename);
	fprintf(f, "\t\"nodeCount\": %d,\n", highestNode);
	fprintf(f, "\t\"edgeCount\": %d,\n", edgeCount);
	fprintf(f, "\t\"highestDeg\": %d,\n", higestDegNum);
	fprintf(f, "\t\"lowestDeg\": %d,\n", lowestDegNum);
	fprintf(f, "\t\"avgDeg\": %d,\n", avgDegree);
	fprintf(f, "\t\"standardDev\": %lf,\n", standardDev);
	fprintf(f, "\t\"values\": [\n");

	for (i = 0; i <= higestDegNum; i++) {
		if (i > 0 && distribuion[i] == 0) {
			continue;
		}

		fprintf(f, "\t\t{\"x\": %d, \"y\": %d},\n", i, distribuion[i]);
	}

	fprintf(f, "\t]\n");
	fprintf(f, "}\n");

	fclose(f);
	free(distribuion);

	printf("Data written as \"web/data.js\"\n");

}

void degreeStats(char *filename) {
	long averageDegree = 0;
	int lowestDegree = INT_MAX, highestDegree = 0, degree;
	int i;

	int higestDegNum, lowestDegNum;

	// parallel
	int chunk = highestNode / 10;

#pragma omp parallel shared(lowestDegree, highestDegree, degree, chunk, higestDegNum, lowestDegNum), private(i)
{

	#pragma omp for schedule(dynamic,chunk) nowait
	for (i = 0; i < highestNode; i++) {
		if (graph[i] != NULL) {
			degree = countDegree(graph[i]);

			if (degree > highestDegree) {
				highestDegree = degree;
				higestDegNum = i;
			}
			if (degree < lowestDegree) {
				lowestDegree = degree;
				lowestDegNum = i;
			}

			averageDegree += degree;
		}
	}

}

	averageDegree /= highestNode;

	printf("Highest degree:  %8d (#%d)\n", highestDegree, higestDegNum);
	printf("Lowest degree:   %8d (#%d)\n", lowestDegree, lowestDegNum);
	printf("Average degree:  %8ld\n", averageDegree);

	writeDegreeDistribution(highestDegree, lowestDegree, averageDegree, filename);
}

void graphStats(char *filename) {
	printf("===== Graph Stats =====\n");
	printf("Number of Nodes: %8d\n", highestNode);
	printf("Number of Edges: %8d\n", edgeCount);
	degreeStats(filename);
}

void printGraph() {
	int i;
	for (i = 0; i < highestNode; i++) {
		if (graph[i] != NULL) {
			printf("%d", i);

			Node *temp = graph[i]->next;
			while (temp != NULL) {
				printf("->%d", temp->vertexNum);
				temp = temp->next;
			}

			printf("\n");
		}
	}
}

int main(int argc, char const *argv[]) {
	
	if (argc != 2) {
		fprintf(stderr, "Usage: %s <graphFile>\n", argv[0]);
		fprintf(stderr, "<graphFile>: Name of graph to test\n");
		exit(1);
	}

	clock_t t1, t2;
	t1 = clock();

	FILE *f = fopen(argv[1], "r");

	if (!f) {
		fprintf(stderr, "Could not open file\n");
		exit(1);
	}

	char buffer[256];
	
	while ((fgets(buffer, 256, f)) != NULL) {
		char *tok = strtok(buffer, " ");
		int nodeNum = atoi(tok);

		if (nodeNum > highestNode)
			highestNode = nodeNum;

		tok = strtok(NULL, " ");
		nodeNum = atoi(tok);

		if (nodeNum > highestNode)
			highestNode = nodeNum;

	}

	graph = (Node **)malloc(sizeof(Node *)*highestNode+1);

	rewind(f);

	while ((fgets(buffer, 256, f)) != NULL) {
		char *tok = strtok(buffer, " ");
		int src = atoi(tok);
		tok = strtok(NULL, " ");
		int dest = atoi(tok);

		connectNode(src, dest);
	}

	fclose(f);

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
		runSimulation();

	free(temp);
	free(graph);

	t2 = clock() - t2; // Done processing

	printf("\nGraph Read Runtime: %lu clicks (%.3f seconds)\n", t1, (((float)t1)/CLOCKS_PER_SEC));
	printf("Graph Process Runtime: %lu clicks (%.3f seconds)\n", t2, (((float)t2)/CLOCKS_PER_SEC));
	printf("Total Runtime: %lu clicks (%.3f seconds)\n", t1+t2, (((float)t1+t2)/CLOCKS_PER_SEC));

	return 0;
}