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

// vid occurence
class Occurence
{
public:
    int vid;
    vector<int> pos;
};

// result occurence
class R_Occurence
{
public:
    int vid;
    vector<vector<int>> pos;
};

vector<vector<Occurence>> search_frag; // <<vid, <pos>>>
vector<R_Occurence> result_posting; // this keeps the result posting after intersection
int query_len;
clock_t start; // global variable added to mark the starting point of time statistics
double duration;  // global variable added to calculate non-I/O time

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
            fout << "term " << i << " Occ " << j << " vid " << o.vid << " pos ";
            for (int k=0; k<o.pos.size(); k++)
                fout << o.pos[k] << " ";
            fout << endl;
        }
    }
    fout.close();
}

void read_search_frag()
{
    ifstream fin;
    fin.open("../lucene_step_tradi/search_frag.txt");
    search_frag.clear();
    int vid, pos, term;
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
        fin >> occ.vid;
        occ.pos.clear();
        // push_back all the positions
        while((fin >> pos) && (pos != -1))
        {
            occ.pos.push_back(pos);
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

bool mycmp(const Occurence &obj1, const Occurence &obj2)
{
    return obj1.vid < obj2.vid;
}

// This is the first intersection algorithm. Use option jump list. The complexity is O(n1+n2+...+nq).
void intersection1()
{
    // sort: sort search_frag by its element's vid entry
    // We assume in the index the occurrences are already sorted according to their vid so this sorting time is not included.
    int i;
    for (i=0; i<search_frag.size(); i++)
        sort(search_frag[i].begin(), search_frag[i].end(), mycmp);
    
    // begin timing
    struct timeval dwstart, dwend;
    gettimeofday(&dwstart, NULL);
    double dwtime = 0;
    int term_number = search_frag.size();
    // Calculate intersection using jump list algorithm.
    // After reading search_frag.txt, vector<vector<Occurence>> search_frag stores the position information for each term.
    // Here, we will intersect them.
    int *pointer = new int[term_number];
    int *p_size = new int[term_number];
    for (i=0; i<term_number; i++){
        pointer[i] = 0;
        p_size[i] = search_frag[i].size();
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
            if (max < search_frag[i][pointer[i]].vid)
            {
                max = search_frag[i][pointer[i]].vid;
                max_pointer = i;
            }
        }
        // increase each value to max or bigger
        for (i=0; i<term_number; i++)
        {
            while (search_frag[i][pointer[i]].vid < max){
                // increase the pointer until bigger or equal than max
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
        // check if all term_postings are the same vid
        bool if_same = true;
        for (i=0; i<term_number; i++)
            if (search_frag[i][pointer[i]].vid != max){
                if_same = false;
                break;
            }
        // if yes, save it to result_posting
        if (if_same) {
            R_Occurence *r_occur = new R_Occurence();
            result_posting.push_back(*r_occur);
            r_occur->vid = max;
            for (i=0; i<term_number; i++)
                r_occur->pos.push_back(search_frag[i][pointer[i]].pos);
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
    cout << "intersection costs: " << dwtime << " ms"<< endl;
    cout << "result_posting: " << result_posting.size() << endl;
    delete [] pointer;
    delete [] p_size;
}

int main(int argc, char** argv)
{
    if (argc !=2)
    {
        cout << "need one parameter for query_len" << endl;
        exit(1);
    }
    query_len=atoi(argv[1]);
    //init();
    read_search_frag();
    //intersection();
    intersection1();
    //scoring();
    return 0;
}
