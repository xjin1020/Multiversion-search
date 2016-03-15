rm -rf index
#java -jar archive_lucene_index.jar ../gen_index/fragments
#time java -jar archive_lucene_index.jar ../fragment/result1
time java -jar archive_lucene_index2.jar ../fragment/data/rep.txt
#time ./binary_index_gen --index index < ../fragment/data/rep.txt
