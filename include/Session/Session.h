//
// Created by Kyoseung Koo on 2021/05/05.
//

#ifndef M2BENCH_AO_SESSION_H
#define M2BENCH_AO_SESSION_H

#include <vector>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace std;

class Session {
public:
    virtual vector<json> fetch() = 0;
    virtual bool isDone() = 0;
};


#endif //M2BENCH_AO_SESSION_H
