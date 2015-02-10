#!/usr/bin/python
fin = open ('out_sigir12_phase1', 'r')
lines = fin.readlines()
inter = []
score = []
total = []
query_number = 50
for i in range(len(lines)):
    line = lines[i]
    if line[0] == 'i' and lines[i-1][0] == 'R':
        tokens = line.rstrip('\n').split(' ')
        inter.append(float(tokens[2]))
    if line[0] == 'S':
        tokens = line.rstrip('\n').split(' ')
        score.append(float(tokens[1]))
        total.append(score[-1]+inter[-1])
for i in range(query_number):
    print inter[i],'\t',score[i],'\t',total[i]
