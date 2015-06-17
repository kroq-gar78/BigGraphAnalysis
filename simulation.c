#include "simulation.h"
#include "graph.h"

bool allInfected() {
	bool isAllInfected = true;

	int i;
	for (i = 0; i <= highestNode; i++) {
		if (graph[i] != NULL && !graph[i]->isInfected) {
			isAllInfected = false;
			break;
		}
	}

	return isAllInfected;
}

int seedInfection() {
	int patientZero = rand() % highestNode;

	graph[patientZero]->isInfected = true;

	return patientZero;
}

int infectNeighbors(Node *node) {
	Node *temp = node->next;

	int newInfectious = 0;
	while (temp != NULL) {
		if (graph[temp->vertexNum] != NULL && !graph[temp->vertexNum]->isInfected) {
			float chance = (float)(rand() % 100) / 100;

			if (chance < infectiousProbability) {
				temp->isInfected = true;
				graph[temp->vertexNum]->isInfected = true;
				newInfectious++;
			}
		}

		temp = temp->next;
	}

	return newInfectious;
}

int numberInfected() {
	int i, numInfected = 0;
	for (i = 0; i <= highestNode; i++) {
		if (graph[i] != NULL && graph[i]->isInfected)
			numInfected++;
	}

	return numInfected;
}

void drawProgressBar(int i, int simulDuration) {
	int j, numberBars = i / simulDuration;

	printf("\r[");
	for (j = 0; j < 100; j++) {
		if (j <= numberBars)
			printf("#");
		else
			printf(" ");
	}
	printf("]");
	//fflush(stdout);
}

void runSimulation() {
	srand(time(NULL));

	printf("Enter the probability of an agent to become Infections: ");
	scanf("%f", &infectiousProbability);

	if (infectiousProbability > 1.0 || infectiousProbability < 0.0) {
		fprintf(stderr, "Invalid Probability entered. Use a number between 0 and 1\n");
		exit(1);
	}

	int simulDuration;
	printf("How many timesteps for this simulation: ");
	scanf("%d", &simulDuration);

	int *newInfectious = (int *)malloc(sizeof(int)*simulDuration);

	printf("\n");

	int i, j, zero;
	for (i = 0; i < simulDuration; i++) {
		//drawProgressBar(i, simulDuration);
		printf("\rPeforming timestep %d", i);
		if (i == 0)
			zero = seedInfection();

		int infectionsThisRound = 0;
		for (j = 0; j < highestNode; j++) {
			if (graph[j] != NULL && graph[j]->isInfected)
				infectionsThisRound += infectNeighbors(graph[j]);
		}
		newInfectious[i] = infectionsThisRound;

		if (allInfected())
			break;
	}

	printf("\n======== Simulation Results ========\n");
	printf("Duration: %d timesteps\n", simulDuration);
	printf("PatientZero: %d\n", zero);
	printf("Number Infected: %d/%d\n", numberInfected(), highestNode);

	if (allInfected()) {
		printf("\nAll agents were infected by round: %d\n", i);
	}

	FILE *output = fopen("results.txt", "w");
	if (!output) {
		fprintf(stderr, "Could not open output file\n");
		exit(1);
	}

	fprintf(output, "----- Per Round Infections -----\n");
	for (j = 0; j <= i; j++) {
		fprintf(output, "Round %d: %d\n", j, newInfectious[j]);
	}

	fclose(output);
	printf("\nPer round results written to \"results.txt\"\n");
}
