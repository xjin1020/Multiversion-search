#!/usr/bin/python3

import sys
import subprocess

list = []

with open('queries_all.txt', 'r') as f:
    for line in f:
        q = line.strip()
        length = len(q.split(' '))
        list.append((q, length))

for (q, length) in list:
    print(q + " " + str(length))
    comm = './run_optionTest_ratioA.sh "' + q + '" ' + str(length)
    subprocess.call(comm, shell = True)
