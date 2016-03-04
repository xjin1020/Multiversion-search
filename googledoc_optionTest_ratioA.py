#!/usr/bin/python
finA = open('outTestA_ratio_vector', 'r')
linesA = finA.readlines()
finA.close()
for i in range(len(linesA)):
    lineA = linesA[i]
#    if lineA[0] == 'R' and lineA[1] == 'u' and lineA[2] == 'n':
#        print lineA
    if lineA[0] == 'c' and lineA[1] == 'l' and lineA[2] == 'u':
        tokens = lineA.rstrip('\n').split('  ')
        cluster1 = tokens[1]
        time1 = tokens[5]
        count1 = tokens[7]
        f1 = tokens[9]
#        print cluster1,'\t',
        print time1,'\t',
        print count1,'\t',
#        print f1,'\t',
        print
