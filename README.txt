Author: Xin Jin

******************************************************************************
Step 1: framentation:
After fragmentation, version$i's results will be in fragment/result$i/*.txt. Each file will have the title as the first line and each fragment as a line.
For the new version code, we will put all versions in one file. Thus, it will have one file as input and all the results will be in result1

******************************************************************************
Step 2: generate index using the results from step 1:
Fragments result are saved after this step in fragments/.

Data structures in gen_index:
VID is the same as this page's filename number. E.g. VID 2440 is in fragment/result1/2440.txt. However, some VID might not exist because some pages may not have title so that they are disregarded.
1. title_did_hash: <string, int>
2. VID->DID
    vid_did_hash: <int,int>
3. DID->a vector of VID
    did_vid_hash: <int,vector<int>>
4. frag size table
    fid_size_hash: <int,int>
5. frag->content
    fid_content_hash: <int,string>
6. content->frag
    content_fid_hash: <string,int>
7. vid->a vector of FID
    vid_fid_hash: <int,vector<int>>
8. frag reuse table: fid->a vector of <vid, offset>
    frag_reuse_table: <int,vector<ReuseTableInfo>>
        ReuseTableInfo: vid, offset
9. did->title
    did_title_hash: <int, string>

Format of the output: index.txt:
**each part is separated with -99999**
(1) FragReuseTable:
fid vector.size vid1 offset1 vid2 offest2 ...
(2) vid_titlelen_hash:
vid title_length
...

"dvrelation.txt" saves the mapping from doc to version
Format of the output:dvrelation.txt
did vid_vector_length vid1 vid2 ...

"vdrelation.txt" saves the mapping from version to doc
Format:
vid did (Note that vid is the same as its filename in fragment/result1 since we only have 1 version now. However, the number of vid might not be consecutive because some pages are disregarded because they do not have title)

******************************************************************************
Step 3:
Index fragments/ in Lucene and get search results in fragments style. (modify Lucene 3.0.2's "lucene/search/PhraseScorer.java")
The output file is "search_frag.txt".
File format:
termID fragFileID pos1 pos2 ...

******************************************************************************
Step 4:
Use the data structures generated from step 2 and searching results from step 3 to get final results.

Be care: A query might span several fragments in a document. Worst case needs o(n^k) comparison, where k is the query terms number.

Input:
(1) ../gen_index/index.txt: 
    frag_reuse_table
    vid_titlelen_hash
(2) ../search_frag.txt:
    search_frag

intersection_hash:
    hash<int, vector<set<int>>>: 
        VID->vector<set<int>>, 
        set<int> is the set of positions for one term; 
        vector<set<int>> is for n_terms

Scoring function:
    final_score = 5/6title + 1/6body
    title = appear/term_number
    body = term_number/min_span

******************************************************************************
******************************************************************************
Our positional two-step search algorithm:

Indexing Pipeline:
Phase 1: index for all versions
    (1) fragment // fragment results will be used for Phase 2's creating cluster step
    **(2) gen_index** // only necessary for baseline method
    **(3) Lucene_index** // only neceaary for baseline method

Phase 1_rep: index for representatives
    (1) fragment
    (2) gen_index
    (3) Lucene_index

Phase 2: index for each cluster
    for each representative, we will have a list of pages in its cluster (from gen_index/dvrelation.txt)
    we do the following for each cluster
    (1) fragment docs in the cluster: 
        use create_cluster.py to copy the fragment results from phase1/fragment/result1. We need to use phase1/gen_index/dvrelation.txt here.
        create "convert table" here because we need to rename each file in the cluster to "0.txt", "1.txt" ...; we need to use this convert table in the future to convert its back.
    (2) gen_index
    (3) Lucene_index: build index for a cluster

Searching Pipeline:
**Phase 1: search on all version index** // only necessary for baseline method
    **(1) Lucene_search**
    **(2) search_doc**

Phase 1_rep: search on representative index
    (1) Lucene_search
    (2) search_doc

Phase 2: search on top k clusters
    (1) Pick up top k from Phase 1_rep.(2)
    (2) for each cluster in (1), Lucene_search and search_doc: be attention that after this step, each cluster's search_doc results is based on the renamed file "0.txt", "1.txt" ...
    (3) combine the k "result.txt" and retrieve final top m results: need to use the convert table to convert them back


******************************************************************************
******************************************************************************
Scripts:
super/count.py: calculate distinct word statistics


******************************************************************************
New modification: Oct. 2014
In search_doc/main.cpp
intersection(): hash based intersection algorithm
intersection3(): jump list based intersection algorithm

Add lucene_search_tradi/,search_doc_tradi/:
Use traditional index to search.


******************************************************************************
New modification: Dec. 2014
Add convert.txt in phase1_rep: convert from traditional reuse table to forward
reuse table. This will help implement Option B and C.
Format of the output "forward.txt":
vid n(number of fids) fid1 offset1 fid2 offset2 ... fidn offsetn


******************************************************************************
New modification: Dec.18 2014
Add option A, B, C
run_options.sh:
Process:
(1) In phase1_rep: run lucene package to generate "search_frag.txt"
(2) In phase1_rep: run search_doc to generate "vidlist.txt"
(3) In phase2: run "runjob_optionC.py" for optionC and "runjob_optionA.py" for optionA

batch_options is to to a batch of queries
(4) copy "vidlistC.txt" to phase1/search_doc to run "optionB" for optionB


******************************************************************************
Jan. 9. 2015
Explanation of batch*:
    (1) with "_longest": for SLO, otherwise for SLA
    (2) batch_intersection_phase2*: calculate phase2 intersection time. It can be also
used to calculate phase2 scoring time, only need to copy
phase2/search_doc/main.cpp from main_scoring.cpp other than
main_intersection.cpp
    (3) How to generate option A, B, C time in the paper:
    batch_optionTest*: calculate outTestA, outTestC and then use googledoc_optionTest.py to generate ft file which can be used to show on google_doc and used for choose_parameter.py to get OptionD result which choose best A or C for each term instead of each cluster.
How to run: (a)make sure phase2/search_doc/main.cpp comes from main_options.cpp (use diff to make sure they are the same)and (b) modify the flag which labels to run optionA or optionB in phase2/search_doc/optionTest.cpp to the corresponding options(A or C) and need to be run twice(first run is warm up).
    outTestA_f_avg saves the results of batch_optionTest for option A 
    outTestC_f_avg saves the results of batch_optionTest for option C 

    (4) choose option D by cluster:
    run "batch_optionTestCluster.py" to get results like outTestClusterA and
    outTestClusterC. 
    Then use choose_parameterCluster.cpp to generate best optionD which choose
    best A or C for each cluster.

    (5) batch_phase1.py: calculate phase1 time; batch_phase1_sigir.py:
    calculate sigir phase1 time.

Format of outTestClusterA(C):
-99999
query_len numberofcluster
f time(cluster 1 token1) f time(cluster 1 token2) ... f time(last cluster last token)
totaltime[token0] ... totaltime[tokenl]
-99999
...

******************************************************************************
Feb. 3. 2016
Qinghao's option A, B, C check README_qinghao
Qinghao's ratio scripts check README_ratio

******************************************************************************
Mar. 15. 2016
Qinghao's indexer: in qinghao_indexer folder
For Web and Github dataset:
The only difference in indexing phase is:   phase1/index_qinghao.sh
Different datasets have different 7th line: 
for wiki:
./run_gen_index_new.sh 3838724
for web:
./run_gen_index_new.sh 2500000
for github:
./run_gen_index_new.sh 5007525
