//
// Created by Kyoseung Koo on 2021/05/05.
//

#ifndef M2BENCH_AO_CONNECTION_H
#define M2BENCH_AO_CONNECTION_H

#include <list>
#include <iostream>
#include <utility>

#include <nlohmann/json.hpp>

#include "Cursor.h"

using namespace std;
using json = nlohmann::json;

class Connection {
public:
    Connection(string url, string db, string username, string password):
            url(std::move(url)), db(std::move(db)), username(std::move(username)), password(std::move(password)) {};
    virtual unique_ptr<Cursor> exec(string query) = 0;

protected:
    string url, db, username, password;         // server info
};


#endif //M2BENCH_AO_CONNECTION_H
