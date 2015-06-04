#ifndef GRAPH_H
#define GRAPH_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <time.h>
#include <omp.h>

typedef struct Node_t {
	struct Node_t *next;

	int vertexNum;

} Node;

Node **graph;

#endif
