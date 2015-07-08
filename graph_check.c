#include <stdlib.h>
#include <check.h>
#include "graph.h"

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

Suite *graph_suite() {
	Suite *s;
	TCase *tc_core;

	s = suite_create("Graph");
	tc_core = tcase_create("Core");

	tcase_add_test(tc_core, ring_test);
	tcase_add_test(tc_core, undirected_test);
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
