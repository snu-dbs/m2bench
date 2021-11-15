//
// Created by Kyoseung Koo on 2021/05/05.
//

#include "Connection/OrientConnection.h"
#include "Session/OrientSession.h"

OrientConnection::OrientConnection(const string& url, const string& db, const string& username, const string& password)
    : Connection(url, db, username, password) {
    cpr::Response r = cpr::Get(cpr::Url{"http://" + url + "/connect/" + db + "/"},
                               cpr::Authentication{string(username), string(password)});
    if (r.status_code != 200) assert("Creating OrientConnection failed!");
}

unique_ptr<Cursor> OrientConnection::exec(string query) {
    unique_ptr<Session> session(new OrientSession(url, username, password, query));
    unique_ptr<Cursor> cursor(new Cursor(move(session)));
    return cursor;
}
