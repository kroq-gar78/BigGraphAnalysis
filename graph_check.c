#include <stdlib.h>
#include <time.h>
#include <check.h>
#include "graph.h"
#include "simulation.h"

// When we generate a Ring graph. each node should have at most 2 neighbors
START_TEST (ring_test)
{
	readGraph("ring.txt");

	int i;
	for (i = 0; i <= highestNode; i++) {
		if (graph[i] != NULL) {
			int neighbors = countDegree(graph[i]);
			ck_assert_msg(neighbors == 2, "Was expecting 2 neighbors. Got %d", neighbors);
		}
	}

	free(graph);
}
END_TEST

// Ensure that the graph data structure is undirected (i.e. A <-> B)
START_TEST (undirected_test)
{
	readGraph("MEDIUM_10M/astro-ph.txt");

	int i;
	for (i = 0; i <= highestNode; i++) {
		if (graph[i] != NULL) {
			Node *temp = graph[i]->next;

			while (temp != NULL) {
				ck_assert_msg(checkConnection(graph[temp->vertexNum], i),
					"Node %d is connected to %d but not vice versa", i, temp->vertexNum);

				temp = temp->next;
			}
		}
	}

	free(graph);
}
END_TEST

START_TEST (simul_init_test)
{
	readGraph("MEDIUM_10M/rgg_n_2_17_s0.txt");

	int *susceptible, *infected, *recovered;

	susceptible = (int *)malloc(sizeof(int));
	infected    = (int *)malloc(sizeof(int));
	recovered   = (int *)malloc(sizeof(int));

	*susceptible = *infected = *recovered = 0;

	srand(time(NULL));
	seedInfection();
	countNodes(0, infected, recovered, susceptible);

	ck_assert_msg(infected[0] == 1,  "There should be only 1 infection: %d", infected[0]);
	ck_assert_msg(recovered[0] == 0, "There should be no recovered agents: %d", recovered[0]);
	
	free(graph);
	free(susceptible);
	free(infected);
	free(recovered);
}
END_TEST

Suite *graph_suite() {
	Suite *s;
	TCase *tc_core;

	s = suite_create("Graph");
	tc_core = tcase_create("Core");

	tcase_add_test(tc_core, ring_test);
	tcase_add_test(tc_core, undirected_test);
	tcase_add_test(tc_core, simul_init_test);
	suite_add_tcase(s, tc_core);

	return s;
}

int main(int argc, char const *argv[]) {
	
	int numFailed;
	Suite *s;
	SRunner *sr;

	s = graph_suite();
	sr = srunner_create(s);

	srunner_run_all(sr, CK_NORMAL);
	numFailed = srunner_ntests_failed(sr);
	srunner_free(sr);

	return (numFailed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
