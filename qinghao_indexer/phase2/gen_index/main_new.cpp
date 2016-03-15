/** This project is used to generate fragment-based index for multiversion data.
 *  Author: Qinghao Wu
 *  Date: Mar.11.2016
 **/
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <unordered_map>
using namespace std;

//#define VERSION_NO 5
//const int doc_number[VERSION_NO] = {1000, 1000, 1000, 1000, 1000}; // the number of docs in each version
int VERSION_NO;
int *doc_number;

int total_found_fragment;
int did, next_did; // document id
int vid, next_vid; // version id
int fid, next_fid; // fragment id
unordered_map<string, int> title_did_hash; // hash from title to document id
unordered_map<int, int> vid_did_hash; // hash from vid to did
unordered_map<int, vector<int>> did_vid_hash; // hash from did to vid; each document will have a vector of vid
unordered_map<int, int> fid_size_hash; // hash from fid to its size
unordered_map<int, string> fid_content_hash; // hash from fid to its content
unordered_map<string, int> content_fid_hash; // hash from a fragment's content to its id
unordered_map<int, vector<int>> vid_fid_hash; // hash from vid to fid; each page will have a vector of fragment
unordered_map<int ,string> did_title_hash; // hash from did to title 

class ReuseTableInfo
{
public:
    int vid;
    int offset;
};

unordered_map<int, vector<ReuseTableInfo>> frag_reuse_table;

/** location of the docs in version $i: ../fragment/result$(i+1)/XX.txt
 **/

// find the document id of a page given its title
int get_did_from_title(string &title)
{
    int id;
    unordered_map<string, int>::const_iterator iter = title_did_hash.find(title); 
    if (iter != title_did_hash.end()) // find docid existing in current hash
        id = iter->second; 
    else // not found
    {
        id = next_did; 
        next_did++;
        title_did_hash[title] = id;
    }
    return id;
}

// maintain did_vid_hash
void maintain_did_vid_hash()
{
    unordered_map<int, vector<int>>::iterator iter = did_vid_hash.find(did);
    if (iter != did_vid_hash.end()) // found
    {
        vector<int> *p;
        p = &(iter->second);
        p->push_back(vid);
    }
    else // not found
    {
        vector<int> new_vector;
        new_vector.clear();
        new_vector.push_back(vid);
        did_vid_hash[did] = new_vector;
    }
}

// calculate the number of words in a string
int get_words_number(string &line)
{
    int number=1;
    for (int i=0; i<line.length(); i++)
    {
        if (line[i] == ' ')
            number++;
    }
    return number;
}

void output_fragment(int fid, string &line)
{
    ofstream fout;
    //ostringstream oss;
    //oss << "fragments/" << fid << ".txt";
    //fout.open(oss.str().c_str());
    //fout << line;
    //fout.close();

    fout.open("all_fragments.txt", ofstream::app);
    fout << line << endl;
    fout.close();
}

void deal_with_title(string title, int pageno)
{
    // If the title is empty, we don't deal with this page. Regard it as an error one.    
    if (title.length() < 2)
    {
        cout << "Empty title at page " << pageno << ". Disregard it!"<< endl;
        return;
    }
    // If we get the title correctly, deal with this page.
    did = get_did_from_title(title);
    // maintain vid_did_hash
    vid_did_hash[vid] = did;
    // maintain did_title_hash
    did_title_hash[did] = title;
    // maintain did_vid_hash: add vid to the list of did
    maintain_did_vid_hash();
}

// do the work for all the fragments
void do_fragments(ifstream &fin, int pageno)
{
    string line;
    int current_offset=0; // cumulated words offset within page
    int frag_words_number=0; // the words number of current fragment
	bool if_title = true; // at first, set it true
    while(getline(fin, line))
    {
    	// if this is title, deal with title firstly, then treat it as a fragment
	    if (if_title)
	    {
	    	deal_with_title(line, pageno);
	    	if_title = false;
	    }
        // each time, get a line, it is a fragment
        // Step 1, get the fid: check if this fragment occurs before. 
        // If yes, get the old fid; otherwise, give it a new fid.
        unordered_map<string, int>::const_iterator iter = content_fid_hash.find(line);
        if (iter != content_fid_hash.end()) // found; If we have saved this fragment before, we don't need to maintain this fragment's info.
        {
            total_found_fragment++;
            fid = iter->second;
            frag_words_number = fid_size_hash[fid];
        }
        else // not found
        {
            fid = next_fid++; // give it a new fid
            // maintain this fragment's info
            content_fid_hash[line] = fid;
            fid_content_hash[fid] = line;
            frag_words_number = get_words_number(line); // If it's the first time we meet this fragment, we must calculate its size.
            fid_size_hash[fid] = frag_words_number;
            // output this fragment
            output_fragment(fid, line);
        }

        // Step 2, maintain vid_fid_hash
        unordered_map<int, vector<int>>::iterator iter2 = vid_fid_hash.find(vid);
        if (iter2 != vid_fid_hash.end()) // found
        {
            vector<int> *p2 = &(iter2->second);
            p2->push_back(fid);
        }
        else // not found
        {
            vector<int> new_vector;
            new_vector.clear();
            new_vector.push_back(fid);
            vid_fid_hash[vid] = new_vector;
        }

        // Step 3, maintain frag_reuse_table
        ReuseTableInfo new_element;
        new_element.vid = vid;
        new_element.offset = current_offset;
        
        unordered_map<int, vector<ReuseTableInfo>>::iterator iter3 = frag_reuse_table.find(fid);
        if (iter3 != frag_reuse_table.end()) // found
        {
            vector<ReuseTableInfo> *p = &(iter3->second);
            p->push_back(new_element);
        }
        else // not found
        {
            vector<ReuseTableInfo> new_vector;
            new_vector.clear();
            new_vector.push_back(new_element);
            frag_reuse_table[fid] = new_vector;
        }

        // At last, add frag_words_number to current_offset
        current_offset += frag_words_number;
    }
}

// do the work for a page which can be acquired using the input file stream "fin"
void do_page(ifstream &fin, int pageno)
{
    vid = next_vid++;
    
    // deal with the fragments now
    do_fragments(fin, pageno);
}

// do the work for the whole version v
void do_version(int v)
{
    //cout << endl << "Begin doing version " << v << endl;
    cout << endl << "Begin... " << endl;
    ifstream fin;
    string filename;
    ostringstream oss;
    oss << "./"; // plus 1 because version i is saved in result$(i+1)
    string base_file = oss.str();
    for (int i=0; i<doc_number[v]; i++)
    {
        ostringstream newoss;
        newoss << i << ".txt";
        filename = base_file + newoss.str();
        fin.open(filename.c_str());
        //cout << "page: " << i << " ";
        do_page(fin, i); 
        fin.close();
    }
}

void init()
{
    total_found_fragment=0;
    next_did = next_vid = next_fid = 0;
    title_did_hash.clear();
    vid_did_hash.clear();
    did_vid_hash.clear();
    fid_size_hash.clear();
    fid_content_hash.clear();
    content_fid_hash.clear();
    vid_fid_hash.clear();
    frag_reuse_table.clear();
    did_title_hash.clear();
}

void output_index()
{
    ofstream findex;
    findex.open("index.txt");
    // output index in txt format
    // output reuse_table
    unordered_map<int, vector<ReuseTableInfo>>::iterator iter;
    for (iter = frag_reuse_table.begin(); iter != frag_reuse_table.end(); iter++)
    {
        findex << iter->first << " ";
        vector<ReuseTableInfo> v;
        v = iter->second;
        findex << v.size() << " ";
        for (int i=0; i<v.size(); i++){
            findex << v[i].vid << " " << v[i].offset << " ";
        }
        findex << endl;
    }
    findex << -99999 << endl; // index separator
    // output vid title hash
    unordered_map<int ,int>::iterator iter2; // iteration of vid_did_hash
    for (iter2 = vid_did_hash.begin(); iter2 != vid_did_hash.end(); iter2++)
    {
        string title = did_title_hash[iter2->second];
        // calculate how many words
        int words_number=1;
        for (int i=0; i<title.length(); i++)
            if (title[i] == ' ')
                words_number++;
        findex << iter2->first << " " << words_number<< endl;
    }
    findex << -99999 << endl; // index separator
    findex.close();
}

// output the relationship between version and document
void output_dvrelation()
{
    ofstream freldv, frelvd;
    freldv.open("dvrelation.txt");
    frelvd.open("vdrelation.txt");
    // output vid_did_hash
    unordered_map<int, int>::iterator iter;
    for (iter = vid_did_hash.begin(); iter != vid_did_hash.end(); iter++)
    {
        frelvd << iter->first << " " << iter->second << endl;
    }
    // output did_vid_hash
    unordered_map<int, vector<int>>::iterator iter2;
    for (iter2 = did_vid_hash.begin(); iter2 != did_vid_hash.end(); iter2++)
    {
        freldv << iter2->first << " ";
        vector<int> vid_vec = iter2->second;
        freldv << vid_vec.size() << " ";
        for (int i=0; i<vid_vec.size(); i++)
            freldv << vid_vec[i] << " ";
        freldv << endl;
    }
    frelvd.close();
    freldv.close();
}

int main(int argc, char** argv)
{
    if (argc<2){
        cout << "Usage: doc_num" << endl;
        return 1;
    }
    VERSION_NO = 1;
    doc_number = new int[VERSION_NO];
    for (int i=0; i<VERSION_NO; i++)
        doc_number[i] = atoi(argv[i+1]);
    init();
    // work on one version each time
    for (int i=0; i<VERSION_NO; i++)
        do_version(i);
    output_index();
    output_dvrelation();
    cout << "total duplicate fragments number is "<< total_found_fragment << endl;
    delete []doc_number;
    return 0;
}

