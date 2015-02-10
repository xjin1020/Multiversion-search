rm -rf cluster
cp ../phase1/gen_index/vdrelation.txt ./

echo "********************start creating clusters********************"
time `python create_cluster.py`

echo "********************start gen_index step********************"
cd gen_index/
time python runjob.py

echo "********************start lucene_index step********************"
cd ../lucene_step/
time python runjob_index.py

echo "********************Finish indexing!!!********************"
cd ..
