#!/bin/bash
#drops articles that look like redirects from already parsed word-bag results
grep -Ev "^[^	]*	redirect .{,260}" $1 > "noredirect_$1"
