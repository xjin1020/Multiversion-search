#!/usr/bin/python

# This file is used to modify the output data.

query_len = 6#!!!
fin = open('outputTest', 'r')
lines = fin.readlines()
# print time
times=[]
nums=[]
fs=[]
# initialize all terms' time to 0
for i in range(query_len):
    times.append(0.0)
    nums.append(0)
    fs.append(0)
for j in range(len(lines)):
    line = lines[j]
    if line[0] == 'w':
        tokens = line.rstrip('\n').split(' ')
        cluster_id = tokens[3]
    if line[0] == 'C' and line[1] == 'a':
        tokens = line.rstrip('\n').split(' ')
        index = int(tokens[2])
        time = float(tokens[4])
        times[index] = time
	f = int(tokens[7])
	fs[index] = f;
    if line[0] == 'C' and line[1] == 'o':
        tokens = line.rstrip('\n').split(' ')
        index = int(tokens[4])
        num = tokens[2]
        nums[index] = num
    if line[0] == '!' and line[1]== '!':
        for i in range(query_len):
            print "cluster: ",cluster_id," term: ",i," time: ",times[i]," num: ",nums[i]," f: ",fs[i]
        print "finish cluster ", cluster_id
