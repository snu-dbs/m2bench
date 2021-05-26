//
// Created by Kyoseung Koo on 2021/05/05.
//

#ifndef M2BENCH_AO_ARANGOSESSION_H
#define M2BENCH_AO_ARANGOSESSION_H

#include <string>
#include <cpr/cpr.h>

#include "Session.h"

class ArangoSession : public Session {
public:
    ArangoSession(string url, string jwt, string query);
    vector<json> fetch() override;
    bool isDone() override;

private:
    string id;      // arango cursor id
    string url, jwt, query;
    bool isFirst = true;
    bool done = false;
};


#endif //M2BENCH_AO_ARANGOSESSION_H
