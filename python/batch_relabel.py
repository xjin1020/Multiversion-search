#!/usr/bin/python3

import sys
import subprocess

subprocess.call("rm -rf stat.txt", shell=True)

list = []

with open('queries.txt', 'r') as f:
    for line in f:
        q = line.strip()
        list.append(q)

for q in list:
    comm = './reuse_label.py "' + q + '" >> stat.txt'
    subprocess.call(comm, shell = True)
