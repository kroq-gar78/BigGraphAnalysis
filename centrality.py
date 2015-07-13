# coding: utf-8

from graph_tool.all import *

def cen_degree(g):
    return g.degree_property_map("total").a / (g.num_vertices() - 1)

def cen_katz(g):
    return katz(g).a

def cen_eigenvector(g):
    return _,eigenvector(g).a # throw out the largest eigenvalue for now

def cen_pagerank(g):
    return pagerank(g).a

# get the 'n' largest elements and return their indecies
def nlargest_idx(a, n):
    return np.argpartition(a, -n)[-n:]

def write_nodes(a, fname):
    f = open(fname, 'w')
    for i in a:
        f.write("%d\n" % (i+1))
    f.close()
