echo "****************start generating longest version***********"
./gen_longest.py
doc_num=$(wc -l fragment/data/longest_version | grep -o "[0-9]*")

echo "********************start fragment step********************"
cd fragment/
./run_fragment.sh

echo "********************start gen_index step********************"
cd ../gen_index/
./run_gen_index.sh $doc_num 

echo "********************start lucene_index step********************"
cd ../lucene_step/
./run_index.sh > output_index

echo "********************Finish indexing!!!********************"
cd ..
