/* This program is used to choose best parameters for option D Cluster mode
 * Author: Qinghao Wu
 * Date: Feb.29, 2016 
 */
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <cstdlib>

#define MAX_QUERY 100
#define MAX_CLUSTER 200
#define MAX_QUERY_LENGTH 10

using namespace std;

const int query_num = 50;

int f[MAX_QUERY][MAX_CLUSTER][MAX_QUERY_LENGTH];
float timeA[MAX_QUERY][MAX_CLUSTER][MAX_QUERY_LENGTH];
float timeC[MAX_QUERY][MAX_CLUSTER][MAX_QUERY_LENGTH];

int main()
{
    ifstream finA, finC;
    finA.open("outTestClusterA_vector");
    finC.open("outTestClusterC_vector");
    int token, token2;
    float ftoken;
    int total = 0;
    int chooseA = 0;
    int query_length[MAX_QUERY], cluster_num[MAX_QUERY];
    float optimal=0, totaltimeA=0, totaltimeC=0, totaltimeD=0;
    for (int i=0; i<query_num; i++){
        finA >> token; //-99999
        finC >> token2; // -99999
            //cout << token << " " << token2 << endl;
        if (token != -99999 || token2 != -99999)
        {
            cout << "Not -99999!" << endl;
            cout << token << " " << token2 << endl;
            exit(1);
        }
        finA >> query_length[i] >> cluster_num[i]; // read query_length, cluster_num
        finC >> query_length[i] >> cluster_num[i]; // read query_length, cluster_num
        for (int j=0; j<cluster_num[i]; j++){
            for (int k=0; k<query_length[i]; k++){
                finA >> f[i][j][k] >> timeA[i][j][k];
                finC >> f[i][j][k] >> timeC[i][j][k];
                totaltimeA += timeA[i][j][k];
                totaltimeC += timeC[i][j][k];
		total++;
                chooseA = chooseA + f[i][j][k];
                if (timeA[i][j][k] < timeC[i][j][k])
                    optimal += timeA[i][j][k];
                else
                    optimal += timeC[i][j][k];
            }
        }
        for (int j=0; j<query_length[i]; j++){
            finA >> ftoken;
            finC >> ftoken;
        }
        for (int j=0; j<query_length[i]; j++){
            finA >> token;
            finC >> token;
        }
    }
    finA.close();
    finC.close();
    cout << "Average timeA: " << totaltimeA/query_num << endl;
    cout << "Average timeD: " << totaltimeC/query_num << endl;
    cout << "Optimal timeD: " << optimal/query_num << endl;
    //cout << "Choose OptionA: " << chooseA << endl;
    //cout << "Choose OptionC: " << total - chooseA << endl;
}
