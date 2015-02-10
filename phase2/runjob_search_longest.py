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

query_len = 6#!!!USER!!!
fin = open('../phase1_long/search_doc/result.txt', 'r') # !!!USER!!!use phase 1's result, need to modify input file path
k=100#!!!USER!!!
lines = fin.readlines()
fin.close()
if (k>len(lines)):
    print 'not enough lines, k<result length, set k to ', len(lines)
    k= len(lines)
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

# create dictionary 'dic_dv' for dvrelation
fin_dv = open('convert_table.txt', 'r')
lines_dv = fin_dv.readlines()
fin_dv.close()
dic_dv = {}
for i in range(len(lines_dv)):
    line_dv_list = lines_dv[i].rstrip('\n').split(' ')
    key_dv = line_dv_list[0]
    value_dv = []
    for j in range(int(line_dv_list[1])):
        value_dv.append(int(line_dv_list[2+j]))
    dic_dv[key_dv] = value_dv

final_result = []
for i in range(k):
    # 1. find the top k vid results from phase 1
    line_list = lines[i].rstrip('\n').split(' ')
    vid = line_list[0]
    # 2. use longest version as representative, since longest version is 1 to 1 map with clusters, its vid is did, hence cluster id, this is different from using lasted version
    # did = dic[vid]
    did = vid
    # 3. run lucene_search and search_doc in the cluster
    os.chdir('cluster/'+did)
    print '**********enter cluster '+did+'**********'
    # lucene search
    com = '../../lucene_step/run_search.sh'
    print 'run lucene search:'
    run_command(com)
    # search_doc
    com = 'rm -f result.txt'
    os.popen(com)
    com = 'time ../../search_doc/search_doc '+ str(query_len)
    print 'run search_doc:'
    run_command(com)
    # 4. combine k "result.txt" from k clusters
    fr = open('result.txt', 'r')
    lines_r = fr.readlines()
    fr.close()
    for i in range(len(lines_r)):
        lines_r_list = lines_r[i].rstrip('\n').split(' ')
        rid = int(lines_r_list[0])
        rscore = float(lines_r_list[1])
        rvid = dic_dv[did][rid]
        final_result.append((rvid, rscore))
    # finally, quit this cluster
    os.chdir('../..')
final_result.sort(cmp=mycmp)
fout=open('final_result_long.txt', 'w')
for item in final_result:
    fout.write(str(item[0])+' '+str(item[1])+'\n')
fout.close()
