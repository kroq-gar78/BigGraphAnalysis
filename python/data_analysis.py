#!/usr/bin/env python2

import numpy as np
import scipy as sp

from data_reader import load_infData

from glob import glob

if __name__=="__main__":
    methods=["deg","ev","katz"]
    vals=[20,40,60,80]

    data = {}
    for method in methods:
        files = glob(methods[0]+"_"+str(vals[0])+"*.json")
        data[method] = [None]*len(vals)
        for i in xrange(len(vals)):
            data[method][i] = [None]*5
            for j in xrange(len(files)):
                data[method][i][j] = load_infData(files[j])
