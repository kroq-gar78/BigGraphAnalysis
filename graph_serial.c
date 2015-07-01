#include "graph.h"

int highestNode = 0;
int edgeCount = 0;
int rank;

void connectNode(int src, int dest) {
	if (graph[src] == NULL) {
		Node *head = (Node *)malloc(sizeof(Node));
		head->vertexNum = src;
		graph[src] = head;
	}

	Node *temp = graph[src];
	while (temp->next != NULL)
		temp = temp->next;

	Node *newNode = (Node *)malloc(sizeof(Node));
	newNode->vertexNum = dest;
	temp->next = newNode;

	if (graph[dest] == NULL) {
		Node *head = (Node *)malloc(sizeof(Node));
		head->vertexNum = dest;
		graph[dest] = head;
	}

	temp = graph[dest];
	while (temp->next != NULL)
		temp = temp->next;

	newNode = (Node *)malloc(sizeof(Node));
	newNode->vertexNum = src;
	temp->next = newNode;

	edgeCount++;
}

int countDegree(Node *node) {
	int count = 0;
	Node *temp = node;
	while (temp->next != NULL) {
		count++;
		temp = temp->next;
	}

	return count;
}

void degreeStats() {
	long averageDegree = 0;
	int lowestDegree = INT_MAX, highestDegree = 0, degree;
	int i;

	int higestDegNum, lowestDegNum;

	// parallel
	int chunk = highestNode / 10;

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

	averageDegree /= highestNode;

	printf("Highest degree:  %8d (#%d)\n", highestDegree, higestDegNum);
	printf("Lowest degree:   %8d (#%d)\n", lowestDegree, lowestDegNum);
	printf("Average degree:  %8d\n", (int)averageDegree);
}

void graphStats() {
	printf("===== Graph Stats =====\n");
	printf("Number of Nodes: %8d\n", highestNode);
	printf("Number of Edges: %8d\n", edgeCount);
	degreeStats();
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
		fprintf(stderr, "Specify a graph file\n");
		exit(1);
	}

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

	//printGraph();
	graphStats();

	free(graph);

	return 0;
}
