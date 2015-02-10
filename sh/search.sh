#!/bin/bash

# $1 -> query, $2 -> query length

num_docs=10
top_k=100

if [ -z "$1" ]; then
	echo "The query cannot be empty."
else
	###################
	# WWW 07 Baseline #
	###################
	
	echo 
	echo ">>>> Start Searching WWW 07 Baseline <<<<"
	echo
	# phase 1 search
	cd phase1
	# edit query
	sed -i "s/java -jar archive_lucene_search_noprint.jar.*/java -jar archive_lucene_search_noprint.jar $1/g" lucene_step/run_search_noprint.sh
	sed -i "s/java -jar archive_lucene_search.jar.*/java -jar archive_lucene_search.jar $1/g" lucene_step/run_search.sh
	# do search
	./search.sh $2
	# copy result over
	cd ..
	mkdir -p "results/$1/baseline_www07"
	cp phase1/search_doc/result.txt "results/$1/baseline_www07/raw_result.txt"
	# extract docs
	# ./extract.py -n $num_docs --data "phase1/fragment/data/linux_439_all.txt" --result "results/$1/baseline/result.txt" --dest "results/$1/baseline/baseline_docs.txt" --dict "phase1/gen_index/vdrelation.txt"
	
	#####################
	# SIGIR 12 Baseline #
	#####################
	
	echo
	echo ">>>> Start Searching SIGIR 12 Baseline <<<<"
	echo
	
	cd sigir12
	echo "$1" > queries
	./prepare.sh
	
	cp "datasets/$1" level2/fragment/data/nonpos.txt
	
	cd level2
	./index.sh
	# edit query
	sed -i "s/java -jar archive_lucene_search_noprint.jar.*/java -jar archive_lucene_search_noprint.jar $1/g" lucene_step/run_search_noprint.sh
	sed -i "s/java -jar archive_lucene_search.jar.*/java -jar archive_lucene_search.jar $1/g" lucene_step/run_search.sh
	# do search
	./search.sh $2
	cd ../../
	mkdir -p "results/$1/baseline_sigir12"
	cp sigir12/level2/search_doc/result.txt "results/$1/baseline_sigir12/_raw_result.txt"
	# map document id to those in the original dataset
	./map.py "results/$1/baseline_sigir12/_raw_result.txt" "sigir12/mappings/$1" > "results/$1/baseline_sigir12/raw_result.txt"
	rm "results/$1/baseline_sigir12/_raw_result.txt"
	# extract docs
	# ./extract.py -n $num_docs --data "sigir12/level2/fragment/data/nonpos.txt" --result "results/$1/baseline_sigir12/result.txt" --dest "results/$1/baseline_sigir12/baseline_docs_sigir12.txt" --dict "sigir12/level2/gen_index/vdrelation.txt"
	
	############################
	# Two-phase Latest Version #
	############################

	echo
	echo ">>>> Start Searching Two-phase Representative (Latest Version) <<<<"
	echo
	
	sed -i "s/java -jar archive_lucene_search_noprint.jar .*/java -jar archive_lucene_search_noprint.jar $1/g" phase1_rep/lucene_step/run_search_noprint.sh
	sed -i "s/java -jar ..\/..\/lucene_step\/archive_lucene_search_noprint.jar .*/java -jar ..\/..\/lucene_step\/archive_lucene_search_noprint.jar $1/g" phase2/lucene_step/run_search_noprint.sh
	
	sed -i "s/java -jar archive_lucene_search.jar .*/java -jar archive_lucene_search.jar $1/g" phase1_rep/lucene_step/run_search.sh
	sed -i "s/java -jar ..\/..\/lucene_step\/archive_lucene_search.jar .*/java -jar ..\/..\/lucene_step\/archive_lucene_search.jar $1/g" phase2/lucene_step/run_search.sh
	sed -i "s/query_len = .*#!!!USER!!!/query_len = $2#!!!USER!!!/g" phase2/runjob_search.py
	sed -i "s/k=.*#!!!USER!!!/k=$top_k#!!!USER!!!/g" phase2/runjob_search.py
	
	cd phase1_rep/
	./search.sh $2
	
	echo
	echo ">> Start Searching Two-phase Phase 2 (Latest Version) <<"
	echo
	
	cd ../phase2/
	./search.sh
	
	# copy result over
	cd ..
	mkdir -p "results/$1/twophase_latest"
	cp phase2/final_result.txt "results/$1/twophase_latest/raw_result.txt"
	# extract docs
	# ./extract.py -n $num_docs --data "phase1/fragment/data/linux_439_all.txt" --result "results/$1/twophase_latest/final_result.txt" --dest "results/$1/twophase_latest/twophase_docs.txt" --dict "phase1/gen_index/vdrelation.txt"
	
	#############################
	# Two-phase Longest Version #
	#############################
	
	echo
	echo ">>>> Start Searching Two-phase Representative (Longest Version) <<<<"
	echo
	
	sed -i "s/java -jar archive_lucene_search_noprint.jar .*/java -jar archive_lucene_search_noprint.jar $1/g" phase1_long/lucene_step/run_search_noprint.sh
	sed -i "s/java -jar ..\/..\/lucene_step\/archive_lucene_search_noprint.jar .*/java -jar ..\/..\/lucene_step\/archive_lucene_search_noprint.jar $1/g" phase2/lucene_step/run_search_noprint.sh
	
	sed -i "s/java -jar archive_lucene_search.jar .*/java -jar archive_lucene_search.jar $1/g" phase1_long/lucene_step/run_search.sh
	sed -i "s/java -jar ..\/..\/lucene_step\/archive_lucene_search.jar .*/java -jar ..\/..\/lucene_step\/archive_lucene_search.jar $1/g" phase2/lucene_step/run_search.sh
	sed -i "s/query_len = .*#!!!USER!!!/query_len = $2#!!!USER!!!/g" phase2/runjob_search_longest.py
	sed -i "s/k=.*#!!!USER!!!/k=$top_k#!!!USER!!!/g" phase2/runjob_search_longest.py
	
	# remember to generate longest version and index first
	cd phase1_long/
	./search.sh $2
	
	echo
	echo ">>>> Starting Searching Two-phase Phase 2 (Longest Version) <<<<"
	echo
	
	cd ../phase2/
	time python runjob_search_longest.py
	
	# copy result over
	cd ..
	mkdir -p "results/$1/twophase_longest"
	cp phase2/final_result_long.txt "results/$1/twophase_longest/raw_result.txt"
	# extract docs
	# ./extract.py -n $num_docs --data "phase1/fragment/data/linux_439_all.txt" --result "results/$1/twophase_longest/final_result_long.txt" --dest "results/$1/twophase_longest/twophase_docs_long.txt" --dict "phase1/gen_index/vdrelation.txt"
	
	echo
	echo ">>>> Start Processing Results <<<<"
	echo
	# extract docs
	mkdir -p "results/$1/union"
	./extract.py -n $num_docs --data "phase1/fragment/data/allA13v.txt" --result "results/$1/baseline_www07/raw_result.txt,results/$1/baseline_sigir12/raw_result.txt,results/$1/twophase_latest/raw_result.txt,results/$1/twophase_longest/raw_result.txt" --dest "results/$1/union/docs.txt" --dict "phase1/gen_index/vdrelation.txt"
	
fi
