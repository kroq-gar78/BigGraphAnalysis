#!/usr/bin/env python2

import numpy as np
import scipy as sp

from data_reader import load_infData

from glob import glob

# gets peaks for all trials of input data
# returned structure is of form peaks[methods][vacc_rates][stages][trials]
def get_peaks(data_graph, stages=['sus','inf','rec','newinf']):
    graph_peaks = {}
    for method in data_graph.iterkeys():
        graph_peaks[method] = [None]*len(data_graph[method])
        for i, vacc_rate in enumerate(data_graph[method]): # vaccination rates
            graph_peaks[method][i] = [None]*len(vacc_rate)
            for j, trial in enumerate(data_graph[method][i]): # iterations
                #for stage, x in data_graph[method][i][j].items(): print stage, np.amax(x[:,1])
                graph_peaks[method][i][j] = {stage: np.amax(x[:,1]) for stage, x in trial.items()}
            #for stage in stages:
                ##graph1_peaks[method][i][stage] = [None]*len(vals)
                #graph_peaks[method][i][stage] = [np.amax(x[stage][:,1]) for x in data_graph[method][i]]
    return graph_peaks

# condenses the peaks by trials using a given function
# returned structure is of form peaks[methods][vacc_rates][stages]
def peaks_condense_trials(peaks, func, stages=['sus','inf','rec','newinf']):
    peaks_condensed = {}
    for method in peaks.iterkeys():
        peaks_condensed[method] = [None]*len(peaks[method])
        for i, vacc_rate in enumerate(peaks[method]): # vaccination rates
            peaks_condensed[method][i] = {}
            for stage in stages:
                #print [x[stage] for x in vacc_rate]
                peaks_condensed[method][i][stage] = func([x[stage] for x in vacc_rate])
    return peaks_condensed

# condense all trials through average
def get_peaks_avg(peaks, stages=['sus','inf','rec','newinf']):
    #graph_peaks_avg = {}
    #for method in peaks.iterkeys():
        #graph_peaks_avg[method] = [None]*len(peaks[method])
        #for i, vacc_rate in enumerate(peaks[method]): # vaccination rates
            #graph_peaks_avg[method][i] = {}
            #for stage in stages:
                #graph_peaks_avg[method][i][stage] = np.mean([x[stage] for x in vacc_rate])
    #return graph_peaks_avg
    return peaks_condense_trials(peaks, np.mean, stages)

def get_peaks_highest(peaks, stages=['sus','inf','rec','newinf']):
    return peaks_condense_trials(peaks, max, stages)

# Normalize everything by the number of non-vaccinated individuals.
# Requires 'sus', 'inf', and 'rec' (this one is optional) stages to calculate
# total unvaccinated population.
def norm_unvacc(data_graph, stages=['sus','inf','rec','newinf']):
    normalized = {}
    for method in data_graph.iterkeys():
        normalized[method] = [None]*len(data_graph[method])
        for i, vacc_rate in enumerate(data_graph[method]): # vaccination rates
            normalized[method][i] = [None]*len(vacc_rate)
            for j, trial in enumerate(data_graph[method][i]): # iterations
                normalized[method][i][j] = {}
                unvaccinated = trial['sus'][0,1] + trial['inf'][0,1]
                if 'rec' in trial: unvaccinated += trial['rec'][0,1]
                for stage in stages:
                    normalized[method][i][j][stage] = trial[stage].astype(float)
                    normalized[method][i][j][stage] /= unvaccinated
    return normalized

#def get_peaks_avg(data_graph):
    #graph_peaks_avg = {}
    #stages = ['sus','inf','rec','newinf']
    #for method in data_graph.iterkeys():
        #graph_peaks_avg[method] = [None]*len(data_graph[method])
        #for i in xrange(len(data_graph[method])):
            #graph_peaks_avg[method][i] = {}
            #for stage in stages:
                ##graph1_peaks[method][i][stage] = [None]*len(vals)
                #graph_peaks_avg[method][i][stage] = np.average([np.amax(x[stage][:,1]) for x in data_graph[method][i]])
    #return graph_peaks_avg

#def get_peaks_highest(data_graph):
    #graph_peaks_highest = {}
    #stages = ['sus','inf','rec']
    #for method in data_graph.iterkeys():
        #graph_peaks_highest[method] = [None]*len(data_graph[method])
        #for i in xrange(len(data_graph[method])):
            #graph_peaks_highest[method][i] = {}
            #for stage in stages:
                ##graph1_peaks[method][i][stage] = [None]*len(vals)
                #graph_peaks_highest[method][i][stage] = 0
                #for j in xrange(len(data_graph[method][i])):
                    ##print data_graph1[method][i][j]
                    #tmp_max = np.amax(data_graph[method][i][j][stage])
                    #graph_peaks_highest[method][i][stage] = max(graph_peaks_highest[method][i][stage] , tmp_max)

    #return graph_peaks_highest

if __name__=="__main__":
    methods=["deg","ev","pr", "rnd"]
    #vals=[20,40,60,80]
    vacc_rates = [10, 20, 30, 35, 40, 45, 50, 55, 60, 70, 80, 90]

    graph1_dir = "../output/2015-08-12T17-48-39"
    data_graph1 = {}
    for method in methods:
        #print files
        data_graph1[method] = [None]*len(vacc_rates)
        for i in xrange(len(vacc_rates)):
            files = glob(graph1_dir+"/"+method+"_"+str(vacc_rates[i])+"*.json")
            #print len(files), map(lambda x: x[x.rfind("/"):], sorted(files))
            data_graph1[method][i] = [None]*len(files)
            for j in xrange(len(files)):
                #print files[j]
                data_graph1[method][i][j] = load_infData(files[j])

    exit(0)
    graph2_dir = "output/2015-08-07_graph2"
    data_graph2 = {}
    for method in methods:
        data_graph2[method] = [None]*len(vals)
        for i in xrange(len(vals)):
            files = glob(graph2_dir+"/"+method+"_"+str(vals[i])+"*.json")
            data_graph2[method][i] = [None]*len(files)
            for j in xrange(len(files)):
                data_graph2[method][i][j] = load_infData(files[j])

    # get the peaks for each run (for both graphs)
    #graph1_peaks = get_peaks(data_graph1)
    #graph2_peaks = get_peaks(data_graph2)
    #graph1_peaks = {}
    #graph2_peaks = {}
    #stages = ['sus','inf','rec']
    #for method in data_graph1.iterkeys():
        #graph1_peaks[method] = [None]*len(data_graph1[method])
        #graph2_peaks[method] = [None]*len(data_graph2[method])
        #for i in xrange(len(data_graph1[method])):
            #graph1_peaks[method][i] = {}
            #graph2_peaks[method][i] = {}
            #for stage in stages:
                ##graph1_peaks[method][i][stage] = [None]*len(vals)
                #graph1_peaks[method][i][stage] = 0
                #graph2_peaks[method][i][stage] = 0
                #for j in xrange(len(data_graph1[method][i])):
                    ##print data_graph1[method][i][j]
                    #tmp_max = np.amax(data_graph1[method][i][j][stage])
                    #graph1_peaks[method][i][stage] = max(graph1_peaks[method][i][stage] , tmp_max)

                    #tmp_max = np.amax(data_graph2[method][i][j][stage])
                    #graph2_peaks[method][i][stage] = max(graph2_peaks[method][i][stage] , tmp_max)

