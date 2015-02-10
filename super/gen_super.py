#!/usr/bin/python3

import pickle
import sys

## change your whole dataset path here
path_data = './allA13v_stem_clean.txt'
num_doc_per_version = 3838724
num_versions = 2

# create dictionary 'dic' for vdrelation
dic = {}
with open('../phase1/gen_index/vdrelation.txt', 'r') as fin_vd:
    for line_vd in fin_vd:
        line_vd_list = line_vd.split()
        vid = int(line_vd_list[0])
        did = int(line_vd_list[1])
        dic[vid] = did

dv_dic = {}
with open('../phase1/gen_index/dvrelation.txt', 'r') as f:
    for line in f:
        line_dv_list = line.split()
        did = int(line_dv_list[0])
        dv_dic[did] = len(line_dv_list) - 1

# read stop words
stop_words = set()
with open('stop_words.txt', 'r') as f:
    for line in f:
        stop_words.add(line.strip())

# create a hash for each did a hash of word => frequency

print('Creating word unions ...', file=sys.stderr)

# dict_d_tf: a dictionary, key is did, value is a dictionary dict_tf;
# dict_tf: a dictionary, key is word, value is frequency
dict_d_tf = {}

with open(path_data, 'r') as fin:
    for vid, line_data in zip(range(num_doc_per_version * num_versions), fin):

        if vid < 3000000:
            continue

        # if vid == 3000000:
        #     break

        did = dic.get(vid)
        if did is None:
            print("version {} not found!".format(str(vid)), file=sys.stderr)
            continue
        assert isinstance(line_data, str)
        words = line_data.split()

        dict_tf = dict_d_tf.setdefault(did, {})
        for word in words:
            dict_tf[word] = dict_tf.get(word, 0) + 1

        if vid > 0 and vid % 10000 == 0:
            print(vid, 'lines processed.', file=sys.stderr)

# with open('raw_dict_linux_2000000_3000000', 'wb') as f:
#     f.write(pickle.dumps(dict_d_tf))

# process file
# print('Generating index ...', file=sys.stderr)
index = {}

for doc_id in dict_d_tf:
    for word in dict_d_tf[doc_id]:
        if word in index:
            index[word].append(doc_id)
        else:
            index[word] = []

with open('raw_index_linux_3000000_end', 'wb') as f:
    f.write(pickle.dumps(index))

# print('Writing super-version ...', file=sys.stderr)
# for word in index:
#     if len(word) > 0 and len(index[word]) > 0:
#         tf_list = ['{did}:{tf:.2f}'.format(did=doc_id, tf=dict_d_tf[doc_id][word]/dv_dic.get(doc_id, 1)) for doc_id in index[word] if word not in stop_words]
#         print('{w}\t{tf}'.format(w=word, tf=' '.join(tf_list)))