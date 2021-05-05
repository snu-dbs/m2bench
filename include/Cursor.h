//
// Created by Kyoseung Koo on 2021/05/05.
//

#ifndef M2BENCH_AO_CURSOR_H
#define M2BENCH_AO_CURSOR_H

#include <vector>
#include <queue>
#include <nlohmann/json.hpp>
#include "Session/Session.h"

using json = nlohmann::json;
using namespace std;

class Cursor {
public:
    Cursor(Session* session);
    bool hasNext();
    json next();
private:
    Session *session;
    queue<json> q;

    void push_vector(const vector<json>& v);
};


#endif //M2BENCH_AO_CURSOR_H
