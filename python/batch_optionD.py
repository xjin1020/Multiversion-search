#!/usr/bin/python3

import sys
import subprocess

list = []

with open('queries_temp.txt', 'r') as f:
    for line in f:
        q = line.strip()
        length = len(q.split(' '))
        list.append((q, length))

for (q, length) in list:
    print(q + " " + str(length))
    # need to run twice because the first run will eliminate the cache behaviour
    comm = './run_optionD.sh "' + q + '" ' + str(length) + '&> /dev/null'
    subprocess.call(comm, shell = True)
    comm2 = './run_optionD.sh "' + q + '" ' + str(length)
    subprocess.call(comm2, shell = True)
