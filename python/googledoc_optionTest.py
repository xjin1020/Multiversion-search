#!/usr/bin/python
finA = open('outTestA_f_avg', 'r')
finC = open('outTestC_f_avg', 'r')
linesA = finA.readlines()
finA.close()
linesC = finC.readlines()
finC.close()
max_length = 10 # max query length
k=1000#!!!User!!!
#initialize lists
f = []
timeA = []
timeC = []
totalA = 0.0
totalC = 0.0
totalD = 0.0
for i in range(max_length):
    f.append(0)
    timeA.append(0.0)
    timeC.append(0.0)
for i in range(len(linesA)):
    # deal with each query one by one:
    lineA = linesA[i]
    if lineA[0] != '*':
        continue;
    # deal with one query
    number_of_count = k 
    increase=1
    lineA = linesA[i+increase]
    lineNextA = linesA[i+increase+1]
    query_len = 0
    while lineNextA[0] != '*': # while next line is not "*****"
        if lineA[0] == 't' and lineA[1] == 'o' and lineA[2] == 't' and lineA[3] == 'a' and lineA[4] == 'l': # found a "total time..."
            query_len = query_len+1
            tokens = lineA.rstrip('\n').split(' ')
            termNumber = int(tokens[5])
            timeA[termNumber] = float(tokens[9])
            f[termNumber] = int(tokens[12])
            lineC = linesC[i+increase]
            tokens = lineC.rstrip('\n').split(' ')
            timeC[termNumber] = float(tokens[9])
        if lineA[0] == 'n' and lineA[1] == 'o' and lineA[2] == 't':#find "not enough..."
            tokens = lineA.rstrip('\n').split(' ')
            number_of_count = int(tokens[-2])
            break;
        increase = increase+1
        if i+increase+1 >= len(linesA): # last query
            break;
        lineA = linesA[i+increase]
        lineNextA = linesA[i+increase+1]
    # print this query's results
    for j in range(query_len):
        print f[j],'\t',
    print 
    for j in range(query_len):
        timeA[j] = timeA[j] / number_of_count * k # adjust timeA
        print timeA[j],'\t',
        totalA = totalA + timeA[j]
    print 
    for j in range(query_len):
        timeC[j] = timeC[j] / number_of_count * k # adjust timeC
        print timeC[j],'\t',
        totalC = totalC + timeC[j]
    print 
    for j in range(query_len):
        if timeA[j] < timeC[j]:
            totalD = totalD + timeA[j]
        else:
            totalD = totalD + timeC[j]
print 'Avg Time A=',totalA/50
print 'Avg Time C=',totalC/50
print 'Avg Time D=',totalD/50

