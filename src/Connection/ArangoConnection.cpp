//
// Created by Kyoseung Koo on 2021/05/05.
//

#include "../../include/Connection/ArangoConnection.h"
#include "../../include/Session/ArangoSession.h"

ArangoConnection::ArangoConnection(const string& url, string db, const string& username, const string& password)
    : Connection(url, db, username, password) {
    json aAuth = json::parse(R"({"username": ")" + username + R"(", "password": ")" + password + "\"}");
    cpr::Response r = cpr::Post(cpr::Url{"http://" + url + "/_open/auth"},
                                cpr::Header{{"accept", "application/json"}},
                                cpr::Body{aAuth.dump()});

    if (r.status_code != 200) assert("Creating ArangoConnection failed!");

    json j = json::parse(r.text);
    jwt = j["jwt"].get<string>();
}

Cursor* ArangoConnection::exec(string query) {
    auto session = new ArangoSession(url, jwt, query);
    auto cursor = new Cursor(session);

    cursors.push_back(cursor);
    sessions.push_back(session);

    return cursor;
}
