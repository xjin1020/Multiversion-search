#!/usr/bin/python
fin = open ('out_options_TP', 'r')
lines = fin.readlines()
total = []
query_number = 50
for i in range(len(lines)):
    line = lines[i]
    if line[0] == 't' and lines[i-1][0] == 'C':
        tokens = line.rstrip('\n').split(' ')
        total.append(float(tokens[-1]))
for i in range(query_number):
    print total[i]
