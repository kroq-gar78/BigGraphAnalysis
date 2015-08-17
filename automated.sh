#!/bin/bash -u

# automate the execution of a number of trials of each method at different
# vaccination rates
# Only argument is the graph file
# Output is stored in "output/" directory under time

graph="$1"

vacc_root="vacc_"
methods=("deg" "ev" "katz") # methods of centrality; ev = eigenvector
vals=(20 40 60 80) # percentages of vaccination
trials=5

outdir="output/$(date +'%FT%H-%M-%S')"
mkdir -p "$outdir"

for method in "${methods[@]}"
do
    for val in "${vals[@]}"
    do
        #for i in {0..$trials}
        for((i=0; i<$trials; i++))
        do
            #./ReadGraph "$graph" "${vacc_root}_${method}${val}"
            cp -va web/infData.json "${outdir}/${method}_${val}_${i}.json"
        done
    done
done
