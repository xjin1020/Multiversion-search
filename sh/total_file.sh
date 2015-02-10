#!/bin/bash
find . -type f | xargs -n1 stat -c %s | awk '{s+=$1}END{print s/1024}'
