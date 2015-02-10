#! /usr/bin/python3

import sys
import os

if not os.path.isdir('./results_copy'):
    print('"./results_copy" not found! Please copy your old labels!')
    exit()

query = ''
query = sys.argv[1]

old_vid = './results_copy/{}/union/doc_id_union.txt'.format(query)
old_label = './results_copy/{}/union/rating.txt'.format(query)

if not os.path.exists(old_label):
    print('query: {} was not previously labeled. You have to label it all'.format(query))
    exit()


# hash for old vid and labels
dict_vl = {}
print('reading old labels for query: {}'.format(query))
with open(old_label, 'r') as labels, open(old_vid, 'r') as vids:
    for line_label, line_vid in zip(labels, vids):
        assert isinstance(line_vid, str)
        assert isinstance(line_label, str)
        try:
            label = int(line_label.strip())
            vid = int(line_vid.strip())
            dict_vl[vid] = label
        except ValueError:
            pass


new_vid = './results/{}/union_super/doc_id_union.txt'.format(query)
new_label = './results/{}/union_super/rating.txt'.format(query)

print("labeling ...")
with open(new_label, 'w') as f_label, open(new_vid, 'r') as f_vid:
    for line in f_vid:
        vid = int(line.strip())
        rate = dict_vl.get(vid, ' ')
        f_label.write(str(rate) + '\n')

print("labeling complete. Please checkout any unlabeled pages.")
