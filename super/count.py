#!/usr/bin/python3

import sys
import os

use_longest = True # change to False if use latest

# you only need to edit once here your dataset info 
path_data = '../phase1/fragment/data/allA13v.txt'
path_rep = '../phase1_rep/fragment/data/rep.txt'
total_docs = 3838724  
start_offset = 3838724 - 306416 

# create dictionary 'dic' for vdrelation

dic = {}
with open('../phase1/gen_index/vdrelation.txt', 'r') as fin_vd:
    for line_vd in fin_vd:
        line_vd_list = line_vd.strip('\n').split()
        vid = int(line_vd_list[0])
        did = int(line_vd_list[1])
        dic[vid] = did

# create a word set for each did

print('creating word sets based on {} ...'.format('longest version' if use_longest else 'latest version'))

dict_d_wset = {}    # hash: did => word set

if use_longest:
    with open('../phase1_long/longest_map.txt', 'r') as longest_map, open('../phase1_long/fragment/data/longest_version', 'r') as longest_version:
        for line_map, line_file in zip(longest_map, longest_version):
            list_d_v_len = line_map.strip().split()  # read the tuple(did, vid, length)
            did = int(list_d_v_len[0])
            assert isinstance(line_file, str)
            words = line_file.strip().split()
            word_set = set(words)
            dict_d_wset[did] = word_set
else:
    with open(path_rep, 'r') as latest_version:
        for vid, line_file in zip(range(start_offset, total_docs), latest_version):
            if vid not in dic.keys():
                print("version {} was discarded before!".format(str(vid)))
                continue
            did = dic.get(vid)
            assert isinstance(line_file, str)
            words = line_file.strip().split()
            word_set = set(words)
            dict_d_wset[did] = dict_d_wset.get(did, set()).union(word_set)  # word set for did might exist, do union

# create a hash for each did a hash of word => frequency

print('calculating super version term frequency ...')

# dict_d_tf: a dictionary, key is did, value is a difference set;
dict_d_diff = {}

with open(path_data, 'r') as fin:
    for vid, line_data in zip(range(total_docs), fin):
        did = dic.get(vid)
        if did is None:
            print("version {} not found!".format(str(vid)))
            continue
        assert isinstance(line_data, str)
        words = line_data.strip().split()
        word_set = set(words)
        set_difference = word_set - dict_d_wset.get(did, set()) # word set for did might not exist, because this did not in representative
        dict_d_diff[did] = dict_d_diff.get(did, set()).union(set_difference)

# print to file
print('printing to file')

with open("stats_"+("longest" if use_longest else "latest"), 'w') as fout:
    for did in sorted(dict_d_diff):
        fout.write('{}\t{}\t{}\n'.format(did, len(dict_d_wset.get(did, set())), len(dict_d_diff.get(did, set()))))


# Susen,now we have following structures:
# dict_d_tf: a dictionary, key is did, value is a dictionary tf;
# tf: a dictionary, key is word, value is frequency
