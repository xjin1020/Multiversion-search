#!/usr/bin/python3

import sys
import os
import random
import subprocess


MAX_CLUSTER_NUM = 315000
SAMPLE_SIZE = 2000

cluster_tested = set()
total_size = 0  # in KB

while len(cluster_tested) < SAMPLE_SIZE:
    cluster_id = random.randrange(MAX_CLUSTER_NUM)
    if cluster_id not in cluster_tested:
        # test index size
        p = subprocess.Popen(['du', '-h', 'phase2/cluster/{}/index'.format(cluster_id)], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        out, err = p.communicate()

        if len(out) > 0:
            current_size_str = out.decode().split()[0]
            value = float(current_size_str[:-1].replace(',', '.'))
            unit = current_size_str[-1].lower()
            if unit == 'k':
                total_size += value
            elif unit == 'm':
                total_size += value * 1024
            elif unit == 'g':
                total_size += value * 1024 * 1024

            cluster_tested.add(cluster_id)

print('Size for', SAMPLE_SIZE, 'cluster index:', total_size, 'KB')
print('Average size for single cluster index:', total_size / SAMPLE_SIZE, 'KB')
