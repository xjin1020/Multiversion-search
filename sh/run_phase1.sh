#!/bin/bash

# $1 -> query, $2 -> query length

if [ -z "$1" ]; then
    echo "The query cannot be empty."
else
    echo ""
    echo "************************************************"
    echo "Run query: $1"
    # Step 1: In phase1_rep: run lucene package to generate "search_frag.txt"
    cd phase1_long/lucene_step
    # edit query
    sed -i "s/java -jar archive_lucene_search.jar.*/java -jar archive_lucene_search.jar $1/g" run_search.sh
    ./run_search.sh &> /dev/null
    # Step 2: In phase1_rep: run lucene_tradi package to generate "search_frag.txt"
    cd ../lucene_step_tradi
    # edit query
    sed -i "s/java -jar archive_lucene_search.jar.*/java -jar archive_lucene_search.jar $1/g" run_search.sh
    ./run_search.sh &> /dev/null
    # Step 3: In phase1_rep: run search_doc_tradi to get intersection time
    cd ../search_doc_tradi
    ./search_doc $2
    # Step 4: In phase1_rep: run search_doc to get scoring time
    cd ../search_doc
    ./search_doc $2
fi
