#include "simulation.h"
#include "graph.h"

int infectiousPeriod;
float contactChance;
int kVal;

int seedInfection() {
	int patientZero = rand() % highestNode;

	graph[patientZero]->isInfected = true;
	graph[patientZero]->roundInfected = 0;

	return patientZero;
}

int infectNeighbors(Node *node, int round) {
	if (round == node->roundInfected && node->roundInfected != 0) {
		// can't infect in the same round we got infected
		return 0;
	}

	Node *temp = node->next;

	int newInfectious = 0;
	while (temp != NULL) {
		if (graph[temp->vertexNum] != NULL && !graph[temp->vertexNum]->isInfected) {
			if (graph[temp->vertexNum]->isRecovered) {
				temp->isRecovered = true;
				temp = temp->next;
				continue;
			}

			// Check if we contact the other agent
			float contact = (float)(rand() % 100) / 100;
			if (contact > contactChance) { continue; }

			float chance = (float)(rand() % 100) / 100;

			if (chance < infectiousProbability) {
				temp->isInfected = true;
				temp->roundInfected = round;
				graph[temp->vertexNum]->isInfected = true;
				graph[temp->vertexNum]->roundInfected = round;
				newInfectious++;

				if (newInfectious >= kVal) {
					break; // Can only infect kVal per round
				}
			}
		}

		temp = temp->next;
	}

	return newInfectious;
}

bool checkRecovery(Node *node, int round) {
	if (node == NULL || !node->isInfected) return false;

	if (round - (node->roundInfected+1) >= infectiousPeriod) {
		node->isInfected = false;
		node->isRecovered = true;
		return true;
	}

	return false;
}


void countNodes(int t, int *numInfected, int *numRecovered, int *numSusceptible) {
	int i = 0;
	numInfected[t], numRecovered[t], numSusceptible[t] = 0;
	for (i = 0; i <= highestNode; i++) {
		if(graph[i] != NULL) {
			if(graph[i]->isInfected) (numInfected[t])++;
			else if(graph[i]->isRecovered) (numRecovered[t])++;
			else (numSusceptible[t])++;
		}
	}
}

void runSimulation(char *graphName) {
	srand(time(NULL));

	printf("Enter the probability of an agent to become Infections: ");
	scanf("%f", &infectiousProbability);

	printf("Enter the probability of contact between agents: ");
	scanf("%f", &contactChance);

	printf("Enter k value (Max # a node may infect per round, 0 for no limit): ");
	scanf("%d", &kVal);

	if (kVal == 0)
		kVal = highestNode;

	if (infectiousProbability > 1.0 || infectiousProbability < 0.0) {
		fprintf(stderr, "Invalid Probability entered. Use a number between 0 and 1\n");
		exit(1);
	}

	int simulDuration;
	printf("How many timesteps for this simulation: ");
	scanf("%d", &simulDuration);

	printf("Enter the period an agent will remain infectious: ");
	scanf("%d", &infectiousPeriod);

	int *newInfectious = (int *)malloc(sizeof(int)*simulDuration);
	int *totalInfectious = (int *)malloc(sizeof(int)*simulDuration);
	int *totalRecovered = (int *)malloc(sizeof(int)*simulDuration);
	int *totalSusceptible = (int *)malloc(sizeof(int)*simulDuration);

	printf("\n");

	///// SIMULATION /////

	int i, j, zero, totalInfections = 0, numRecovered = 0, infectedRound = -1;
	for (i = 0; i < simulDuration; i++) {
		printf("\rPeforming timestep %d", i);
		if (i == 0)
			zero = seedInfection();

		int infectionsThisRound = 0, recoveredThisRound = 0;
		for (j = 0; j < highestNode; j++) {
			if (graph[j] != NULL && graph[j]->isRecovered) continue;

			if (checkRecovery(graph[j], i))
				recoveredThisRound++;

			if (graph[j] != NULL && graph[j]->isInfected)
				infectionsThisRound += infectNeighbors(graph[j], i);
		}
		
		countNodes(i, totalInfectious, totalRecovered, totalSusceptible);

		newInfectious[i] = infectionsThisRound;
		totalInfections	+= infectionsThisRound;
		numRecovered 	+= recoveredThisRound;

		bool allSusceptible = (totalSusceptible[i] == highestNode);
		bool allInfectious  = (totalInfectious[i]  == highestNode);
		bool allRecovered   = (totalRecovered[i]   == highestNode);

		if (allRecovered)
			break;
		if (totalInfectious[i] == 0 && totalInfections > 0) // stop if no disease left
			break;
		else if (allInfectious)
			infectedRound = i;
	}

	///// END SIMULATION /////

	printf("\n======== Simulation Results ========\n");
	printf("Duration: %d timesteps\n", simulDuration);
	printf("PatientZero: %d\n", zero);
	printf("Number Infected: %d/%d\n", totalInfections, highestNode);

	bool isAllInfected = (infectedRound > -1);
	if (isAllInfected) {
		printf("\nAll agents were infected by round: %d\n", infectedRound);
	}

	FILE *output = fopen("web/infData.js", "w");
	if (!output) {
		fprintf(stderr, "Could not open output file\n");
		exit(1);
	}

	fprintf(output, "var data = {\n");
	fprintf(output, "\t\"name\": \"%s\",\n", graphName);
	fprintf(output, "\t\"nodeCount\": %d,\n", highestNode);
	fprintf(output, "\t\"edgeCount\": %d,\n", edgeCount);
	fprintf(output, "\t\"infectionCount\": %d,\n", totalInfections);
	fprintf(output, "\t\"patientZero\": %d,\n", zero);
	fprintf(output, "\t\"simulDuration\": %d,\n", simulDuration);
	fprintf(output, "\t\"infectionChance\": %f,\n", infectiousProbability);
	fprintf(output, "\t\"contactChance\": %f,\n", contactChance);
	fprintf(output, "\t\"infectionPeriod\": %d,\n", infectiousPeriod);

	if (isAllInfected) {
		fprintf(output, "\t\"endStep\": %d,\n", infectedRound);
	}

	// Rate of infection
	fprintf(output, "\t\"values\": [\n");
	for (j = 0; j < i; j++) {
		fprintf(output, "\t\t{\"x\": %d, \"y\": %d},\n", j, newInfectious[j]);
	}
	fprintf(output, "\t],\n");

	// number of infected
	fprintf(output, "\t\"numInf\": [\n");
	for (j = 0; j < i; j++) {
		fprintf(output, "\t\t{\"x\": %d, \"y\": %d},\n", j, totalInfectious[j]);
	}
	fprintf(output, "\t],\n");

	// number of recovered
	fprintf(output, "\t\"numRec\": [\n");
	for (j = 0; j < i; j++) {
		fprintf(output, "\t\t{\"x\": %d, \"y\": %d},\n", j, totalRecovered[j]);
	}
	fprintf(output, "\t],\n");

	// number of susceptible
	fprintf(output, "\t\"numSus\": [\n");
	for (j = 0; j < i; j++) {
		fprintf(output, "\t\t{\"x\": %d, \"y\": %d},\n", j, totalSusceptible[j]);
	}
	fprintf(output, "\t]\n");

	fprintf(output, "}\n");

	free(newInfectious);
	free(totalInfectious);
	free(totalRecovered);

	fclose(output);
	printf("\nPer round results written to \"results.txt\"\n");
}
