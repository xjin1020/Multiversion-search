#!/usr/bin/python
import os
import sys

# function to run shell command
def run_command(com):
    output = os.popen(com).read()
    a = output.split('\n')
    for b in a:
        print b

def mycmp(a,b):
    if (a[1]>b[1]):
        return -1
    if (a[1]<b[1]):
        return 1
    return 0

query_len = 2#!!!USER!!!
fin = open('../phase1_rep/search_doc/vidlist.txt', 'r')# !!!USER!!!use phase 1's result, need to modify     input file path
k=1000#!!!USER!!! top k results in option B, we will find p clusters which contain k pages
latest_offset = 3532308 ##!!!USER!!! 3532308 reason: we use latest version, so here vid 0 is actually the first vid of the last version. How to compute this number: the last vid of vdrelation.txt + 1 - number of lines in "phase1_rep/fragment/data/rep.txt"

line = fin.readline()
fin.close()
vd_list = line.rstrip('\n').split(' ')
vd_list.remove('') # vd_list contains the vid from phase1_rep

# create dictionary 'dic' for vdrelation
fin_vd = open('vdrelation.txt', 'r')
lines_vd = fin_vd.readlines()
fin_vd.close()
dic = {}
for i in range(len(lines_vd)):
    line_vd_list = lines_vd[i].rstrip('\n').split(' ')
    key = line_vd_list[0]
    value = line_vd_list[1]
    dic[key] = value
            
# create dictionary 'dic_dv' for the relation from did to number of vid in its cluster
fin_dv = open('convert_table.txt', 'r')
lines_dv = fin_dv.readlines()
fin_dv.close()
dic_dv = {}
for i in range(len(lines_dv)):
    line_dv_list = lines_dv[i].rstrip('\n').split(' ')
    key_dv = line_dv_list[0]
    value_dv = int(line_dv_list[1]);
    dic_dv[key_dv] = value_dv


# choose p vids which occupy k vids to compare with optionTest
count=0
final_vid_list=[]
fout = open('vidlistC.txt', 'w')
for p in range(len(vd_list)):
    final_vid_list.append(vd_list[p]) # add one more vid into the list
    vid = str(int(vd_list[p])+latest_offset)     
    did = dic[vid]

    count = count + dic_dv[did]
    # work on cluster did
    print 'work on cluster', did, ':'
    fout.write(vid+'\n')
    
    os.chdir('cluster/'+did)
    print '**********enter cluster '+did+'**********'
    # lucene search to generate "search_frag.txt"
    com = '../../lucene_step/run_search.sh'
    print 'run lucene search:'
    run_command(com)
    # search_doc to generate "vidlist.txt"
    com = '../../search_doc/search_doc '+ str(query_len)
    print 'run search_doc:'
    run_command(com)
    # run option C using "search_frag.txt", "vidlist.txt" and "forward.txt"
    com = '../../search_doc/optionTestCluster_vector '+ str(query_len)
    print 'run optionTestCluster:'
    run_command(com)
    # finally, quit this cluster
    os.chdir('../..')
    
    if count > k:
        print 'p=', p
        break
if count < k :
    print 'vidlist.txt does not contain enough pages to calculate top ', k
    print count, p+1
fout.close()
