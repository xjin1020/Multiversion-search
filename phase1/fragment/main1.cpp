#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "neverlost_util.h"

using namespace std;

/*
For your reference:
class NeverLostUtil {
public:
    
    static string readFileContent(const char* filename);
    static void writeFileContent(const char* filename, const string& content_str);


    // chunking
    static void chunkContent(const string& content_str, const bool var, const uint32_t chunksize, vector<string> &chunks);
};

*/

char hash_string(string s)
{
    int result=0;
    for (int i=0; i<s.size(); i++)
    {
        result += s[i];
        result %= 127;
    }
    return (char)(result+1);
}

void hash_content(char *result_hash, vector<string>& v)
{
    int i;
    for (i=0; i<v.size(); i++)
        result_hash[i] = hash_string(v[i]);
    result_hash[i] = '\0';
}

void deal_with_doc(string& content, int doc_id, string& output_base)
{
    /* Deal with each document:
     * (1) Firstly we need to hash each string to a char.
     * (2) Then we partition on this char array.
     * (3) Lastly we restore the fragment on doc words.
     */ 
    // Step(1):
    // find title
    size_t found = content.find_first_of("\t");
    size_t len = found + 1;
    string title;
    title = content.substr(0, len);
    content = content.substr(len); // don't print title again in the second line
    // read strings of a doc
    istringstream iss(content);
    vector<string> v;
    v.clear();
    string word;
    while (iss >> word)
    {
        v.push_back(word);
    }
    int word_number = v.size();
    char *result_hash = new char[word_number+1];
    hash_content(result_hash, v);
    
    // Step(2):
    string char_doc(result_hash);
    vector<string> chunks;
	NeverLostUtil::chunkContent(char_doc, true, 30, chunks); 
    
    // Step(3):
    ofstream fout;
    int cur_start=0;
    
    // output frags into one file
    ostringstream oss;
    oss << output_base << doc_id << ".txt";
    string output_file = oss.str();
    fout.open(output_file.c_str());
    // print title as the first line
    fout << title << endl;
    for (int i=0; i<chunks.size(); i++) 
    {
        string fragment="";
        for (int j=0; j<chunks[i].size(); j++)
        {
            fragment += (j==0?"":" ");
            fragment += v[cur_start++];
        }
        fout << fragment << endl;
    }
    fout.close();
    delete []result_hash;
}

int main(int argc, char** argv) {
    if (argc != 3)
    {
        cout << "need 2 arguments: filename result_folder_name" << endl;
        exit(1);
    }
	//string filename = "data/10k_1304_A.txt";
    string filename = string(argv[1]);
    string output_base = string(argv[2]);
	string testdata_content;
    ifstream fin;
    fin.open(filename.c_str());
    string line; // each line of the input file is a document
    int doc_num=0;
    while(getline(fin, line))
    {
	// deal with each document
        if(doc_num == 174750){
	    	exit(1);
	    }
        deal_with_doc(line, doc_num, output_base);
        //deal_with_doc(line, doc_num, output_base);
        doc_num++;
    }
    fin.close();
    return 0;
}
