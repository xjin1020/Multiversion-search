#!/bin/bash

num_l1_results=${1-1000}

cd level1/
./search_nopos.sh $num_l1_results
cd ..

./formVid.py
