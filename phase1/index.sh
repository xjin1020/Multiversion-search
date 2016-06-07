echo "********************start fragment step********************"
cd fragment/
make
./run_fragment.sh

echo "********************start gen_index step********************"
cd ../gen_index/
make
./run_gen_index.sh 3838724 

echo "********************start lucene_index step********************"
cd ../lucene_step/
./run_index.sh > output_index

echo "********************Finish indexing!!!********************"
cd ..
