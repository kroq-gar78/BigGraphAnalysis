#!/usr/bin/env python2

# generate a graph based on a normal distribution

import scipy as sp
import scipy.stats
from graph_tool import Graph

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
# uses a naive/greedy approach, works mostly well (on large graphs)
def normal_dist_graph(mean=10, stdev=5, num_vtx=100):
    g = Graph(directed=False)
    vtx = list(g.add_vertex(num_vtx))

    # make sure all points in distribution are within bounds
    dist = []
    while(len(dist) < num_vtx):
        extra = sp.stats.norm.rvs(loc=mean, scale=stdev, size=num_vtx-len(dist))
        dist.extend(extra)
        dist = filter(lambda x: not (x < 1 or x >= num_vtx), dist) # remove points out of degree bounds
        print len(dist), num_vtx

    #print len(dist), num_vtx
    #return
    dist = map(lambda x: int(round(x)) , dist)

    # can't have odd sum of degrees, so add 1 to random vertex
    if sum(dist)&1==1:
        i_rand = random.randrange(num_vtx)
        dist[i_rand] += 1

    needed = dist[:]
    vtx_short = vtx[:] # "template" list for nodes to copy

    for i in xrange(len(vtx)):

        if needed[i] > 0:
            print "%4d, needed: %d" % (i, needed[i])
            vtx_tmp = vtx_short[:]
            vtx_tmp.remove(vtx[i])
        else: continue

        # repeat for as many nodes as needed
        neighbors = list(vtx[i].all_neighbours())
        for j in xrange( needed[i] ):
            if(len(vtx_tmp) > 0): dest = random.choice(vtx_tmp)
            else: print "give up 1, needed: %d" % needed[i]; break # give up; stop adding more edges
            while(dest in neighbors or needed[g.vertex_index[dest]]<=0):
                vtx_tmp.remove(dest)
                if(needed[g.vertex_index[dest]]<=0):
                    vtx_short.remove(dest)
                if(len(vtx_tmp) > 0): dest = random.choice(vtx_tmp)
                else: print "give up 2, needed %d" % needed[i]; break # give up; use final node in `vtx_tmp`
            #print vertex_degree(dest) > dist[g.vertex_index[dest]]
            edge = g.add_edge(vtx[i], dest)
            needed[i] -= 1
            needed[g.vertex_index[dest]] -= 1
            if(len(vtx_tmp) > 0): vtx_tmp.remove(dest)

        if (needed[i] != 0):
            print "ERROR: (degree(vtx[%d]) = %d) != %d" % (i, vertex_degree(vtx[i]), dist[i])
            #break

    print "total expected edges: %d" % (sum(dist)/2)
    return g

# generate a ring that is connected to neighbors with distance 'k'
# also, redirect edges using probability 'p' (0 = ring, 1 = random graph)
def ring(num_vtx=100, k=2, p=0.0):
    g = Graph(directed=False)
    vtx = list(g.add_vertex(num_vtx))

    # connect neighbors
    for i in vtx:
        for j in xrange(-k,k+1):
            if j==0: continue
            dest = g.vertex( (g.vertex_index[i]+j)%num_vtx )
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
    #normal_dist_graph(mean=10, stdev=5, num_vtx=10000)
    ring(num_vtx=100, k=2, p=0.5)

