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

class ForwardTableInfo
{
public:
    int fid;
    int offset;
};

const int MAX_VID = 5500000;//max number of VID

vector<ForwardTableInfo> forward_table[MAX_VID];
vector<vector<Occurence>> search_frag; // <<fID, <pos>>>
vector<int> vid_list; // vid list
unordered_map<int, vector<vector<int>>*> vid_posting; // vid_posting, for each vid, there will be a posting
int query_len;

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
    struct timeval dwstart, dwend;
    double dwtime = 0;
    int vid, fid, frag_offset;
    vector<vector<int>> *e = new vector<vector<int>>[vid_list.size()];
    for (int i=0; i<vid_list.size(); i++)
        e[i].resize(query_len);
    gettimeofday(&dwstart, NULL);
    for (int k=0; k<query_len; k++) // each time, deal with one term
    {
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
    for (int i=0; i<vid_list.size(); i++)
        vid_posting[vid] = &(e[i]);
    // end timing
    gettimeofday(&dwend, NULL);
    dwtime = 1000.0*(dwend.tv_sec-dwstart.tv_sec)+(dwend.tv_usec-dwstart.tv_usec)/1000.0;
    cout << "Calculate positional info: " << dwtime << "ms" << endl;
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
    // read postings
    read_search_frag();
    // read vids
    read_vid();
    // read forward reuse table: vid -> a list of fids
    read_forward();
    // calculate the positional information for each vid
    get_positional_info();
    return 0;
}

