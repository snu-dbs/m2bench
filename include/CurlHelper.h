//
// Created by Kyoseung Koo on 2021/05/05.
//

#ifndef M2BENCH_AO_CURLHELPER_H
#define M2BENCH_AO_CURLHELPER_H

#include <iostream>
#include <iomanip>
#include <string>

#include <cpr/cpr.h>

using namespace std;

class CurlHelper {
public:
    static string spaceReplace(const string &input);
    static string replace(const string &input);
};


#endif //M2BENCH_AO_CURLHELPER_H
