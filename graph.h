#ifndef GRAPH_H
#define GRAPH_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>
#include <time.h>
#include <math.h>

#ifdef PARALELL
#include <omp.h>
#endif

typedef struct Node_t {
	struct Node_t *next1;
	struct Node_t *next2;

	int vertexNum;

	bool isInfected;
	bool isRecovered;
	int roundInfected;
	int roundRecovered;

} Node;

Node **graph;

float infectiousProbability;
int highestNode;
int edgeCount;

void readGraph(const char *filename, int graphNum);
int countDegree(Node *node, int graphNum);
bool checkConnection(Node *srcNode, int dest, int graphNum);

#endif
