//
// Created by Kyoseung Koo on 2021/05/05.
//

#include "../../include/Connection/OrientConnection.h"
#include "../../include/Session/OrientSession.h"

OrientConnection::OrientConnection(const string& url, const string& db, const string& username, const string& password)
    : Connection(url, db, username, password) {
    cpr::Response r = cpr::Get(cpr::Url{"http://" + url + "/connect/" + db + "/"},
                                 cpr::Authentication{string(username), string(password)});
    if (r.status_code != 200) assert("Creating OrientConnection failed!");
}

Cursor* OrientConnection::exec(string query) {
    auto session = new OrientSession(url, username, password, query);
    auto cursor = new Cursor(session);

    cursors.push_back(cursor);
    sessions.push_back(session);

    return cursor;
}
