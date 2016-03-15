rm -rf index
#time java -jar archive_lucene_index.jar ../gen_index/fragments
time java -jar archive_lucene_index2.jar ../gen_index/all_fragments.txt
