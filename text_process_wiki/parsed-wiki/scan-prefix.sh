#!/bin/bash
#scans for wikipedia pages with the given prefix in the title
#Written by: Michael Agun
#Last Updated: 2/20/2014
prefix="$1";
shift
sed -ne "/<title>${prefix}[^<]*<\/title>/,/<\/page>/{s/^[ \t]*<title>${prefix}[^<]*<\/title>/  <page>\n&/;p}" $*
