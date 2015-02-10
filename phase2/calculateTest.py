#!/usr/bin/python
query_len = 6#!!!
fin = open('outputTest', 'r')
lines = fin.readlines()
totaltime=[]
f=[]
cluster_num=0#number of clusters
# initialize all terms' time to 0
for i in range(query_len):
    totaltime.append(0.0)
    f.append(0)
for i in range(len(lines)):
    line = lines[i]
    # if beginning with 'Calculate'
    if line[0] == 'C':
        tokens = line.rstrip('\n').split(' ')
        time = float(tokens[4])
        for j in range(query_len):
            if int(tokens[2]) == j:
                totaltime[j] = totaltime[j] + time
                f[j] = f[j] + int(tokens[-1])
                if j == 0:
                    cluster_num = cluster_num+1
second_last_line = lines[len(lines)-2]
# print time
for j in range(query_len):
    f[j] = f[j] / cluster_num
    print "total time of term ",j," : ", totaltime[j], "ms", "f=", f[j]
if second_last_line[0] == 'v':
    print "not enough", " #count #cluster=", lines[len(lines)-1]

