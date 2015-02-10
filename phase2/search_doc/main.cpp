#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <unordered_map>
#include <set>
#include <algorithm>
#include <iterator>
#include <math.h>
#include <ctime>
#include <sys/time.h>

using namespace std;

class ReuseTableInfo
{
public:
    int vid;
    int offset;
};

// fid occurence
class Occurence
{
public:
    int fid;
    vector<int> v_pos;
};

class ScoreResult
{
public:
    int vid;
    double score;
};

// vid occurence
class V_Occurence
{
public:
    int vid;
    set<int> pos;
};

// result occurence
class R_Occurence
{
public:
    int vid;
    vector<set<int>> pos;
};

unordered_map<int, vector<ReuseTableInfo>> frag_reuse_table;
vector<vector<Occurence>> search_frag; // <<fID, <pos>>>
unordered_map<int ,int> vid_titlelen_hash; // vid to title length
unordered_map<int, vector<set<int>>> intersection_hash; // this keeps the intersection results
vector<R_Occurence> result_posting; // this keeps the result posting after intersection
vector<ScoreResult> score_result; // this vector keeps the final scoring results
int query_len;

clock_t start;  // global variable added to mark the starting point of time statistics
double duration;  // global variable added to calculate non-I/O time

void read_index()
{
    ifstream fin;
    fin.open("index.txt");
    // read frag_reuse_table
    frag_reuse_table.clear();
    int fid, size, vid, offset;
    while(fin >> fid) // deal with one line
    {
        if (fid == -99999) // index separator
            break;
        fin >> size;
        vector<ReuseTableInfo> v;
        v.clear();
        for (int i=0; i<size; i++)
        {
            fin >> vid >> offset;
            ReuseTableInfo r;
            r.vid = vid;
            r.offset = offset;
            v.push_back(r);
        }
        frag_reuse_table[fid] = v; // create one entry
    }
    int title_len;
    while(fin >> vid)
    {
        if (vid == -99999) // index separator
            break;
        fin >> title_len;
        vid_titlelen_hash[vid] = title_len;
    }
    fin.close();
}

void validate()
{
    ofstream fout;
    fout.open("validate.txt");
    for (int i=0; i<search_frag.size(); i++)
    {
        vector<Occurence> v = search_frag[i];
        for (int j=0; j<v.size(); j++)
        {
            Occurence o = v[j];
            fout << "term " << i << " Occ " << j << " fid " << o.fid << " pos ";
            for (int k=0; k<o.v_pos.size(); k++)
                fout << o.v_pos[k] << " ";
            fout << endl;
        }
    }
    fout.close();
}

void read_search_frag()
{
    ifstream fin;
    fin.open("search_frag.txt");
    search_frag.clear();
    int fid, pos, term;
    int previous_term=-1;
    while(fin >> term) // each time deal with an occurence
    {
        if (term != previous_term) // meet a new term
        {
            if (term != previous_term+1)
            {
                cout << "missing frags for term " << previous_term+1 << endl;
                exit(1);
            }
            vector<Occurence> cur_v;
            cur_v.clear();
            search_frag.push_back(cur_v);
        }
        vector<Occurence> *v;
        v = &(search_frag[term]);
        Occurence occ;
        fin >> occ.fid;
        occ.v_pos.clear();
        // push_back all the positions
        while((fin >> pos) && (pos != -1))
        {
            occ.v_pos.push_back(pos);
        }
        // push back an occurence
        v->push_back(occ);
        previous_term = term;
    }
    if (previous_term != query_len-1)
    {
        cout << "missing frags for term " << query_len-1 << endl;
        exit(1);
    }
    //validate();
    fin.close();
}

// Print the information after intersection step. 
// Term_number specified how many terms in the query.
void print_intersection(int term_number)
{
    ofstream fout;
    fout.open("output2.txt");
    //fout << intersection_hash.size() << endl;
    unordered_map<int, vector<set<int>>>::iterator iter;
    for (iter = intersection_hash.begin(); iter != intersection_hash.end(); iter++)
    {
        fout << "VID" << iter->first;
        for (int i=0; i<term_number; i++)
        {
            fout << " term" << i;
            set<int> s = (iter->second)[i];
            set<int>::iterator iter2;
            for (iter2 = s.begin(); iter2 != s.end(); iter2++)
                fout << " " << *iter2;
        }
        fout << endl;
    }
    fout.close();
}

bool mycmp(const V_Occurence &obj1, const V_Occurence &obj2)
{
    return obj1.vid < obj2.vid;
}

// This is the first intersection algorithm. Use option1. The complexity is O(n1+n2+...+nq).
void intersection1()
{
    // begin timing
    struct timeval dwstart, dwstart_sort, dwstart_insert, dwstart_merge;
    struct timeval dwend, dwend_sort, dwend_insert, dwend_merge, dwend1;
    double dwtime = 0, dwtime_sort=0, dwtime_insert=0, dwtime_merge=0, dwtime1=0;
    gettimeofday(&dwstart, NULL);
    int term_number = search_frag.size();
    // Step 1: Convert the fragment posting to document posting.
    vector<vector<V_Occurence>> doc_posting(term_number); // each term, a list of vid_occurence, result of step 1 is saved in this.
    doc_posting.clear();
    int i, j, k, l;
    for (i=0; i<term_number; i++) // deal with each query term
    {
        vector<V_Occurence> current_posting; // doc posting for current term
        current_posting.clear();
        
        // algorithm for step 1: insert -> sort -> merge, this can merge the same vids
        
        // insert: insert each posting, the problem is that for different fragments, they might match the same vid, so that there will be multiple entries for the same vid. This duplication will be merged later.
        gettimeofday(&dwstart_insert, NULL);
        vector<Occurence> *vector_occ = &(search_frag[i]);
        for (j=0; j<vector_occ->size(); j++) // each time, deal with one fragment
        {
            Occurence *o = &((*vector_occ)[j]); // the fragment occurence object
            int fid = o->fid; // fid
            vector<ReuseTableInfo> *vector_reuse = &(frag_reuse_table[fid]); // fragment reuse entry for fid
            for (k=0; k<vector_reuse->size(); k++) // each time deal with a reuse version page for a given fragment
            {
                int vid = (*vector_reuse)[k].vid;
                int frag_offset = (*vector_reuse)[k].offset; // fragment's offset in the page
                V_Occurence element;
                current_posting.push_back(element); // firstly push back an empty element
                V_Occurence *v_occur = &(current_posting[current_posting.size()-1]); // get the element just pushed in
                v_occur->vid = vid;
                v_occur->pos.clear();
                for (l=0; l<o->v_pos.size(); l++)
                    v_occur->pos.insert(frag_offset+o->v_pos[l]);
            }
         }
        gettimeofday(&dwend_insert, NULL);
        dwtime_insert += 1000.0*(dwend_insert.tv_sec-dwstart_insert.tv_sec)+(dwend_insert.tv_usec-dwstart_insert.tv_usec)/1000.0;
        // sort: sort current_posting by its element's vid entry
        gettimeofday(&dwstart_sort, NULL);
        sort(current_posting.begin(), current_posting.end(), mycmp);
        gettimeofday(&dwend_sort, NULL);
        dwtime_sort += 1000.0*(dwend_sort.tv_sec-dwstart_sort.tv_sec)+(dwend_sort.tv_usec-dwstart_sort.tv_usec)/1000.0;
        // merge: After sorting, elements with same vid must be adjacent. Merge them.
        gettimeofday(&dwstart_merge, NULL);
        vector<V_Occurence> *merge_result = &(doc_posting[i]);
        merge_result->clear();
        merge_result->push_back(current_posting[0]); // insert first element
        int pre_index = 0;
        for (j=1; j<current_posting.size(); j++)
        {
            if (current_posting[j].vid != (*merge_result)[pre_index].vid){
                merge_result->push_back(current_posting[j]);
                pre_index++;
            }
            else // merge
            {
                set<int>::iterator iter;
                for (iter = current_posting[j].pos.begin(); iter != current_posting[j].pos.end(); iter++)
                    (*merge_result)[pre_index].pos.insert(*iter);
            }
        }
        gettimeofday(&dwend_merge, NULL);
        dwtime_merge += 1000.0*(dwend_merge.tv_sec-dwstart_merge.tv_sec)+(dwend_merge.tv_usec-dwstart_merge.tv_usec)/1000.0;
    }
    gettimeofday(&dwend1, NULL);
    dwtime1 = 1000.0*(dwend1.tv_sec-dwstart.tv_sec)+(dwend1.tv_usec-dwstart.tv_usec)/1000.0;
    cout << "Step1_insert:" << dwtime_insert << "ms" << endl;
    cout << "Step1_sort:" << dwtime_sort << "ms" << endl;
    cout << "Step1_merge:" << dwtime_merge << "ms" << endl;
    cout << "Step1:" << dwtime1 << "ms"<< endl;
    
    // Step 2: Calculate intersection using algorithm Option 1.
    // After step 1, vector<vector<V_Occurence>> doc_posting stores the position information for each term.
    // Here, we will intersect them.
    int *pointer = new int[term_number];
    int *p_size = new int[term_number];
    for (i=0; i<term_number; i++){
        pointer[i] = 0;
        p_size[i] = doc_posting[i].size();
    }
    int current_focus;
    result_posting.clear();
    bool while_flag = true;
    while(while_flag){
        int max, max_pointer;
        max = -99999;
        for (i=0; i<term_number; i++)
        {
            // find max value
            if (max < doc_posting[i][pointer[i]].vid)
            {
                max = doc_posting[i][pointer[i]].vid;
                max_pointer = i;
            }
        }
        // increase each value to max or bigger
        for (i=0; i<term_number; i++)
        {
            while (doc_posting[i][pointer[i]].vid < max){ // increase the pointer until bigger or equal than max
                pointer[i]++;
                if (pointer[i] >= p_size[i]) // no more element in the posting, exit while loop
                {
                    while_flag = false;
                    break;
                }
            }
        }
        if (!while_flag)
            break;
        // check if all term-postings are the same vid
        bool if_same = true;
        for (i=0; i<term_number; i++)
            if (doc_posting[i][pointer[i]].vid != max){
                if_same = false;
                break;
            }
        // if yes, save it to result_posting
        if (if_same){
            R_Occurence *r_occur = new R_Occurence();
            r_occur->vid = max;
            for (i=0; i<term_number; i++)
                r_occur->pos.push_back(doc_posting[i][pointer[i]].pos);
            result_posting.push_back(*r_occur);
            // increase each posting and check with p_size
            for (i=0; i<term_number; i++){
                pointer[i]++;
                if (pointer[i] >= p_size[i]){
                    while_flag = false;
                    break;
                }
            }
            if (!while_flag)
                break;
        }
    }

    // end timing
    gettimeofday(&dwend, NULL);
    dwtime = 1000.0*(dwend.tv_sec-dwstart.tv_sec)+(dwend.tv_usec-dwstart.tv_usec)/1000.0;
    cout << "Step2:" << dwtime-dwtime1 << "ms"<< endl;
    cout << "result_posting:" << result_posting.size() << endl;
    delete [] pointer;
    delete [] p_size;
}

void intersection()
{
    // added by Susen
    start = clock();  // start ticking
    // added by Susen
    int term_number = search_frag.size();
    // init our intersection data structure "intersection_hash" for the first term
    vector<Occurence> v_occ;
    v_occ = search_frag[0];
    int i, j, m;
    for (i=0; i<v_occ.size(); i++) // each time, deal with one fragment occurence
    {
        Occurence *o = &(v_occ[i]); // the fragment occurence object
        int fid = o->fid; // fid
        vector<ReuseTableInfo> v_reuse = frag_reuse_table[fid]; // fragment reuse entry for fid
        for (j=0; j<v_reuse.size(); j++) // each time deal with a reuse version page for a given fragment
        {
            int vid = v_reuse[j].vid;
            int frag_offset = v_reuse[j].offset; // fragment's offset in the page
            unordered_map<int, vector<set<int>>>::iterator iter = intersection_hash.find(vid);
            vector<set<int>> *p; // pointer of the value of intersection_hash[vid]
            if (iter != intersection_hash.end()) // find vid saved already
            {
                p = &(iter->second);
            }
            else// not found
            {
                vector<set<int>> new_entry; new_entry.clear();
                set<int> t; t.clear();
                new_entry.push_back(t);
                intersection_hash[vid] = new_entry; // insert new_entry
                p = &(intersection_hash[vid]);
            }
            if (p->size()<1) // shouldn't be smaller than 1
            {
                cout << "wrong! p->size()<1, vid=" << vid  << endl;
                exit(1);
            }
            set<int> *pos_set = &((*p)[0]);
            for (m=0; m<(o->v_pos).size(); m++) // loop for each position in the fragment occurence
                pos_set->insert(o->v_pos[m]+frag_offset); // insert into the set; thus if duplicate, will not save
        }
    }
    // do for the following terms: term1, term2, ..., term(n-1)
    int term;
    for (term = 1; term<term_number; term++) // each time, deal with one term
    {
        unordered_map<int, vector<set<int>>> temp_intersection_hash;
        temp_intersection_hash.clear();

        v_occ = search_frag[term];
        for (i=0; i<v_occ.size(); i++) // each time, deal with one fragment occurence
        {
            Occurence *o = &(v_occ[i]);
            int fid = o->fid;
            vector<ReuseTableInfo> v_reuse = frag_reuse_table[fid];
            for (j=0; j<v_reuse.size(); j++)
            {
                int vid = v_reuse[j].vid;
                int frag_offset = v_reuse[j].offset;
                unordered_map<int, vector<set<int>>>::iterator iter = intersection_hash.find(vid);
                vector<set<int>> *p;
                if (iter != intersection_hash.end()) // find
                {
                    p = &(iter->second);
                }
                else // not found, disregard this vid because a page must contain all the previous terms
                    continue; 
                // check if we have this entry in temp_intersection_hash
                unordered_map<int, vector<set<int>>>::iterator iter_temp = temp_intersection_hash.find(vid);
                if (iter_temp == temp_intersection_hash.end()) // not find, copy the entry from intersection_hash and create a new set for this term
                {
                    vector<set<int>> cp_entry = *p;
                    set<int> t; t.clear();
                    cp_entry.push_back(t);
                    temp_intersection_hash[vid] = cp_entry;
                }
                // if find, don't need to do anything because the new set for this term should be created before
                p = &(temp_intersection_hash[vid]);
                if (p->size()<term+1) // shouldn't be smaller than term+1
                {
                    cout << "wrong! p->size()<term+1, vid=" << vid<< endl;
                    exit(1);
                }
                set<int> *pos_set = &((*p)[term]);
                for (m=0; m<(o->v_pos).size(); m++)
                {
                    pos_set->insert((o->v_pos)[m]+frag_offset);
                }
            }
        }
        intersection_hash = temp_intersection_hash; // update the new "intersection_hash"
    }
    //print_intersection(term_number);
    // added by Susen
    duration = (clock() - start) / (double) CLOCKS_PER_SEC;
    cout << "Duration without I/O: " << duration*1000 << "ms" << endl;
    start = clock();
    // added by Susen
}

// calculate the min span in title: sliding window algorithm O(n)
void cal_min_span_title(int &min_span, int vid, vector<set<int>> &occur_terms, int term_number, int title_len)
{
    // cur_pos saves the current focus of terms' positions in the algorithm
    int *cur_pos = new int[term_number];
    int *cur_index = new int[term_number];
    if (term_number != occur_terms.size())
    {
        cout << "error in cal_min_span_title: term_number != occur_terms.size()" << endl;
        exit(1);
    }
    // copy occur_terms to vec_occ because set cannot be sorted
    vector<vector<int>> vec_occ(term_number);
    for (int i=0; i<term_number; i++)
    {
        vec_occ[i].clear();
        copy(occur_terms[i].begin(), occur_terms[i].end(), back_inserter(vec_occ[i]));
        sort(vec_occ[i].begin(), vec_occ[i].end()); // sort each term's positions from small to large
        cur_pos[i] = vec_occ[i][0]; // let cur_pos focus on the first occurence of each term
        cur_index[i] = 0;
    }
    // each time check if need to update min_span, move min_pos to next to check the next loop
    while(1)
    {
        int min, max, min_index, max_index;
        min = 99999, max = -99999;
        // every time, min is the smallest position among all current term position focus, max is the largest one
        for (int i=0; i<term_number; i++)
        {
            if (min > cur_pos[i]) {min=cur_pos[i]; min_index=i;}
            if (max < cur_pos[i]) {max=cur_pos[i]; max_index=i;}
        }
        // if max is not in title, terminate the loop
        if (max >= title_len)
            break;
        if (max - min + 1 < term_number) // double check: If true, something is wrong with our index.
        {
            cout << "error in cal_min_span: max-min+1<term_number" << endl;
            cout << "vid=" << vid << " min=" << min << " max=" << max << endl;
            exit(1);
        }

        if (min_span > max - min + 1) // check if we need to update min_span
            min_span = max - min + 1;
        // termination condition is cur_index[min_index] is the last one
        if (cur_index[min_index] == vec_occ[min_index].size() - 1)
            break;
        // otherwise, find the next one
        cur_index[min_index]++;
        cur_pos[min_index] = vec_occ[min_index][cur_index[min_index]];
    }
    delete []cur_pos;
    delete []cur_index;
}

// calculate the min span in body: sliding window algorithm O(n)
void cal_min_span(int &min_span, int vid, vector<set<int>> &occur_terms, int term_number)
{
    // cur_pos saves the current focus of terms' positions in the algorithm
    int *cur_pos = new int[term_number];
    int *cur_index = new int[term_number];
    if (term_number != occur_terms.size())
    {
        cout << "error in cal_min_span: term_number != occur_terms.size()" << endl;
        exit(1);
    }
    // copy occur_terms to vec_occ because set cannot be sorted
    vector<vector<int>> vec_occ(term_number);
    for (int i=0; i<term_number; i++)
    {
        vec_occ[i].clear();
        copy(occur_terms[i].begin(), occur_terms[i].end(), back_inserter(vec_occ[i]));
        sort(vec_occ[i].begin(), vec_occ[i].end()); // sort each term's positions from small to large
        cur_pos[i] = vec_occ[i][0]; // let cur_pos focus on the first occurence of each term
        cur_index[i] = 0;
    }
    // each time check if need to update min_span, move min_pos to next to check the next loop
    while(1)
    {
        int min, max, min_index, max_index;
        min = 99999, max = -99999;
        for (int i=0; i<term_number; i++)
        {
            if (min > cur_pos[i]) {min=cur_pos[i]; min_index=i;}
            if (max < cur_pos[i]) {max=cur_pos[i]; max_index=i;}
        }
        if (max - min + 1 < term_number) // double check: If true, something is wrong with our index.
        {
            cout << "error in cal_min_span: max-min+1<term_number" << endl;
            cout << "vid=" << vid << " min=" << min << " max=" << max << endl;
            exit(1);
        }

        if (min_span > max - min + 1) // check if we need to update min_span
            min_span = max - min + 1;
        // termination condition is cur_index[min_index] is the last one
        if (cur_index[min_index] == vec_occ[min_index].size() - 1)
            break;
        // otherwise, find the next one
        cur_index[min_index]++;
        cur_pos[min_index] = vec_occ[min_index][cur_index[min_index]];
    }
    delete []cur_pos;
    delete []cur_index;
}

int body_tf(int vid, vector<set<int>> &occur_terms, int term_number)
{
    if (term_number != occur_terms.size())
    {
        cout << "error in body_tf: term_number != occur_terms.size()" << endl;
        exit(1);
    }
    int *tf = new int[term_number];
    int min_tf = 99999;
    for (int i=0; i<term_number; i++)
    {
        tf[i] = occur_terms[i].size();
        if (min_tf > tf[i])
            min_tf = tf[i];
    }
    return min_tf;
}

void score_page(int vid, vector<set<int>> &occur_terms)
{
    ScoreResult new_entry;
    new_entry.vid = vid;
    double score;
    // now we need to calculate the score for vid
    double title_score, body_score;
    double text_appear_score, proximity_score;
    int term_number = search_frag.size();
    
    // calculate for title:
    int title_len = vid_titlelen_hash[vid];
    int appear=0;
    int min_span=9999;
    for (int i=0; i<term_number; i++)
    {
        bool if_cal_appear = false; // flag
        set<int>::iterator iter_set;
        for (iter_set = occur_terms[i].begin(); iter_set != occur_terms[i].end(); iter_set++)
        {
            if (*iter_set < title_len && if_cal_appear == false)
            {
                appear++;
                if_cal_appear = true;
            }
        }
    }
    text_appear_score = (double)appear / term_number;
    if (appear != term_number) // if not all terms occur in title, no proximity_score
        proximity_score = 0;
    else
    {
        min_span=9999;
        cal_min_span_title(min_span, vid, occur_terms, term_number, title_len);
        if (min_span < term_number)
        {
            cout << "min_span < term_number for title" << " something is wrong for cal_min_span_title" << endl;
            exit(1);
        }
        proximity_score = (double)term_number/min_span;
    }
    title_score = 1.0/6.0*text_appear_score + 5.0/6.0*proximity_score;
    
    // calculate for body:
    
    // old text_appear_score = 1
    //text_appear_score = 1; // text_appear_score=1 because in body we ensure that all text appears
    
    // new text_appear_score = log2(TF+1), TF is the min of all TFs.
    text_appear_score = log2(1.0+body_tf(vid, occur_terms, term_number));

    min_span=9999;
    cal_min_span(min_span, vid, occur_terms, term_number);
    proximity_score = (double)term_number/min_span; 
    body_score = 1.0/6.0*text_appear_score+5.0/6.0*proximity_score;

    score = 5.0/6.0*title_score + 1.0/6.0*body_score;
    new_entry.score = score;

    score_result.push_back(new_entry);
}

bool my_sort_function(const ScoreResult &v1, const ScoreResult &v2) { return (v1.score > v2.score); }

void scoring()
{
    // go through the intersection_hash and score each entry
    score_result.clear();
    unordered_map<int, vector<set<int>>>::iterator iter;
    for (iter = intersection_hash.begin(); iter != intersection_hash.end(); iter++)
    {
        score_page(iter->first, iter->second);
    }
    // sort score_result
    sort(score_result.begin(), score_result.end(), my_sort_function);
    // added by Susen
    //duration += (clock() - start) / (double) CLOCKS_PER_SEC;  // added
    // added by Susen
    ofstream fout;
    fout.open("result.txt");
    for (int i=0; i<score_result.size(); i++)
        fout << score_result[i].vid << " " << score_result[i].score<< endl;
    fout.close();
}

void init()
{
    vid_titlelen_hash.clear();
    intersection_hash.clear();
}

void print_vid()
{
    ofstream fout;
    fout.open("vidlist.txt");
    for (int i=0; i<result_posting.size(); i++)
        fout << result_posting[i].vid << " ";
    fout.close();
}

int main(int argc, char** argv)
{
    if (argc !=2)
    {
        cout << "need one parameter for query_len" << endl;
        exit(1);
    }
    query_len=atoi(argv[1]);
    init();
    read_index();
    read_search_frag();
    //intersection();
    intersection1();
    print_vid();
    //scoring();
    return 0;
}

