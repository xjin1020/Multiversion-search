rm -rf cluster
cp ../phase1/gen_index/vdrelation.txt ./

echo "********************start creating clusters********************"
time `python create_cluster.py`

echo "********************start gen_index step********************"
cd gen_index/
g++ main_new.cpp -std=c++0x -o gen_index
time python runjob_new.py

echo "********************start lucene_index step********************"
cd ../lucene_step/
time python runjob_index_new.py

echo "********************Finish indexing!!!********************"
cd ..
