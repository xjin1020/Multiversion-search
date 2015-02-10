#include <fstream>

using namespace std;

int main()
{
    ifstream fin;
    ofstream fout;
    fin.open("vidlist.txt");
    int a;
    fout.open("1000.txt");
    for (int i=0; i<1000; i++){
        fin >> a;
        fout << a << " ";
    }
    fin.close();
    fout.close();
}
