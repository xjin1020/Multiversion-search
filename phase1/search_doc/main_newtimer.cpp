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

unordered_map<int, vector<ReuseTableInfo>> frag_reuse_table;
vector<vector<Occurence>> search_frag; // <<fID, <pos>>>
unordered_map<int ,int> vid_titlelen_hash; // vid to title length
unordered_map<int, vector<set<int>>> intersection_hash; // this keeps the intersection results
vector<ScoreResult> score_result; // this vector keeps the final scoring results
int query_len;

struct timeval start, end; // global variable added to mark the starting and ending point of time

void read_index()
{
    ifstream fin;
    fin.open("../gen_index/index.txt");
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
    fin.open("../lucene_step/search_frag.txt");
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

void intersection()
{
    gettimeofday(&start, NULL);
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
    gettimeofday(&end, NULL);
    double timeuse = 1000.0 * ( 0.0 + end.tv_sec - start.tv_sec ) + (end.tv_usec - start.tv_usec+0.0)/1000.0; 
    printf("time: %lf ms\n", timeuse);
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
    intersection();
    scoring();
    return 0;
}

