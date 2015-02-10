import shutil
import os

def ensure_dir(f):
    d = os.path.dirname(f)
    if not os.path.exists(d):
        os.makedirs(d)

fin = open("../phase1/gen_index/dvrelation.txt", "r")
fout = open("convert_table.txt", "w")
for line in fin.readlines():
    line_list = line.split(" ")
    did = line_list[0]
    vid_list = []
    len = int(line_list[1])
    # create vid_list, which saves the corresponding vids to a did
    for i in range(len):
        vid_list.append(line_list[2+i])
    # each vid_list is a cluster, copy these files from ../phase1/fragment/result1/ to cluster/did/
    for vid_i in vid_list:
        src = '../phase1/fragment/result1/'+vid_i+'.txt'
        dst = 'cluster/'+did+'/ori_'+vid_i+'.txt'
        ensure_dir(dst)
        #shutil.copyfile(src, dst)
    # create convert table
    # this file will be in "phase2/convert_table.txt", it is the same format as dvrelation.txt but it makes sure that each vid list is sorted from small to large
    fout.write(did+' '+str(len))
    vid_int_list = []
    for i in range(len):
        vid_int_list.append(int(vid_list[i]))
    vid_int_list.sort()
    for i in range(len):
        fout.write(' '+str(vid_int_list[i]))
    fout.write('\n')
    # copy the files in the cluster to new name "0.txt" "1.txt"...
    for i in range(len):
        src = '../phase1/fragment/result1/'+str(vid_int_list[i])+'.txt'
        dst = 'cluster/'+did+'/'+str(i)+'.txt'
        shutil.copyfile(src, dst)
fin.close()
fout.close()
