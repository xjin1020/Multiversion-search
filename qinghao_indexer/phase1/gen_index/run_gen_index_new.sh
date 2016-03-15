#!/bin/bash
rm all_fragments.txt
g++ main_new.cpp -std=c++0x -o gen_index
time ./gen_index $1
