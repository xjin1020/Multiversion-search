#include <iostream>
#include <cstdlib>
#include <sys/time.h>

using namespace std;

#define L 1000000
char a[L];
char b[L];
char c[L];

int main()
{
    for (int i=0; i<L; i++){
        a[L] = i*rand()%128;
        b[L] = i*rand()%128;
    }
    struct timeval dwstart, dwend;
    double dwtime=0;
    gettimeofday(&dwstart, NULL);
    for (int i=0; i<L; i++)
        c[L] = a[L] & b[L];
    gettimeofday(&dwend, NULL);
    dwtime = 1000000.0*(dwend.tv_sec-dwstart.tv_sec)+(dwend.tv_usec-dwstart.tv_usec);
    cout << "Byte AND takes " << dwtime/L << "us." << endl;
    return 0;
}
