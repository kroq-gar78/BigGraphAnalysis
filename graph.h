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
	struct Node_t **next; // array element corresponds to neighbor in layer

	int vertexNum;

	bool isInfected;
	bool isRecovered;
	bool isVaccinated;
	int roundInfected;
	int roundRecovered;

} Node;

extern const char *argp_program_version; // is this safe?
struct arguments {
    char *graph_path;
    char *graph_path2;
    char *vacc_path;
    char *outfile;
    char action;
    char type;
    float infectiousProbability;
    float contactChance;
    float kVal;
    int infectiousPeriod;
    int simulDuration;
};
struct arguments arguments;

Node **graph;

int numLayers; // number of layers in the multiplex

float infectiousProbability;
int highestNode;
int edgeCount;
int *numVaccinated;

void readGraph(const char *filename, int graphNum);
void readVaccinated(const char *filename);
int countDegree(Node *node, int graphNum);
bool checkConnection(Node *srcNode, int dest, int graphNum);

#endif
