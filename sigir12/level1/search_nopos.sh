#!/bin/bash

num_l1_results=${1-100}
java -jar ./lucene_search.jar -paging $num_l1_results -queries ../queries > level1_results.txt 
