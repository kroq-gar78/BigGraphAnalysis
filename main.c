#include "graph.h"
#include "simulation.h"

// first, set up argparse
const char *argp_program_version = "ReadGraph";
static char doc[] = "Graph reader and SIR model -- analyze graphs and run SIR epidemics with given parameters";
static char args_doc[] = "ARG1 ARG2";
// in order: {NAME, KEY, ARG, FLAGS, DOC}
// TODO: specify output file (-o)
static struct argp_option options[] = 
{
    {"analyze", 'a', 0, 0, "Produce a degree distribution of the graph"},
    {"infect",  'r', 0, 0, "Run an SIR (or derivative) simulation on the graph"},

    {0,0,0,0, "Simulation options:"},
    {"type",     't', "type", 0, ""}, // TODO: combine this with 'infect'
    {"pinfect",  'i', "PROB", 0, "Probability of an infected node infecting a susceptible neighbor"},
    {"pcontact", 'c', "PROB", 0, "Probability of two nodes with an edge of coming into contact"},
    {"kval",     'k', "k",    0, "Maximum number of neighbors an infected node may infect"},
    {"infecttime",'p',"steps",0, "Infectious period; how long a node stays infected once infected"},
    {"duration", 'd', "steps",0, "Maximum number of time steps"},
    {0}
};
static error_t parse_opt(int key, char *arg, struct argp_state *state);
static struct argp argp;

int main(int argc, char const *argv[]) {
	
	if (argc != 2) {
		fprintf(stderr, "Usage: %s <graphFile>\n", argv[0]);
		fprintf(stderr, "<graphFile>: Name of graph to test\n");
		exit(1);
	}

	clock_t t1, t2;
	t1 = clock();

	readGraph(argv[1]);

	t1 = clock() - t1; // Done reading
	t2 = clock();

	char *temp = (char *)malloc(strlen(argv[1])+1);
	strcpy(temp, argv[1]);

	char *tok = strtok(temp, "/");

	if ((tok = strtok(NULL, "/")) != NULL) {
		tok = strtok(tok, ".");
	} else {
		tok = strtok((char *)argv[1], ".");
	}

	char choice = 'z';
	printf("[a]nalyze graph or [r]un simulation: ");
	scanf("%c", &choice);

	if (choice == 'a')
		graphStats(tok);
	else
		runSimulation(tok);

	free(temp);
	free(graph);

	t2 = clock() - t2; // Done processing

	printf("\nGraph Read Runtime: %lu clicks (%.3f seconds)\n", t1, (((float)t1)/CLOCKS_PER_SEC));
	printf("Graph Process Runtime: %lu clicks (%.3f seconds)\n", t2, (((float)t2)/CLOCKS_PER_SEC));
	printf("Total Runtime: %lu clicks (%.3f seconds)\n", t1+t2, (((float)t1+t2)/CLOCKS_PER_SEC));

	return 0;
}
