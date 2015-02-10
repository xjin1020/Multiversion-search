/* This program is used to convert from traditional reuse table to forward reuse table
 * Author: Xin Jin
 * Dec. 10. 2014
 */

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

using namespace std;

struct ForwardPosting
{
    int fid;
    int offset;
};

const int MAX_VID=400000; // need to be >= max vid in "index.txt"
char *did; // which cluster

vector<ForwardPosting> v[MAX_VID]; // this saves the forward posting for each page

// initialize all vectors to empty
void init()
{
    int i;
    for (i=0; i<MAX_VID; i++)
        v[i].clear();
}

// read traditional reuse table
void read()
{
    ifstream fin;
    string filename;
    filename = "cluster/"+string(did)+"/index.txt";
    fin.open(filename.c_str());
    int fid, size, i;
    int offset;
    int vid;
    // Since in each loop iteration, fid is visited from small to large, we can guarantee that the order it's pushed into v[] is also from small to large. Therefore, in "forward.txt", in each posting, the fid is sorted.
    while (fin >> fid)
    {
        if (fid < 0) // -99999
            return; // finish reading traditional reuse table
        fin >> size;
        for (i=0; i<size; i++){
            fin >> vid >> offset;
            ForwardPosting e;
            e.fid = fid;
            e.offset = offset;
            v[vid].push_back(e);
        }
    }
    fin.close();
}

// convert the traditional reuse table to a forward reuse table
void convert()
{
    // statistics for fragments number in one page
    int min=99999;
    int max=0;
    int count=0;
    double avg=0;
    int id;
    ofstream fout;
    string filename;
    filename = "cluster/"+string(did)+"/forward.txt";
    fout.open(filename.c_str());
    int i, j;
    for (i=0; i<MAX_VID; i++)
        if (v[i].size() > 0){
            if (v[i].size() < min)
                min = v[i].size();
            if (v[i].size() > max){
                max = v[i].size();
                id = i;
            }
            count++;
            avg += v[i].size();
            fout << i << " " << v[i].size();
            for (j=0; j<v[i].size(); j++){
                fout << " " << v[i][j].fid << " " << v[i][j].offset;
            }
            fout << endl;
        }
    fout.close();
    cout << "max fragments at page" << id << " number: " << max << endl;
    cout << "min fragments number: " << min << endl;
    cout << "avg fragments number: " << avg/count << endl;
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        cout << "need one argument" << endl;
        return 1;
    }
    did = argv[1];
    init();
    read();
    convert();
    return 0;
}
