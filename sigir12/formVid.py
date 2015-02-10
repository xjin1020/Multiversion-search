#!/usr/bin/python3

import sys
import os
import re
import subprocess

dest = "datasetsExtractVid/"

# parse query and their results into dict of page => query
d = {}
with open('./level1/level1_results.txt', 'r') as results:
    for line in results:
        if re.match(r'Searching for: ', line):
            query = re.sub(r'Searching for: ', "", line)
            query = query.rstrip()
            l = []
            d[query] = l
        elif re.match(r'No-IO-Time:', line):
            print("Level1 Search " + line)
        elif re.match(r'^.*/phase1/fragment/result1/', line):
            did = re.sub(r'^.*/phase1/fragment/result1/', "", line)
            did = re.sub(r'.txt', "", did)
            did = did.rstrip()
            l.append(int(did))


# read version-doc dict
print('Print vid list ...')

for q in sorted(d.keys()):
    # get the result of current query
    v_id_list = d[q]
    top_k_results = []
    for v_id in v_id_list:
        top_k_results.append(v_id)
    
    with open('optionB/vidlist.txt', 'w') as fmap:
        l_sorted = sorted(top_k_results)
        for i in range(len(l_sorted)):
            fmap.write(str(l_sorted[i]) + ' ')
