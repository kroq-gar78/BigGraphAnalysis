#!/usr/bin/env python2

# generate a graph based on a normal distribution

import scipy as sp
import scipy.stats
from graph_tool.all import *

import random

def write_graph(g, outfile):
    f = open(outfile, "w")
    for i in g.edges():
        idx_src = g.vertex_index[i.source()]+1
        idx_dest = g.vertex_index[i.target()]+1
        f.write("%d %d 1\n" % (idx_src, idx_dest))

def vertex_degree(vtx):
    return len(list(vtx.all_edges()))

# build a random graph based on a degree distribution
# uses a naive/greedy approach, doesn't quite work
def normal_dist_graph(mean=10, stdev=5, num_vtx=100):
    g = Graph(directed=False)
    vtx = list(g.add_vertex(num_vtx))
    dist = sp.stats.norm.rvs(loc=mean, scale=stdev, size=num_vtx)
    dist = map(lambda x: 1 if x < 1 else x , dist) # lower limit; assume connected
    dist = map(lambda x: num_vtx-1 if x >= num_vtx else x , dist) # upper limit
    dist = map(lambda x: int(round(x)) , dist)
    print dist
    for i in xrange(len(vtx)):
        vtx_tmp = vtx[:]
        vtx_tmp.remove(vtx[i])

        # repeat for as many nodes as needed
        needed = dist[i] - vertex_degree(vtx[i])
        print "needed: %d" % needed
        neighbors = list(vtx[i].all_neighbours())
        for j in xrange( needed ):
            if(len(vtx_tmp) > 0): dest = random.choice(vtx_tmp)
            else: break
            while(dest in neighbors and vertex_degree(dest)>dist[g.vertex_index[dest]]):
                vtx_tmp.remove(dest)
                if(len(vtx_tmp) > 0): dest = random.choice(vtx_tmp)
                else: break
            edge = g.add_edge(vtx[i], dest)
            vtx_tmp.remove(dest)

        if (vertex_degree(vtx[i]) != dist[i]):
            print "ERROR: (degree(vtx[%d]) = %d) != %d" % (i, vertex_degree((vtx[i]), dist[i])
            #break

    return g

# generate a ring that is connected to neighbors with distance 'k'
# also, redirect edges using probability 'p' (0 = ring, 1 = random graph)
def ring(num_vtx=100, k=2, p=0.0):
    g = Graph(directed=False)
    vtx = list(g.add_vertex(num_vtx))

    # connect neighbors
    for i in vtx:
        for j in xrange(1,k+1):
            dest = g.vertex( (g.vertex_index[i]-j)%num_vtx )
            if(g.edge(i,dest) is None):
                g.add_edge(i,dest)

    # redirect edges
    #old_edges = list(g.edges())
    old_edges = [(x.source(), x.target()) for x in g.edges()]
    for i in old_edges:
        n = random.random()
        if n < p: # redirect edge; choose random vertex as new destination
            vtx_tmp = vtx[:]
            vtx_tmp.remove(i[1])
            if i[0] in vtx_tmp: vtx_tmp.remove(i[0])
            dest = random.choice(vtx_tmp)
            while(g.edge(i[0],dest) is not None):
                vtx_tmp.remove(dest)
                dest = random.choice(vtx_tmp)

            g.remove_edge(g.edge(i[0], i[1]))
            g.add_edge(i[0], dest)

    return g

if __name__ == "__main__":
    #normal_dist_graph(mean=10, stdev=5, num_vtx=100)
    ring(num_vtx=100, k=2, p=0.5)

