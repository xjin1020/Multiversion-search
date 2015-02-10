#!/usr/bin/python
fin = open('outputD', 'r')
lines = fin.readlines()
totaltime=0
for i in range(len(lines)):
    line = lines[i]
    if line[0] == 'C':
        tokens = line.rstrip('\n').split(' ')
        time = float(tokens[3][:-2])
        totaltime = totaltime + time
second_last_line = lines[len(lines)-2]
if second_last_line[0] != 'v':
    print "total time:", totaltime, "ms"
else:
    print "not enough, total time:", totaltime, "ms\t#count #cluster=", lines[len(lines)-1]

