#!/usr/bin/python3

import re
import os

if __name__ == '__main__':
    src_path = './allA13v_stem.txt'
    dest_path = './allA13v_stem_clean.txt'

    if os.path.isfile(src_path):
        with open(src_path, 'r') as f_in, open(dest_path, 'w') as f_out:
            for line in f_in:
                s = re.sub(r'[^a-zA-Z0-9\s]', ' ', line)
                f_out.write(s)
