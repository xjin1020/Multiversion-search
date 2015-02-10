#!/bin/bash
rm -rf fragments
rm all_fragments.txt
mkdir fragments
make
time ./gen_index 5 1000 1000 1000 1000 1000
