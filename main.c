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
    {"output",  'o', "FILE", 0, "Use FILE instead of 'web/data.json' or 'web/infData.json'. Note that data in 'web/' is not changed, so JavaScript graphs aren't updated."},
    {"infect",  'r', "TYPE", OPTION_ARG_OPTIONAL, "Run an SIR (or derivative) simulation on the graph. TYPE determines the type of infection to model."},

    {0,0,0,0, "Simulation options:"},
    {"pinfect",  'i', "PROB", 0, "Probability of an infected node infecting a susceptible neighbor"},
    {"pcontact", 'c', "PROB", 0, "Probability of two nodes with an edge of coming into contact"},
    {"kval",     'k', "k",    0, "Maximum number of neighbors an infected node may infect"},
    {"infecttime",'p',"steps",0, "Infectious period; how long a node stays infected once infected"},
    {"duration", 'd', "steps",0, "Maximum number of time steps"},
    {0}
};
static error_t parse_opt(int key, char *arg, struct argp_state *state) {
    struct arguments *arguments = state->input;

    switch(key) {
        case 'o':
            arguments->outfile = arg;
            break;
        case 'a':
            arguments->action = 'a';
            break;
        case 'r':
            arguments->action = 'r';
            arguments->type = arg ? *arg : 0; // only first character matters anyway
            break;
        case 'i':
            arguments->infectiousProbability = (float)atof(arg);
            break;
        case 'c':
            arguments->contactChance = (float)atof(arg);
            break;
        case 'k':
            arguments->kVal = atof(arg);
            break;
        case 'p':
            arguments->infectiousPeriod = atoi(arg);
            break;
        case 'd':
            arguments->simulDuration = atoi(arg);
            break;
        case ARGP_KEY_ARG:
             /* from: https://www.gnu.org/software/libc/manual/html_node/Argp-Example-4.html
             state->arg_num == 0 since we force this to be parsed last */
            arguments->graph_path = arg;

            // force argp to stop parsing
            state->next = state->argc;
            break;
        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}
static struct argp argp = {options, parse_opt, args_doc, doc};

int main(int argc, char **argv) {

    // define defaults
    arguments.outfile = "";
    arguments.action = 0;
    arguments.type = 0;
    arguments.infectiousProbability = -1;
    arguments.contactChance = -1;
    arguments.kVal = -1;
    arguments.infectiousPeriod = -1;
    arguments.simulDuration = -1;

    //printf("%c %c %f %f %d %d %d", arguments.action, arguments.type, arguments.infectiousProbability,  arguments.contactChance, arguments.kVal, arguments.infectiousPeriod, arguments.simulDuration);

    argp_parse(&argp, argc, argv, 0, 0, &arguments);

    // TODO: see how to use argp to do this
	/*if (argc != 2) {
		fprintf(stderr, "Usage: %s <graphFile>\n", argv[0]);
		fprintf(stderr, "<graphFile>: Name of graph to test\n");
		exit(1);
	}*/

	clock_t t1, t2;
	t1 = clock();

	readGraph(arguments.graph_path);

	t1 = clock() - t1; // Done reading
	t2 = clock();

    char *temp = (char *)malloc(strlen(arguments.graph_path)+1);
	strcpy(temp, arguments.graph_path);

	char *tok = strtok(temp, "/");

	if ((tok = strtok(NULL, "/")) != NULL) {
		tok = strtok(tok, ".");
	} else {
		tok = strtok((char *)arguments.graph_path, ".");
	}

	char choice = arguments.action;
    if (choice == 0) {
        printf("[a]nalyze graph or [r]un simulation: ");
        scanf("%c", &choice);
    }

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
