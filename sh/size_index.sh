#!/bin/bash
directory=$1
bc -ql <<< "`find "$directory" -type f -name '_0.cfs' -or -name '_0.cfx' -or -name 'segments_2' -or -name 'segments.gen' | pv -rabptlL320 | xargs -n1 stat -c %s | awk '{s+=$1}END{print s}'`/1024"
