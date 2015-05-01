#!/bin/bash
#uses parallel to find the specified pages in wiki markup xml
#Written by: Michael Agun
#Last Updated: 2/20/2014
parallel -a "$2" -j100% --recend "</page>" --block-size 20M --pipe ./scan-prefix.sh "'$1'";
