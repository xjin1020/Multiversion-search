#!/bin/bash

rm -rf search_frag.txt
java -jar lucene_search_1000_noprint.jar $1
