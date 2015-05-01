#!/bin/bash
#takes a wikipedia xml snapshot, scans for articles beginning with "A",
#  then parses links/tags and returns only a list of words for each article
#Written by: Michael Agun
#Last Updated: 2/21/2014
infile="$1"
outfile=`echo "${infile}" | sed -e 's/.*\/\(.*\).bz2.*/\1_all.txt/'`
echo "$infile -> $outfile"
time < "$infile" bunzip2 | ./scan-prefix.sh "A" | ./drop-redirect.sed | ./wordbag.sh > "$outfile"
#time < "$infile" bunzip2 | ./drop-redirect.sed | ./wordbag.sh > "$outfile"
