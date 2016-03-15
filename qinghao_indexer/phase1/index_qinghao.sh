echo "********************start fragment step********************"
cd fragment/
./run_fragment.sh

echo "********************start gen_index step********************"
cd ../gen_index/
./run_gen_index_new.sh 3838724

echo "********************start lucene_index step********************"
cd ../lucene_step/
./run_index_new.sh > output_index

echo "********************Finish indexing!!!********************"
cd ..
