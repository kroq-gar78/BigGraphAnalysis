#include "graph.h"
#include "simulation.h"

#define MAX_DATA_POINTS 100

int rank;

Node *createNode(int vertexNum) {
	Node *newNode = (Node *)malloc(sizeof(Node));

	newNode->vertexNum = vertexNum;
	newNode->isInfected = false;
	newNode->isRecovered = false;
	newNode->roundInfected = -1;
	newNode->roundRecovered = -1;
	newNode->next = NULL;

    // TODO: consider setting default to 0
    newNode->weight = 1; // refers to weight of the edge between this node and the previous node

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

void connectNode(int src, int dest, bool directed, float weight) {
	if (graph[src] == NULL) {
		Node *head = createNode(src);
		graph[src] = head;
	}
    if (graph[dest] == NULL) {
        Node *d = createNode(dest);
        graph[dest] = d;
    }

	Node *temp = graph[src];
	Node *newNode;

	if (!checkConnection(temp, dest)) {
		while (temp->next != NULL)
			temp = temp->next;

		newNode = createNode(dest);
        newNode->weight = weight;
		temp->next = newNode;
	}

    if(!directed) {
        connectNode(dest, src, true, weight);
        edgeCount--; // only count unweighted edges once
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

void writeDegreeDistribution(int highestDegNum, int lowestDegNum,
	double avgDegree, char *filename) {
	int numDataPoints = highestNode;
	int step = 1;

    //FILE *f;
    char *outfile;
    if (strlen(arguments.outfile) == 0) {
        outfile = "web/data.json";
    }
    else {
        outfile = (char *)malloc(strlen(arguments.outfile)+1);
        strcpy(outfile, arguments.outfile);
    }
    FILE *f = fopen(outfile, "w");

	if (!f) {
		fprintf(stderr, "Unable to open output file\n");
		exit(-1);
	}

	printf("Writing degree distribution data...\n");

	int *distribution = (int *)malloc(sizeof(int)*highestDegNum+1);
	int chunk = highestDegNum / 10;
	if(highestDegNum < 10) chunk = 1;
	int i;

#pragma omp parallel shared(highestDegNum, distribution, chunk), private(i)
{
	#pragma omp for schedule(dynamic,chunk) nowait
	for (i = 0; i <= highestDegNum; i++)
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

	if (highestDegNum > MAX_DATA_POINTS) {
		numDataPoints = highestNode;
		step = highestDegNum / MAX_DATA_POINTS;
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

	fprintf(f, "{\n");
	fprintf(f, "\t\"name\": \"%s\",\n", filename);
	fprintf(f, "\t\"nodeCount\": %d,\n", highestNode);
	fprintf(f, "\t\"edgeCount\": %d,\n", edgeCount);
	fprintf(f, "\t\"highestDeg\": %d,\n", highestDegNum);
	fprintf(f, "\t\"lowestDeg\": %d,\n", lowestDegNum);
	fprintf(f, "\t\"avgDeg\": %lf,\n", avgDegree);
	fprintf(f, "\t\"standardDev\": %lf,\n", standardDev);
	fprintf(f, "\t\"values\": [\n");

	for (i = 0; i <= highestDegNum; i++) {
		if (i > 0 && distribution[i] == 0) {
			continue;
		}

		fprintf(f, "\t\t{\"x\": %d, \"y\": %d}", i, distribution[i]);
		if(i < highestDegNum) fprintf(f, ",");
		fprintf(f, "\n");
	}

	fprintf(f, "\t]\n");
	fprintf(f, "}\n");

	fclose(f);
	free(distribution);

	printf("Data written as \"%s\"\n", outfile);

}

void degreeStats(char *filename) {
	double averageDegree = 0;
	int lowestDegree = INT_MAX, highestDegree = 0, degree;
	int i;

	int highestDegNum, lowestDegNum;

	// parallel
	int chunk = highestNode / 10;
	if(highestNode < 10) chunk = 1;

#pragma omp parallel shared(lowestDegree, highestDegree, degree, chunk, highestDegNum, lowestDegNum), private(i), reduction(+:averageDegree)
{

	#pragma omp for schedule(dynamic,chunk) nowait
	for (i = 0; i <= highestNode; i++) {
		if (graph[i] != NULL) {
			degree = countDegree(graph[i]);

			if (degree > highestDegree) {
				highestDegree = degree;
				highestDegNum = i;
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

	printf("Highest degree:  %8d (#%d)\n", highestDegree, highestDegNum);
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

void readGraph(const char *filename, bool directed, bool weighted) {
	FILE *f = fopen(filename, "r");

	if (!f) {
		fprintf(stderr, "Could not open file\n");
		exit(1);
	}

	char buffer[256];

	while ((fgets(buffer, 256, f)) != NULL) {
		char *tok = strtok(buffer, " \t");
		int nodeNum = atoi(tok);

		if (nodeNum > highestNode)
			highestNode = nodeNum;

		tok = strtok(NULL, " \t");
		nodeNum = atoi(tok);

		if (nodeNum > highestNode)
			highestNode = nodeNum;

	}

	graph = (Node **)malloc(sizeof(Node *)*(highestNode+1));
	memset(graph, 0, sizeof(Node *)*(highestNode+1));

	rewind(f);

	while ((fgets(buffer, 256, f)) != NULL) {
		char *tok = strtok(buffer, " \t");
		int src = atoi(tok);
		tok = strtok(NULL, " \t");
		int dest = atoi(tok);

        float weight = 1;
        if(weighted)
        {
            tok = strtok(NULL, " \t");
            weight = atof(tok);
        }

		connectNode(src, dest, directed, weight);
	}

	fclose(f);
}
