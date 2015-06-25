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
	struct Node_t *next;

	int vertexNum;

	bool isInfected;
	bool isRecovered;
	bool isVaccinated;
	int roundInfected;

} Node;

Node **graph;

float infectiousProbability;
int highestNode;
int edgeCount;

#endif
