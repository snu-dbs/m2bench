//
// Created by Kyoseung Koo on 2021/05/05.
//

#ifndef M2BENCH_AO_ORIENTCONNECTION_H
#define M2BENCH_AO_ORIENTCONNECTION_H

#include <string>
#include <cpr/cpr.h>

#include "Connection.h"

class OrientConnection: Connection {
public:
    OrientConnection(const string& url, const string& db, const string& username, const string& password);
    unique_ptr<Cursor> exec(string query) override;

};


#endif //M2BENCH_AO_ORIENTCONNECTION_H
