#!/usr/bin/python

infile = open('network.dat', 'r')
contents = infile.read()
infile.close()
contents = contents.replace('\t', ' ')

output = open('network.dat', 'w')
output.write(contents)
output.close()
