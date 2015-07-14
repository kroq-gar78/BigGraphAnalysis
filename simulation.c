#include "simulation.h"
#include "graph.h"

int infectiousPeriod;
float contactChance;
int kVal;

char type;

int seedInfection() {
    int patientZero = (rand() % highestNode)+1; // compensate for 1-based indeces
    /*int patientZero = 6;*/
    printf("maxnode: %d\n", highestNode);

	graph[patientZero]->isInfected = true;
	graph[patientZero]->roundInfected = 0;

	return patientZero;
}

int infectNeighbors(Node *node, int round) {
	if (round == node->roundInfected /*&& node->roundInfected != 0*/) {
		// can't infect in the same round we got infected
		return 0;
	}

	if (type == 'a') {
		if (round - (node->roundInfected+1) >= infectiousPeriod)
			return 0;	
	}

	Node *temp = node->next;

	int newInfectious = 0;
	while (temp != NULL) {
  if (graph[temp->vertexNum] != NULL && !graph[temp->vertexNum]->isInfected && !graph[temp->vertexNum]->isVaccinated) {
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
				if (round == graph[temp->vertexNum]->roundRecovered) {
					temp = temp->next;
					continue;
				}

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
	if (node == NULL || !node->isInfected || type == 'a') return false;

	if (round - (node->roundInfected+1) >= infectiousPeriod) {
		node->isInfected = false;
		if (type == 'n') {
			node->isRecovered = true;
			return true;
		} else if (type == 'r') {
			node->isRecovered = false;
			node->roundRecovered = round;
			return false;
		}
	}

	return false;
}


void countNodes(int t, int *numInfected, int *numRecovered, int *numSusceptible, int *numVaccinated) {
	int i = 0;
	numInfected[t], numRecovered[t], numSusceptible[t] = 0;
    *numVaccinated = 0;
	for (i = 0; i <= highestNode; i++) {
		if(graph[i] != NULL) {
			if(graph[i]->isInfected) (numInfected[t])++;
			else if(graph[i]->isRecovered) (numRecovered[t])++;
            else if(graph[i]->isVaccinated) (*numVaccinated)++;
			else (numSusceptible[t])++;
		}
	}
}

void runSimulation(char *graphName) {
	srand(time(NULL));

	fflush(stdin);

    /**
     * normal: SIR
     * accumulative: SI
     * reinfect: SIS
     **/
	printf("Select simulation: [n]ormal, [a]ccumulative, or [r]einfect: ");
	scanf(" %c", &type);

	printf("Enter the probability of an agent to become infectious: ");
	scanf("%f", &infectiousProbability);

	printf("Enter the probability of contact between agents: ");
	scanf("%f", &contactChance);

	if (infectiousProbability > 1.0 || infectiousProbability < 0.0 ||
        contactChance > 1.0 || contactChance < 0.0) {
		fprintf(stderr, "Invalid probability entered. Use a number between 0 and 1\n");
		exit(1);
	}

	printf("Enter k value (Max # a node may infect per round, 0 for no limit): ");
	scanf("%d", &kVal);

	if (kVal == 0)
		kVal = highestNode;

	int simulDuration;
	printf("How many timesteps for this simulation: ");
	scanf("%d", &simulDuration);

	printf("Enter the period an agent will remain infectious: ");
	scanf("%d", &infectiousPeriod);

	int *newInfectious = (int *)malloc(sizeof(int)*simulDuration);
	int *totalInfectious = (int *)malloc(sizeof(int)*simulDuration);
	int *totalRecovered = (int *)malloc(sizeof(int)*simulDuration);
	int *totalSusceptible = (int *)malloc(sizeof(int)*simulDuration);
    int *numVaccinated = (int *)malloc(sizeof(int));

	memset(newInfectious,    0, sizeof(int)*simulDuration);
	memset(totalInfectious,  0, sizeof(int)*simulDuration);
	memset(totalRecovered,   0, sizeof(int)*simulDuration);
	memset(totalSusceptible, 0, sizeof(int)*simulDuration);

	printf("\n");

	///// SIMULATION /////

	int i, j, zero, totalInfections = 0, numRecovered = 0, infectedRound = -1;
    bool lastRound = false;
	for (i = 0; i < simulDuration; i++) {
		printf("\rPerforming timestep %d", i);

		int infectionsThisRound = 0, recoveredThisRound = 0;
		if (i == 0) {
			zero = seedInfection();
            infectionsThisRound++; // so that we can get 100% infection instead of 99% 
        }

		for (j = 0; j <= highestNode; j++) {
			if (graph[j] != NULL && (graph[j]->isRecovered || graph[j]->isVaccinated)) continue;

			if (checkRecovery(graph[j], i))
				recoveredThisRound++;

			if (graph[j] != NULL && graph[j]->isInfected)
				infectionsThisRound += infectNeighbors(graph[j], i);
		}
		
		countNodes(i, totalInfectious, totalRecovered, totalSusceptible, numVaccinated);

		newInfectious[i] = infectionsThisRound;
		totalInfections	+= infectionsThisRound;
		numRecovered 	+= recoveredThisRound;

		bool allSusceptible = (totalSusceptible[i] == highestNode);
		bool allInfectious  = (totalInfectious[i]  == highestNode);
		bool allRecovered   = (totalRecovered[i]   == highestNode);

        if (lastRound)
            break;
		if (allRecovered)
			lastRound = true; // do an extra round for completeness (otherwise, infectious doesn't really reach 0 in output)
		if (totalInfectious[i] == 0) { // stop if no disease left
            lastRound = true; 
        }
		if (allInfectious)
			infectedRound = i;
	}

    int roundsNeeded = i;

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
    fprintf(output, "\t\"numVaccinated\": %d,\n", *numVaccinated);
	fprintf(output, "\t\"infectionCount\": %d,\n", totalInfections);
	fprintf(output, "\t\"patientZero\": %d,\n", zero);
	fprintf(output, "\t\"simulDuration\": %d,\n", simulDuration);
    fprintf(output, "\t\"roundsNeeded\": %d,\n", roundsNeeded);
	fprintf(output, "\t\"infectionChance\": %f,\n", infectiousProbability);
	fprintf(output, "\t\"contactChance\": %f,\n", contactChance);
	fprintf(output, "\t\"infectionPeriod\": %d,\n", infectiousPeriod);
	fprintf(output, "\t\"kVal\": %d,\n", kVal);

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
