#ifndef SIMUL_H
#define SIMUL_H

void runSimulation(char *graphName);
void countNodes(int t, int *numInfected, int *numRecovered, int *numSusceptible, int *numVaccinated);
int seedInfection();

#endif
