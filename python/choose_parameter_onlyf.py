#!/usr/bin/python
fin = open('ft', 'r')
query_num = 50
f = [[] for i in range(query_num)]
timeA = [[] for i in range(query_num)]
timeC = [[] for i in range(query_num)]
lines = fin.readlines()
for i in range(query_num):
    linef = lines[i*3]
    linetimeA = lines[i*3+1]
    linetimeC = lines[i*3+2]
    f[i] = [int(x) for x in linef.split()]
    timeA[i] = [float(x) for x in linetimeA.split()]
    timeC[i] = [float(x) for x in linetimeC.split()]
best_t = 0
best_avg =99999.0
for t in range(100):
    print 'Threshould=',t,':',
    timeD = 0.0
    for i in range(query_num):
        for j in range(len(f[i])):
            if f[i][j] < t:
                # choose A:
                timeD = timeD + timeA[i][j]
            else:
                timeD = timeD + timeC[i][j]
    avg = timeD/query_num
    if avg < best_avg:
        best_t = t
        best_avg = avg
    print avg
print 'Best_avg = ', best_avg, ' when threshold = ', best_t
    
    
