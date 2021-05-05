//
// Created by Kyoseung Koo on 2021/05/05.
//

#ifndef M2BENCH_AO_CONNECTION_H
#define M2BENCH_AO_CONNECTION_H

#include <list>
#include <iostream>
#include <nlohmann/json.hpp>
#include "../Cursor.h"

using namespace std;
using json = nlohmann::json;

class Connection {
public:
    Connection(string url, string db, string username, string password): url(url), db(db), username(username), password(password) {};
    ~Connection() {
        for (auto *it :cursors) delete it;
        for (auto *it :sessions) delete it;
    };
    virtual Cursor* exec(string query) = 0;

protected:
    string url, db, username, password;         // server info

    // maintain to delete
    list<Cursor*> cursors;
    list<Session*> sessions;
};


#endif //M2BENCH_AO_CONNECTION_H
