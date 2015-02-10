/* This program is used to choose best parameters for option D
 * Author: Xin Jin
 * Date: Jan.15, 2015
 */
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <cstdlib>

using namespace std;

vector<string> &split(const string &s, char delim, vector<string> &elems) {
    stringstream ss(s);
    string item;
    while (getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}


vector<string> split(const string &s, char delim) {
    vector<string> elems;
    split(s, delim, elems);
    return elems;
}

int main()
{
    ifstream fin;
    fin.open("ft");
    const int query_num = 50;
    int i;
    string linef, lineA, lineC;
    int f[100][10];
    int length[100];
    float timeA[100][10];
    float timeC[100][10];

    for (i=0; i<query_num; i++)
    {
        getline(fin, linef);
        getline(fin, lineA);
        getline(fin, lineC);
        vector<string> f_vec = split(linef, '\t');
        vector<string> A_vec = split(lineA, '\t');
        vector<string> C_vec = split(lineC, '\t');
        length[i] = f_vec.size();
        for (int j=0; j<f_vec.size(); j++)
        {
            f[i][j] = atoi(f_vec[j].c_str());
            timeA[i][j] = atof(A_vec[j].c_str());
            timeC[i][j] = atof(C_vec[j].c_str());
        }
    }
    fin.close();
    int t2, t3, t4, t5, t6, t;
    int best2, best3, best4, best5, best6;
    float bestAvg=99999;
    float timeD;
    int chooseA=0, chooseC=0;
    int best_chooseA=0, best_chooseC=0;
    float A_benefit, C_benefit, best_A_benefit, best_C_benefit;
    for (t2=0; t2<10; t2++)
        for (t3=0; t3<10; t3++)
            for (t4=0; t4<10; t4++)
                for (t5=0; t5<15; t5++)
                    for (t6=0; t6<22; t6++)
                    {
                        timeD=0;
                        chooseA=chooseC=0;
                        A_benefit = C_benefit = 0;
                        for (i=0; i<query_num; i++){
                            switch (length[i]){
                                case 2: t=t2; break;
                                case 3: t=t3; break;
                                case 4: t=t4; break;
                                case 5: t=t5; break;
                                case 6: t=t6; break;
                                default: break;
                            }
                            for (int j=0; j<length[i]; j++){
                                if (f[i][j] < t){
                                // choose A:
                                    timeD += timeA[i][j];
                                    chooseA++;
                                    A_benefit += timeC[i][j] - timeA[i][j];
                                }
                                else{
                                    timeD += timeC[i][j];
                                    chooseC++;
                                    C_benefit += timeA[i][j] - timeC[i][j];
                                }
                            }
                        }
                        float avg = timeD/query_num;
                        if (avg < bestAvg){
                            best2 = t2;
                            best3 = t3;
                            best4 = t4;
                            best5 = t5;
                            best6 = t6;
                            bestAvg = avg;
                            best_chooseA = chooseA;
                            best_chooseC = chooseC;
                            best_A_benefit = A_benefit;
                            best_C_benefit = C_benefit;
                        }
                    }
    cout << "Best_avg = " << bestAvg << " when t2, t3, t4, t5, t6 = " << best2 << "," << best3 << "," << best4 << "," << best5 << "," << best6 << endl;
    cout << "ChooseA: " << best_chooseA << "\tbenefit: " << best_A_benefit/best_chooseA << endl;
    cout << "ChooseC: " << best_chooseC << "\tbenefit: " << best_C_benefit/best_chooseC << endl;
    return 0;
}
