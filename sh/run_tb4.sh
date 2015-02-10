#!/bin/bash

while read query; do
  length=`echo $query | wc -w`
  # lucene_step
  cd phase1/lucene_step
  rm -rf search_frag.txt
  java -jar lucene_search_1000_noprint.jar $query
  java -jar archive_lucene_search.jar $query

  # search_doc
  cd ../search_doc
  rm -rf result.txt
  time ./search_doc $length

  cd ../..
done <queries_all.txt
