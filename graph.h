#ifndef GRAPH_H
#define GRAPH_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>
#include <time.h>
#include <math.h>
#include <error.h>
#include <argp.h>

#ifdef PARALELL
#include <omp.h>
#endif

typedef struct Node_t {
	struct Node_t *next;

	int vertexNum;

	bool isInfected;
	bool isRecovered;
	int roundInfected;
	int roundRecovered;

} Node;

extern const char *argp_program_version; // is this safe?
struct arguments {
    char *action;
    char *type;
    float *infectiousProbability;
    float *contactChance;
    int *kVal;
    int *infectiousPeriod;
    int *simulDuration;
};

Node **graph;

float infectiousProbability;
int highestNode;
int edgeCount;

void readGraph(const char *filename);
int countDegree(Node *node);
bool checkConnection(Node *srcNode, int dest);

#endif
