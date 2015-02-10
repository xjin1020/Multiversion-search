#!/bin/bash

num_l1_results=${1-150}

cd level1/
./search_nopos.sh $num_l1_results
cd ..

./formDatasets.py
