//
// Created by morrigan on 13/06/17.
//

#include <fstream>
#include <iostream>

#include "aim_sum_r.h"

using namespace std;

void AimSumR::read_file(string path){
    critical_points.empty();

    ifstream file(path);
    string line;

    while(getline(file,line)) {
        cout << line << endl;
    }
}