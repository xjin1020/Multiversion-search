echo "**********start Lucene search (no print)**********"
cd lucene_step/
./run_search_noprint.sh

echo "**********start Lucene search**********"
./run_search.sh

echo "**********start search_doc**********"
cd ../search_doc/
./run_search_doc.sh $1

cd ..
