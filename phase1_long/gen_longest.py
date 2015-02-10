#!/usr/bin/python3

import sys
import os

# create dictionary 'dic' for vdrelation
fin_vd = open('../phase1/gen_index/vdrelation.txt', 'r')
lines_vd = fin_vd.readlines()
fin_vd.close()
dic = {}
for i in range(len(lines_vd)):
    line_vd_list = lines_vd[i].rstrip('\n').split(' ')
    key = line_vd_list[0]
    value = line_vd_list[1]
    dic[key] = value

# create dictionary 'dr' for did => (vid, length, str)
dict_dr = {}

print('calculating longest version ...')

# read data set by line
with open('../phase1/fragment/data/allA13v.txt', 'r') as fin:
    vid = 0
    for line in fin:
        words = line.split(' ')
        length = len(words)
        if str(vid) not in dic.keys():
            print("version {} not found!".format(str(vid)))
            vid += 1
            continue
        did = int(dic[str(vid)])
        if length > dict_dr.get(did, (None, -1, None))[1]:
            dict_dr[did] = (vid, length, line)
        vid += 1

print('generating longest version ...')

# in longest_version, vid is did; vid is not the old vid any more
with open('./fragment/data/longest_version', 'w') as fout, open('longest_map.txt', 'w') as fmap:
    for key in sorted(dict_dr.keys()):
        fmap.write(str(key) + ' ' + str(dict_dr[key][0]) + ' ' + str(dict_dr[key][1]) + '\n')
        fout.write(dict_dr[key][2])

print('\nlongest version completed!')
