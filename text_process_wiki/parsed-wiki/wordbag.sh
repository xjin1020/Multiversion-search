#!/bin/bash
#parses wikipedia markup in parallel, returning a list of words for each article
#Written by: Michael Agun
#Last Updated: 2/20/2014
parallel -j100% --recend "</page>" --pipe --block-size 20M bash -c 'iconv -f utf8 -t ascii//translit | perl clean-bag.pl'
