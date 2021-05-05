//
// Created by Kyoseung Koo on 2021/05/05.
//

#ifndef M2BENCH_AO_ARANGOCONNECTION_H
#define M2BENCH_AO_ARANGOCONNECTION_H

#include <string>
#include <cpr/cpr.h>
#include "Connection.h"

using namespace std;

class ArangoConnection: Connection {
public:
    ArangoConnection(const string& url, string db, const string& username, const string& password);
    Cursor* exec(string query);

private:
    string jwt;
};


#endif //M2BENCH_AO_ARANGOCONNECTION_H
