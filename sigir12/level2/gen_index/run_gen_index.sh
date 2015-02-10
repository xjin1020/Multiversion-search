#!/bin/bash
rm -rf fragments
rm all_fragments.txt
mkdir fragments
make
time ./gen_index $1
