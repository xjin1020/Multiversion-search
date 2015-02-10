#!/usr/bin/python3

import sys
import os
import subprocess
import argparse


parser = argparse.ArgumentParser(description='Result pages extractor.')
parser.add_argument('-n', default=20, type=int, help='top n results to be extracted')
parser.add_argument('--data', required=True, help='data source')
parser.add_argument('--result', required=True, help='list of result sources, separated by colon')
parser.add_argument('--dest', default='pages.txt', help='extracted pages destination')
parser.add_argument('--dict', default='phase1/gen_index/vdrelation.txt', help='the version-doc table')

args = parser.parse_args()

# read version-doc dict
print('Reading doc-ver mapping ...')

d = {}
with open(args.dict, 'r') as f:
    for line in f:
        doc_id, version = line.split()
        d[doc_id] = version

result_paths = args.result.split(',')

page_id_union = set()

all_page_ids = []
sequence = ['www07', 'sigir12', '2platest', '2plongest']

print('Eliminating redundancy ...')

for path in result_paths:
    page_ids = []
    existed_versions = set()

    try:
        with open(path, 'r') as f:
            for line in f:
                if len(page_ids) == args.n:
                    break

                page_id, score = line.split()

                # if len(page_ids) == 0 or page_ids[-1][1] != score:
                page_id_int = int(page_id)
                if d[page_id] not in existed_versions:
                    existed_versions.add(d[page_id])
                    page_ids.append((page_id, score))
                    page_id_union.add(page_id)
    except IOError:
        print('Result file', path, 'does not exist, treat as empty.')

    dest_path = os.path.join(os.path.dirname(path), 'doc_id.txt')
    with open(dest_path, 'w') as f:
        f.write('\n'.join([t[0] for t in page_ids]))

    all_page_ids.append(page_ids)
    print(page_ids)

page_id_union_list = [int(doc_id) for doc_id in page_id_union]
page_id_union_list.sort()

arg_list = ['-e {}p'.format(int(doc_id) + 1) for doc_id in page_id_union_list]
sed_args = ' '.join(arg_list)

command = 'sed -n {args} "{data}" > "{dest}"'.format(args=sed_args, data=args.data, dest=args.dest)

print('Extracting documents ...')
subprocess.call(command, shell=True)

# content_list = []
# with open(args.dest, 'r') as f:
#     content_list = f.readlines()
#     
# vid_content_map = dict(zip(page_id_union_list, content_list))
# 
# for i in range(len(all_page_ids)):
#     with open(sequence[i] + '.txt', 'w') as f:
#         for vid, score in all_page_ids[i]:
#             f.write('{} => {}'.format(vid, vid_content_map[int(vid)]))

union_dest_path = dest_path = os.path.join(os.path.dirname(args.dest), 'doc_id_union.txt')
with open(union_dest_path, 'w') as f:
    f.write('\n'.join([str(doc_id) for doc_id in page_id_union_list]))

print('Done.')