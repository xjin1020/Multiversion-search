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

// fid occurence
class Occurence
{
public:
    int fid;
    vector<int> v_pos;
};

// vid occurence
class V_Occurence
{
public:
    int vid;
    set<int> pos;
};

class ForwardTableInfo
{
public:
    int fid;
    int offset;
};

class ReuseTableInfo
{
public:
    int vid;
    int offset;
};

const int MAX_VID = 5500000;//max number of VID
const int y = 13; // average number of fragments in one page: 13 wiki, 22 web, 17 git

unordered_map<int, vector<ReuseTableInfo>> frag_reuse_table;
unordered_map<int ,int> vid_titlelen_hash; // vid to title length
vector<ForwardTableInfo> forward_table[MAX_VID];
vector<vector<Occurence>> search_frag; // <<fID, <pos>>>
vector<int> vid_list; // vid list
unordered_map<int, vector<vector<int>>*> vid_posting; // vid_posting, for each vid, there will be a posting
int query_len;

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

// print search_frag in txt format
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

void print_vid_posting()
{
    ofstream fout;
    fout.open("posting.txt");
    unordered_map<int, vector<vector<int>>*>::iterator iter;
    for (iter = vid_posting.begin(); iter != vid_posting.end(); iter++){
        fout << iter->first << " ";
        vector<vector<int>> *v = iter->second;
        for (int i=0; i<(*v).size(); i++){
            fout << "term " << i << ": ";
            for (int j=0; j<(*v)[i].size(); j++)
                fout << (*v)[i][j] << " ";
        }
        fout << endl;
    }
    fout.close();
}

bool mycmp(const Occurence& o1, const Occurence& o2)
{
    return (o1.fid < o2.fid);
}

bool mycmpA(const V_Occurence &obj1, const V_Occurence &obj2)
{
        return obj1.vid < obj2.vid;
}

// read the postings of terms and sort them by fid
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

    fin.close();

    // sort search_frag to make each posing sorted by fid
    for (int i=0; i<query_len; i++){
        sort(search_frag[i].begin(), search_frag[i].end(), mycmp);
        cout << "Term " << i << " contains " << search_frag[i].size() << " postings" << endl;
    }
    //validate();
}

// read vids
void read_vid()
{
    ifstream fin;
    fin.open("vidlist.txt");
    vid_list.clear();
    int v;
    while(fin >> v)
        vid_list.push_back(v);
    cout << "Read "<< vid_list.size() << " vids from previous intersection results."<< endl;
    fin.close();
}

// read forward reuse table
void read_forward()
{
    ifstream fin;
    fin.open("forward.txt");
    int vid, size, fid, offset;
    int count=0;
    while(fin >> vid) // deal with one line
    {
        count++;
        fin >> size;
        vector<ForwardTableInfo> v;
        v.clear();
        for (int i=0; i<size; i++)
        {
            fin >> fid >> offset;
            ForwardTableInfo r;
            r.fid = fid;
            r.offset = offset;
            v.push_back(r);
        }
        forward_table[vid] = v; // create one entry for vid
    }
    fin.close();
    cout << "Read " << count << " forward reuse table entries in forward_table." << endl;
}

// binary search within [start, end] using recursion
int find_index(vector<Occurence> *term_posting, int fid, int start, int end)
{
    if (start > end)
        return -1; // error! fid not in term_posting
    int mid = (start+end)/2;
    if ((*term_posting)[mid].fid == fid)
        return mid;
    else if (fid < (*term_posting)[mid].fid)
        return find_index(term_posting, fid, start, mid-1);
    else
        return find_index(term_posting, fid, mid+1, end);
}

// calculate the positional information for each vid
// each vid: vector<set<int>>, each term corresponds to a set of positions
void get_positional_info()
{
    // begin timing
    struct timeval dwstart, dwend, dwstart_sort, dwend_sort;
    double dwtime = 0, dwtime_sort=0;
    int vid, fid, frag_offset;
    // init Option C's data structures
    vector<vector<int>> *e = new vector<vector<int>>[vid_list.size()];
    for (int i=0; i<vid_list.size(); i++)
        e[i].resize(query_len);
    bool flag_do_optionA = 1;
    // init Option A's data structures
    int term_number = query_len;
    vector<vector<V_Occurence>> doc_posting(term_number); // each term, a list of vid_occurence, result of step 1 is saved in this.
    doc_posting.clear();

    // begin optionD algorithm 
    double ratio;
    if (query_len<5)
        ratio = (5.0-query_len)/4.0;
    else
        ratio = 0.2;
    ratio = 0.4;
    gettimeofday(&dwstart, NULL);
    for (int k=0; k<query_len; k++) // each time, deal with one term
    {
        int f = search_frag[k].size();
        flag_do_optionA = f < (y*ratio);
        if (flag_do_optionA)
        {
            // Option A:
            // Step 1: Convert the fragment posting to document posting.
            vector<V_Occurence> current_posting; // doc posting for current term
            current_posting.clear();

            // algorithm for step 1: insert -> sort -> merge, this can merge the same vids
            // insert: insert each posting, the problem is that for different fragments, they might match the same vid, so that there will be multiple entries for the same vid. This duplication will be merged later.
            vector<Occurence> *vector_occ = &(search_frag[k]);
            for (int j=0; j<vector_occ->size(); j++) // each time, deal with one fragment
            {
                Occurence *o = &((*vector_occ)[j]); // the fragment occurence object
                int fid = o->fid; // fid
                vector<ReuseTableInfo> *vector_reuse = &(frag_reuse_table[fid]); // fragment reuse entry for fid
                for (int m=0; m<vector_reuse->size(); m++) // each time deal with a reuse version page for a given fragment
                {
                    int vid = (*vector_reuse)[m].vid;
                    int frag_offset = (*vector_reuse)[m].offset; // fragment's offset in the page
                    V_Occurence element;
                    current_posting.push_back(element); // firstly push back an empty element
                    V_Occurence *v_occur = &(current_posting[current_posting.size()-1]); // get the element just pushed in
                    v_occur->vid = vid;
                    v_occur->pos.clear();
                    for (int l=0; l<o->v_pos.size(); l++)
                        v_occur->pos.insert(frag_offset+o->v_pos[l]);
                }
            }
            // sort: sort current_posting by its element's vid entry
            gettimeofday(&dwstart_sort, NULL);
            sort(current_posting.begin(), current_posting.end(), mycmpA);
            gettimeofday(&dwend_sort, NULL);
            dwtime_sort += 1000.0*(dwend_sort.tv_sec-dwstart_sort.tv_sec)+(dwend_sort.tv_usec-dwstart_sort.tv_usec)/1000.0;
            // merge: After sorting, elements with same vid must be adjacent. Merge them.
            vector<V_Occurence> *merge_result = &(doc_posting[k]);
            merge_result->clear();
            merge_result->push_back(current_posting[0]); // insert first element
            int pre_index = 0;
            for (int j=1; j<current_posting.size(); j++)
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
        }
        else
        {
            // Option C:
            vector<Occurence> *term_posting = &(search_frag[k]);
            for (int i=0; i<vid_list.size(); i++) // each time, deal with one vid
            {
                vid = vid_list[i];
                //vector<vector<int>> *e = new vector<vector<int>>(query_len);
                for (int j=0; j<forward_table[vid].size(); j++) // each time, deal with one fid in the vid
                {
                    fid = forward_table[vid][j].fid;
                    frag_offset = forward_table[vid][j].offset;
                    // for each fid, lookup the sorted term-fid postings
                    // binary search fid in term_posting O(log(n))
                    int index = find_index(term_posting, fid, 0, term_posting->size()-1);
                    if (index != -1){
                        vector<int>* vpos = &((*term_posting)[index].v_pos);
                        // for each position, insert it into vid_posting
                        for (int l=0; l<vpos->size(); l++)
                            (e[i])[k].push_back((*vpos)[l]+frag_offset);
                    }           
                }
            }
        }
    }
    // In option D, vid_posing[vid] might only contain parts of query terms which is calculated using Option C; for the others, they will appear in Option A's result
    for (int i=0; i<vid_list.size(); i++)
        vid_posting[vid] = &(e[i]);
    // end OptionD algorithm

    // end timing
    gettimeofday(&dwend, NULL);
    dwtime = 1000.0*(dwend.tv_sec-dwstart.tv_sec)+(dwend.tv_usec-dwstart.tv_usec)/1000.0;
    cout << "Calculate positional info: " << dwtime-dwtime_sort << "ms" << endl;
    //print_vid_posting(); // validate results posting
}

int main(int argc, char** argv)
{
    if (argc !=2)
    {
        cout << "need one parameter for query_len" << endl;
        exit(1);
    }
    query_len=atoi(argv[1]);
    // read reuse table: fid -> a list of vids for Option A
    read_index();
    // read postings
    read_search_frag();
    // read vids for Option C
    read_vid();
    // read forward reuse table: vid -> a list of fids for Option C
    read_forward();
    // calculate the positional information for each vid
    get_positional_info();
    return 0;
}

