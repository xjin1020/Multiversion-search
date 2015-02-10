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
best_t2 = 0
best_t3 = 0
best_t4 = 0
best_t5 = 0
best_t6 = 0
best_avg =99999.0
for t2 in range(0,10):
    for t3 in range(0,10):
        for t4 in range(0,10):
            for t5 in range(0,20):
                for t6 in range(0,22):
                    timeD = 0.0
                    for i in range(query_num):
                        if len(f[i]) == 2:
                            t = t2
                        elif len(f[i]) == 3:
                            t = t3
                        elif len(f[i]) == 4:
                            t = t4
                        elif len(f[i]) == 5:
                            t = t5
                        else:
                            t = t6
                        for j in range(len(f[i])):
                            if f[i][j] < t:
                                # choose A:
                                timeD = timeD + timeA[i][j]
                            else:
                                timeD = timeD + timeC[i][j]
                    avg = timeD/query_num
                    if avg < best_avg:
                        best_t2 = t2
                        best_t3 = t3
                        best_t4 = t4
                        best_t5 = t5
                        best_t6 = t6
                        best_avg = avg
print 'Best_avg = ', best_avg, ' when t2, t3, t4, t5, t6 =', best_t2, ',', best_t3, ',', best_t4, ',', best_t5, ',', best_t6
    
    
