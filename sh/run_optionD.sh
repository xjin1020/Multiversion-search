#!/bin/bash

# $1 -> query, $2 -> query length

top_k=1000

if [ -z "$1" ]; then
    echo "The query cannot be empty."
else
    echo ""
    echo "************************************************"
    echo "Run query: $1"
    # Step 1: In phase1_rep: run lucene package to generate "search_frag.txt"
    cd phase1_rep/lucene_step
    #edit query
    sed -i "s/java -jar archive_lucene_search.jar.*/java -jar archive_lucene_search.jar $1/g" run_search.sh
    ./run_search.sh &> /dev/null
    # Step 2: In phase1_rep: run search_doc to generate "vidlist.txt"
    cd ../search_doc
    ./search_doc $2 &> /dev/null
    # Step 3: In phase2: run "runjob_optionC.py" for optionC and "runjob_optionA.py" for optionA
    cd ../../phase2
    # edit query
    sed -i "s/query_len = .*#!!!/query_len = $2#!!!/g" runjob_optionC.py
    sed -i "s/query_len = .*#!!!/query_len = $2#!!!/g" runjob_optionD.py
    sed -i "s/query_len = .*#!!!/query_len = $2#!!!/g" runjob_optionA.py
    sed -i "s/k=.*#!!!/k=$top_k#!!!/g" runjob_optionC.py
    sed -i "s/k=.*#!!!/k=$top_k#!!!/g" runjob_optionD.py
    sed -i "s/k=.*#!!!/k=$top_k#!!!/g" runjob_optionA.py
    sed -i "s/java -jar ..\/..\/lucene_step\/archive_lucene_search.jar.*/java -jar ..\/..\/lucene_step\/archive_lucene_search.jar $1/g" lucene_step/run_search.sh

    echo
    echo ">>>> Start Searching using OptionD <<<<"
    echo
    python runjob_optionD.py &> outputD
    python calculateD.py

fi
