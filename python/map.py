#!/usr/bin/python3

import sys

if __name__ == '__main__':
    if len(sys.argv) == 3:
        src_path = sys.argv[1]
        ref_path = sys.argv[2]

        try:
            mapping = []
            with open(ref_path, 'r') as f:
                for line in f:
                    _, global_id = line.split()
                    mapping.append(global_id)

            with open(src_path, 'r') as f:
                for line in f:
                    local_id, score = line.split()
                    print(mapping[int(local_id)], score)

        except IOError as e:
            print('Cannot open necessary files', file=sys.stderr)
    else:
        print('Usage: ./map.py [src file] [ref file]', file=sys.stderr)