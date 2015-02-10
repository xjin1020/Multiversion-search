#!/usr/bin/python
fin = open('out_intersection_phase2_longest', 'r')
lines = fin.readlines()
fin.close()
k=1000#!!!User!!!
query_number = 50
#initialize lists
time = []
count = []
for i in range(query_number):
    time.append(0.0)
    count.append(k)
total = 0.0
queryth = -1
for i in range(len(lines)):
    line = lines[i]
    if line[0] == 't' and line[1] == 'o' and line[2] == 't' and line[3] == 'a' and line[4] == 'l': # found a "total time..."
        queryth = queryth + 1
        tokens = line.rstrip('\n').split(' ')
        time[queryth] = float(tokens[-2])
    if line[0] == 'n' and line[1] == 'o' and line[2] == 't':#find "not enough..."
        queryth = queryth + 1
        tokens = line.rstrip('\n').split(' ')
        count[queryth] = int(tokens[-2])
        time[queryth] = float(tokens[4])
for i in range(query_number):
    time[i] = time[i] / count[i] * k
    total = total + time[i]
print 'Avg Intersection Time =',total/query_number

