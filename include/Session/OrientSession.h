//
// Created by Kyoseung Koo on 2021/05/05.
//

#ifndef M2BENCH_AO_ORIENTSESSION_H
#define M2BENCH_AO_ORIENTSESSION_H

#include <string>
#include <cpr/cpr.h>

#include "Session.h"

class OrientSession : public Session {
public:
    OrientSession(string url, string username, string password, string query);
    vector<json> fetch() override;
    bool isDone() override;

private:
    string id;      // arango cursor id
    string url, username, password, query;
    bool done = false;
};


#endif //M2BENCH_AO_ORIENTSESSION_H
