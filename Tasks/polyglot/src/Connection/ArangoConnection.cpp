//
// Created by Kyoseung Koo on 2021/05/05.
//

#include "Connection/ArangoConnection.h"
#include "Session/ArangoSession.h"

unique_ptr<ArangoConnection>
ArangoConnection::getConnection(const string &url, const string &db, const string &username, const string &password) {
    unique_ptr<ArangoConnection> ret(new ArangoConnection(url, db, username, password));
    return ret;
}

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

unique_ptr<Cursor> ArangoConnection::exec(string query) {
    unique_ptr<Session> session(new ArangoSession(url, jwt, query));
    unique_ptr<Cursor> cursor(new Cursor(move(session)));
    return cursor;
}

