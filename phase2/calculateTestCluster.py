#!/usr/bin/python
query_len = 6#!!!
k = 1000#!!!
fin = open('outputTestCluster', 'r')
lines = fin.readlines()
f=[]
avgf=[]
time=[]
totaltime=[]
for i in range(query_len):
    totaltime.append(float(0))
    avgf.append(0)
cluster_num=0#number of clusters
print -99999
# initialize all terms' time to 0
for i in range(len(lines)):
    line = lines[i]
    # if beginning with 'Calculate'
    if line[0] == 'C':
        tokens = line.rstrip('\n').split(' ')
        time.append(float(tokens[4]))
        f.append(int(tokens[-1]))
        j=int(tokens[2])
        if j == 0:
            cluster_num = cluster_num+1
second_last_line = lines[len(lines)-2]
number_of_count = k
if second_last_line[0] == 'v':
    #print "not enough", " #count #cluster=", lines[len(lines)-1]
    tokens = lines[len(lines)-1].rstrip('\n').split(' ')
    number_of_count = int(tokens[-2])
print query_len, cluster_num
for i in range(cluster_num*query_len):
    term = i % query_len
    time[i] = time[i] / number_of_count * k
    print f[i], time[i],
    totaltime[term] = totaltime[term]+time[i]
    avgf[term] = avgf[term] + f[i]
print
if 1:
    for i in range(query_len):
        print totaltime[i],
    print
    for i in range(query_len):
        print avgf[i]/cluster_num,
    print

