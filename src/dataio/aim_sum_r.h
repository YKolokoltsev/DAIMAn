//
// Created by morrigan on 13/06/17.
//

#ifndef DAIMAN_AIMSUMR_H
#define DAIMAN_AIMSUMR_H

/*
 * AimALL *.sum output file parser,
 * for the moment it only reads a set of critical points
 * filling out some selected properties
 */

#include <list>
#include <string>

#include "critical_point.hpp"

using namespace std;

class AimSumR {
public:
    void read_file(string);
    list<CP> critical_points;
};


#endif //DAIMAN_AIMSUMR_H
