#!/usr/bin/python3

import sys
import os
import re
import subprocess

data = "../phase1/fragment/data/allA13v.txt"
dest = "datasets/"
vd_relation_path = '../phase1/gen_index/vdrelation.txt'
top_k = 10

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
print('Start de-duplication ...')

# read vid-did mapping
vd_dict = {}
with open(vd_relation_path, 'r') as f:
    for line in f:
        v_id, doc_id = line.split()
        vd_dict[int(v_id)] = doc_id

subprocess.call("mkdir -p datasets", shell=True)
subprocess.call("mkdir -p mappings", shell=True)

for q in sorted(d.keys()):
    # get the result of current query
    v_id_list = d[q]
    top_k_results = []
    # de-dup v_id_list
    unique_doc_ids = set()
    for v_id in v_id_list:
        if len(unique_doc_ids) > top_k:
            break

        top_k_results.append(v_id)
        unique_doc_ids.add(vd_dict[v_id])
    
    del top_k_results[-1]
    
    with open('mappings/' + q, 'w') as fmap:
        l_sorted = sorted(top_k_results)
        for i in range(len(l_sorted)):
            fmap.write(str(i) + ' ' + str(l_sorted[i]) + '\n')

    # copy intermediate non-pos result pages to a new dataset for level-2 positional search
    # arg_list = ['-e {}p'.format(t + 1) for t in sorted(d[q])]
    arg_list = ['-e {}p'.format(t + 1) for t in sorted(top_k_results)]
    sed_args = ' '.join(arg_list)
    sed_command = 'sed -n {args} "{data}" > "{dest}"'.format(args=sed_args, data=data, dest=dest+q)
    # print(sed_command)
    subprocess.call(sed_command, shell=True)
    print(len(top_k_results), 'preliminary results extracted.')
