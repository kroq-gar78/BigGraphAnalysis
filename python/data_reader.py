#!/usr/bin/env python2

import matplotlib.pyplot as plt
import numpy as np
import scipy as sp

import json

def load_infData(path):
    states = {} # new infections and al 3 stages
    f = open(path,'r')
    data = json.load(f)

    newInfections = np.zeros((data['roundsNeeded'], 2), dtype=int)
    for i in data['values']:
        newInfections[i["x"],0] = i["x"]
        newInfections[i["x"],1] = i["y"]
    states['newinf'] = newInfections

    #plot(newInfections[:,0], newInfections[:,1])

    states['inf'] = np.zeros((data['roundsNeeded'], 2), dtype=int)
    states['rec'] = np.zeros((data['roundsNeeded'], 2), dtype=int)
    states['sus'] = np.zeros((data['roundsNeeded'], 2), dtype=int)
    for i in data['numInf']:
        states['inf'][i["x"],0] = i["x"]
        states['inf'][i["x"],1] = i["y"]
    for i in data['numRec']:
        states['rec'][i["x"],0] = i["x"]
        states['rec'][i["x"],1] = i["y"]
    for i in data['numSus']:
        states['sus'][i["x"],0] = i["x"]
        states['sus'][i["x"],1] = i["y"]

    #plt.plot(states['inf'][:,0], states['inf'][:,1], 'r',
         #states['rec'][:,0], states['rec'][:,1], 'b',
         #states['sus'][:,0], states['sus'][:,1], 'g')
    #plt.show()

    return states

if __name__=="__main__":
    states = load_infData("../web/infData.json")

    #plot(newInfections[:,0], newInfections[:,1])

    plt.plot(states['inf'][:,0], states['inf'][:,1], 'r',
         states['rec'][:,0], states['rec'][:,1], 'b',
         states['sus'][:,0], states['sus'][:,1], 'g')
    plt.show()
