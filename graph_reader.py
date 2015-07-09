#!/usr/bin/env python2

# load a graph from a file
# format for rows is: "vertex1 vertex2 edgeWeight"
# number of vertices is taken to be the highest vertex number

from graph_tool.all import *
import numpy as np
import scipy as sp

import sys

def load_graph(infile):
    inmatrix = np.loadtxt(infile, dtype=np.dtype('uint32'), delimiter=" ")
    numv = np.amax(inmatrix[:,0:2])

    #print numv, inmatrix[:,0:2]

    g = Graph(directed=False)
    edge_weights = g.new_edge_property("double")
    g.edge_properties["weights"] = edge_weights
    vlist = list(g.add_vertex(numv))

    for i in inmatrix:
        edge = g.add_edge(vlist[i[0]-1], vlist[i[1]-1]) # need to convert from 1-based index in file to 0-based
        edge_weights[edge] = i[2]

    return g

if __name__ == "__main__":
    fname = sys.argv[1]

    g = load_graph(fname)
