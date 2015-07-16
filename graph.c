#include "graph.h"
#include "simulation.h"

#define MAX_DATA_POINTS 100

int rank;

Node *createNode(int vertexNum) {
	Node *newNode = (Node *)malloc(sizeof(Node));

	newNode->vertexNum = vertexNum;
	newNode->isInfected = false;
	newNode->isRecovered = false;
	newNode->isVaccinated = false;
	newNode->roundInfected = -1;
	newNode->roundRecovered = -1;
	newNode->next = NULL;
	
	return newNode;
}

bool checkConnection(Node *srcNode, int dest) {
	Node *temp = srcNode->next;

	while (temp != NULL) {
		if (temp->vertexNum == dest)
			return true;

		temp = temp->next;
	}

	return false;
}

void connectNode(int src, int dest) {
	if (graph[src] == NULL) {
		Node *head = createNode(src);
		graph[src] = head;
	}

	Node *temp = graph[src];
	Node *newNode;
	
	if (!checkConnection(temp, dest)) {
		while (temp->next != NULL)
			temp = temp->next;

		newNode = createNode(dest);
		temp->next = newNode;
        cs_entry(adj, src, dest, 1.0);
	}

	if (graph[dest] == NULL) {
		Node *head = createNode(dest);
		graph[dest] = head;
	}

	temp = graph[dest];
	
	if (!checkConnection(temp, src)) {
		while (temp->next != NULL)
			temp = temp->next;

		newNode = createNode(src);
		temp->next = newNode;
        cs_entry(adj, dest, src, 1.0);
	}

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

	int *distribution = (int *)malloc(sizeof(int)*higestDegNum+1);
	int chunk = higestDegNum / 10;
	if(higestDegNum < 10) chunk = 1;
	int i;

#pragma omp parallel shared(higestDegNum, distribution, chunk), private(i)
{
	#pragma omp for schedule(dynamic,chunk) nowait
	for (i = 0; i <= higestDegNum; i++)
		distribution[i] = 0;

	#pragma omp for schedule(dynamic, chunk)
	for (i = 0; i <= highestNode; i++) {
		int degree = countDegree(graph[i]);

		if (degree > 0) {
			#pragma omp atomic
			distribution[degree] += 1;
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
	for (i = 0; i <= highestNode; i++) {
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
		if (i > 0 && distribution[i] == 0) {
			continue;
		}

		fprintf(f, "\t\t{\"x\": %d, \"y\": %d},\n", i, distribution[i]);
	}

	fprintf(f, "\t]\n");
	fprintf(f, "}\n");

	fclose(f);
	free(distribution);

	printf("Data written as \"web/data.js\"\n");

}

void degreeStats(char *filename) {
	double averageDegree = 0;
	int lowestDegree = INT_MAX, highestDegree = 0, degree;
	int i;

	int higestDegNum, lowestDegNum;

	// parallel
	int chunk = highestNode / 10;
	if(highestNode < 10) chunk = 1;

#pragma omp parallel shared(lowestDegree, highestDegree, degree, chunk, higestDegNum, lowestDegNum), private(i), reduction(+:averageDegree)
{

	#pragma omp for schedule(dynamic,chunk) nowait
	for (i = 0; i <= highestNode; i++) {
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
	printf("Average degree:  %8f\n", averageDegree);

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
	for (i = 0; i <= highestNode; i++) {
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

void readGraph(const char *filename) {
	FILE *f = fopen(filename, "r");

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

	graph = (Node **)malloc(sizeof(Node *)*(highestNode+1));
    numVaccinated = (int *)malloc(sizeof(int));
	memset(graph, 0, sizeof(Node *)*highestNode+1);
    *numVaccinated = 0;
    /*bool (*adj)[highestNode+1] = malloc((highestNode+1)*sizeof(bool));*/

    // use csparse for sparse matrix
    adj = cs_spalloc(highestNode+1, highestNode+1, (highestNode+1)*(10+1), // assuming average degree 10 
            1 , 1);

	rewind(f);

	while ((fgets(buffer, 256, f)) != NULL) {
		char *tok = strtok(buffer, " ");
		int src = atoi(tok);
		tok = strtok(NULL, " ");
		int dest = atoi(tok);

		connectNode(src, dest);
	}

	fclose(f);
}

void readVaccinated(const char *filename) {
    FILE *f = fopen(filename, "r");

    if (!f) {
        fprintf(stderr, "Could not open vaccination file\n");
        exit(1);
    }

    char buffer[256];

    while((fgets(buffer, 256, f)) != NULL) {
        int nodeNum = atoi(buffer);
        //printf("%d\n",nodeNum);
        if (graph[nodeNum] == NULL)
            fprintf(stderr, "WARNING: Node at %d is NULL\n", nodeNum);
        else {
            graph[nodeNum]->isVaccinated = true;
            (*numVaccinated)++;
        }
    }
}
