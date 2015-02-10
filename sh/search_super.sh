#!/bin/bash

#!/bin/bash

# $1 -> query, $2 -> query length

num_docs=10
top_k=100
green='\e[1;32m'
NC='\e[0m'

if [ -z "$1" ]; then
	echo "The query cannot be empty."
else
	echo "Clean up ..."
	# rm -rf "results/$1/baseline_www07/"
	# rm -rf "results/$1/baseline_sigir12/"
	# rm -rf "results/$1/twophase_latest/"
	# rm -rf "results/$1/twophase_longest/"
	# rm -rf "results/$1/super_latest/"
	# rm -rf "results/$1/super_longest/"
	# rm -f "results/$1/union/docs.txt"
	# rm -f "results/$1/union/doc_id_union.txt"
	

	# rm -f phase1/search_doc/result.txt
	# rm -f sigir12/level2/search_doc/result.txt
	rm -f super/search_doc_superlatest/result.txt
	rm -f super/search_doc_superlongest/result.txt

	rm -f phase1_rep/search_doc/result.txt
	rm -f phase1_long/search_doc/result.txt
	rm -f phase2/final_result.txt
	rm -f phase2/final_result_long.txt

	# prepare
	cd super
	echo "$1" > original_query
	./stem original_query > query
	cd ..

	############################################
	# Two-phase Latest Version + Super-version #
	############################################

	echo
	echo -e "${green}>>>> Start Searching Two-phase Representative (Latest + Super) <<<<${NC}"
	echo
	
	sed -i "s/java -jar archive_lucene_search_noprint.jar .*/java -jar archive_lucene_search_noprint.jar $1/g" phase1_rep/lucene_step/run_search_noprint.sh
	sed -i "s/java -jar ..\/..\/lucene_step\/archive_lucene_search_noprint.jar .*/java -jar ..\/..\/lucene_step\/archive_lucene_search_noprint.jar $1/g" phase2/lucene_step/run_search_noprint.sh
	
	sed -i "s/java -jar archive_lucene_search.jar .*/java -jar archive_lucene_search.jar $1/g" phase1_rep/lucene_step/run_search.sh
	sed -i "s/java -jar ..\/..\/lucene_step\/archive_lucene_search.jar .*/java -jar ..\/..\/lucene_step\/archive_lucene_search.jar $1/g" phase2/lucene_step/run_search.sh
	sed -i "s/query_len = .*#!!!USER!!!/query_len = $2#!!!USER!!!/g" phase2/runjob_search.py
	sed -i "s/k=.*#!!!USER!!!/k=$top_k#!!!USER!!!/g" phase2/runjob_search.py
	
	cd phase1_rep/
	echo "Lucene Step ..."
	cd lucene_step/
	./run_search.sh
	echo "Search_Doc Step ..."
	cd ../../super/search_doc_superlatest/
	time ./search_doc $2 `cat ../query`
	cd ..
	cp search_doc_superlatest/result.txt ../phase1_rep/search_doc/result.txt
	
	echo
	echo ">> Start Searching Two-phase Phase 2 (Latest + Super) <<"
	echo
	
	cd ../phase2/
	./search.sh
	
	# copy result over
	cd ..
	mkdir -p "results/$1/super_latest"
	cp phase2/final_result.txt "results/$1/super_latest/raw_result.txt"

	#############################################
	# Two-phase Longest Version + Super-version #
	#############################################
	
	echo
	echo -e "${green}>>>> Start Searching Two-phase Representative (Longest + Super) <<<<${NC}"
	echo
	
	sed -i "s/java -jar archive_lucene_search_noprint.jar .*/java -jar archive_lucene_search_noprint.jar $1/g" phase1_long/lucene_step/run_search_noprint.sh
	sed -i "s/java -jar ..\/..\/lucene_step\/archive_lucene_search_noprint.jar .*/java -jar ..\/..\/lucene_step\/archive_lucene_search_noprint.jar $1/g" phase2/lucene_step/run_search_noprint.sh
	
	sed -i "s/java -jar archive_lucene_search.jar .*/java -jar archive_lucene_search.jar $1/g" phase1_long/lucene_step/run_search.sh
	sed -i "s/java -jar ..\/..\/lucene_step\/archive_lucene_search.jar .*/java -jar ..\/..\/lucene_step\/archive_lucene_search.jar $1/g" phase2/lucene_step/run_search.sh
	sed -i "s/query_len = .*#!!!USER!!!/query_len = $2#!!!USER!!!/g" phase2/runjob_search_longest.py
	sed -i "s/k=.*#!!!USER!!!/k=$top_k#!!!USER!!!/g" phase2/runjob_search_longest.py
	
	# remember to generate longest version and index first
	cd phase1_long/
	echo "Lucene Step ..."
	cd lucene_step/
	./run_search.sh
	echo "Search_Doc Step ..."
	cd ../../super/search_doc_superlongest/
	time ./search_doc $2 `cat ../query`
	cd ..
	cp search_doc_superlongest/result.txt ../phase1_long/search_doc/result.txt
	
	echo
	echo ">> Start Searching Two-phase Phase 2 (Latest + Super) <<"
	echo
	
	cd ../phase2/
	time python runjob_search_longest.py
	
	# copy result over
	cd ..
	mkdir -p "results/$1/super_longest"
	cp phase2/final_result_long.txt "results/$1/super_longest/raw_result.txt"


	###################
	# Data Processing #
	###################

	echo
	echo -e "${green}>>>> Start Data Processing <<<<${NC}"
	echo

	mkdir -p "results/$1/union_super"
	./extract.py -n $num_docs --data "phase1/fragment/data/allA13v.txt" --result "results/$1/baseline_www07/raw_result.txt,results/$1/baseline_sigir12/raw_result.txt,results/$1/twophase_latest/raw_result.txt,results/$1/twophase_longest/raw_result.txt,results/$1/super_latest/raw_result.txt,results/$1/super_longest/raw_result.txt" --dest "results/$1/union_super/docs.txt" --dict "phase1/gen_index/vdrelation.txt"
	
fi
