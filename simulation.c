#include "simulation.h"
#include "graph.h"

int infectiousPeriod;
float contactChance;
float kVal;

char type;

// infect the first susceptible node
int seedInfection() {
    int patientZero = (rand() % (highestNode-*numVaccinated)); // index of random susceptible node
    /*int patientZero = 6;*/
    int i, curr = 0;
    for(i = 1; i <= highestNode; i++) {
       if(graph[i] != NULL && !graph[i]->isVaccinated) {
           if(curr++ == patientZero) {
               patientZero = i;
               break;
           }
       }
    }
    printf("numVaccinated: %d\n", *numVaccinated);

	graph[patientZero]->isInfected = true;
	graph[patientZero]->roundInfected = 0;

	return patientZero;
}

int infectNeighbors(Node *node, int round, int graphNum) {
	if (round == node->roundInfected /*&& node->roundInfected != 0*/) {
		// can't infect in the same round we got infected
		return 0;
	}

	if (type == 'a') {
		if (round - (node->roundInfected+1) >= infectiousPeriod)
			return 0;
	}

	Node *temp = NULL;
    temp = node->next[graphNum];

	int newInfectious = 0;
	int maxNum = (int)ceil(kVal * countDegree(node, graphNum));
	if(maxNum <= 0) return 0;
	while (temp != NULL) {
        if (graph[temp->vertexNum] != NULL && !graph[temp->vertexNum]->isInfected && !graph[temp->vertexNum]->isVaccinated) {
			if (graph[temp->vertexNum]->isRecovered) {
				temp->isRecovered = true;
                temp = temp->next[graphNum];
				continue;
			}

			// Check if we contact the other agent
			float contact = (float)(rand() % 100) / 100;
			if (contact > contactChance) { continue; }

			float chance = (float)(rand() % 100) / 100;

			if (chance < infectiousProbability) {
				if (round == graph[temp->vertexNum]->roundRecovered) {
                    temp = temp->next[graphNum];
					continue;
				}

				temp->isInfected = true;
				temp->roundInfected = round;
				graph[temp->vertexNum]->isInfected = true;
				graph[temp->vertexNum]->roundInfected = round;
				newInfectious++;

				// Can only infect kVal*degree per round
				if (newInfectious >= maxNum) {
					break;
				}
			}
		}

        temp = temp->next[graphNum];
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
    int tmp_infected = 0, tmp_recovered = 0, tmp_susceptible = 0, tmp_vaccinated = 0;
    //#pragma omp parallel for private(i) shared(graph, highestNode) reduction(+:tmp_infected, tmp_recovered, tmp_susceptible) //schedule(dynamic, 100)
    // might need large chunks for speedup (currently no speedup with parallel)
	for (i = 0; i <= highestNode; i++) {
		if(graph[i] != NULL) {
            if(graph[i]->isInfected) tmp_infected++;
            else if(graph[i]->isRecovered) tmp_recovered++;
            else if(graph[i]->isVaccinated) tmp_vaccinated++;
            else tmp_susceptible++;
		}
	}

    numInfected[t] = tmp_infected;
    numRecovered[t] = tmp_recovered;
    numSusceptible[t] = tmp_susceptible;
    *numVaccinated = tmp_vaccinated;
}

void runSimulation(char *graphName) {

	// use both time (in microseconds) and PID for RNG seed
	// from: https://stackoverflow.com/a/15846558
	struct timeval t;
	gettimeofday(&t, NULL);
	srand(t.tv_usec * t.tv_sec * getpid());

	fflush(stdin);

    /**
     * normal: SIR
     * accumulative: SI
     * reinfect: SIS
     **/
    type = arguments.type;
    if (type == 0) {
        printf("Select simulation: [n]ormal, [a]ccumulative, or [r]einfect (default 'n'): ");
        scanf("%c", &type);
    }
    if(type != 'a' && type != 'r') type = 'n';

    infectiousProbability = arguments.infectiousProbability;
    if (infectiousProbability == -1) {
        printf("Enter the probability of an agent to become infectious: ");
        scanf("%f", &infectiousProbability);
    }

    contactChance = arguments.contactChance;
    if (contactChance == -1) {
        printf("Enter the probability of contact between agents: ");
        scanf("%f", &contactChance);
    }

	if (infectiousProbability > 1.0 || infectiousProbability < 0.0 ||
		contactChance > 1.0 || contactChance < 0.0) {
		fprintf(stderr, "Invalid probability entered. Use a number between 0 and 1\n");
		exit(1);
	}

    kVal = arguments.kVal;
    if (kVal == -1) {
        printf("Enter k value (percentage of neighbors a node can infect per round): ");
        scanf("%f", &kVal);
    }

	if (kVal > 1.0 || kVal < 0.0) {
		fprintf(stderr, "Invalid k value: %f\nUse a number from 0 to 1.", kVal);
	}

	int simulDuration = arguments.simulDuration;
    if (simulDuration == -1) {
        printf("How many timesteps for this simulation: ");
        scanf("%d", &simulDuration);
    }

    infectiousPeriod = arguments.infectiousPeriod;
    if (infectiousPeriod == -1) {
        printf("Enter the period an agent will remain infectious: ");
        scanf("%d", &infectiousPeriod);
    }

	int *newInfectious = (int *)malloc(sizeof(int)*simulDuration);
	int *totalInfectious = (int *)malloc(sizeof(int)*simulDuration);
	int *totalRecovered = (int *)malloc(sizeof(int)*simulDuration);
	int *totalSusceptible = (int *)malloc(sizeof(int)*simulDuration);
    int *numVaccinated = (int *)malloc(sizeof(int));

	memset(newInfectious,	0, sizeof(int)*simulDuration);
	memset(totalInfectious,  0, sizeof(int)*simulDuration);
	memset(totalRecovered,   0, sizeof(int)*simulDuration);
	memset(totalSusceptible, 0, sizeof(int)*simulDuration);

	printf("\n");

	///// SIMULATION /////

	int i, j, zero, totalInfections = 0, numRecovered = 0, infectedRound = -1;
	bool lastRound = false;
	for (i = 0; i < simulDuration; i++) {
		printf("\rPerforming timestep %d", i);
        int graphNum = (i%numLayers);
        /*int graphNum = 1;*/

		int infectionsThisRound = 0, recoveredThisRound = 0;
		if (i == 0) {
			zero = seedInfection();
			infectionsThisRound++; // so that we can get 100% infection instead of 99%
		}

		for (j = 0; j <= highestNode; j++) {
            if (graph[j] == NULL) continue;
			if (graph[j]->isRecovered || graph[j]->isVaccinated) continue;

			if (checkRecovery(graph[j], i))
				recoveredThisRound++;

			if (graph[j]->isInfected)
				infectionsThisRound += infectNeighbors(graph[j], i, graphNum);
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

    char *outfile;
    if (strlen(arguments.outfile) == 0) {
        outfile = "web/infData.json";
    }
    else {
        outfile = (char *)malloc(strlen(arguments.outfile)+1);
        strcpy(outfile, arguments.outfile);
    }
    FILE *output = fopen(outfile, "w");

	if (!output) {
		fprintf(stderr, "Could not open output file\n");
		exit(1);
	}

	fprintf(output, "{\n");
	fprintf(output, "\t\"name\": \"%s\",\n", graphName);
    if(strlen(arguments.graph_path2) != 0)
        fprintf(output, "\t\"graph2\": \"%s\",\n", arguments.graph_path2);
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
	fprintf(output, "\t\"kVal\": %f,\n", kVal);

	if (isAllInfected) {
		fprintf(output, "\t\"endStep\": %d,\n", infectedRound);
	}

	// Rate of infection
	fprintf(output, "\t\"values\": [\n");
	for (j = 0; j < i; j++) {
		fprintf(output, "\t\t{\"x\": %d, \"y\": %d}", j, newInfectious[j]);
		if(j < i-1) fprintf(output, ",");
		fprintf(output, "\n");
	}
	fprintf(output, "\t],\n");

	// number of infected
	fprintf(output, "\t\"numInf\": [\n");
	for (j = 0; j < i; j++) {
		fprintf(output, "\t\t{\"x\": %d, \"y\": %d}", j, totalInfectious[j]);
		if(j < i-1) fprintf(output, ",");
		fprintf(output, "\n");
	}
	fprintf(output, "\t],\n");

	// number of recovered
	fprintf(output, "\t\"numRec\": [\n");
	for (j = 0; j < i; j++) {
		fprintf(output, "\t\t{\"x\": %d, \"y\": %d}", j, totalRecovered[j]);
		if(j < i-1) fprintf(output, ",");
		fprintf(output, "\n");
	}
	fprintf(output, "\t],\n");

	// number of susceptible
	fprintf(output, "\t\"numSus\": [\n");
	for (j = 0; j < i; j++) {
		fprintf(output, "\t\t{\"x\": %d, \"y\": %d}", j, totalSusceptible[j]);
		if(j < i-1) fprintf(output, ",");
		fprintf(output, "\n");
	}
	fprintf(output, "\t]\n");

	fprintf(output, "}\n");

	free(newInfectious);
	free(totalInfectious);
	free(totalRecovered);

	fclose(output);
	printf("\nPer round results written to \"%s\"\n", outfile);
}
